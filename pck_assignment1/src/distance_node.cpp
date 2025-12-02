#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "turtlesim/msg/pose.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/int32.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <math.h>
using std::placeholders::_1;

class DistanceController: public rclcpp::Node{
    public:
        DistanceController(): Node("distance_controller"){
            
            //TIMERS
            check_timer = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&DistanceController::distance_boundaries_timer, this));

            //PUBLISHERS
            distance_pub_ = this->create_publisher<std_msgs::msg::Float32>("/distance", 10);
            t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            t2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
            reverse_state_pub_ = this->create_publisher<std_msgs::msg::Int32>("/is_reversing", 10);

            //SUBSCRIBERS
            t1_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&DistanceController::turtle1_pose_callback, this, _1));
            t2_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&DistanceController::turtle2_pose_callback, this, _1));
            t1_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10, std::bind(&DistanceController::turtle1_vel_callback, this, _1));
            t2_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10, std::bind(&DistanceController::turtle2_vel_callback, this, _1));
            id_turtle_managed_sub_ = this->create_subscription<std_msgs::msg::Int32>("/id_turtle_moved", 10, std::bind(&DistanceController::id_turtle_callback, this, _1));

            //VARIABLES
            stop_turtle.linear.x = 0.0;
            stop_turtle.angular.z = 0.0;
            id_turtle.data = 0;
            is_reversing_t1 = false;
            is_reversing_t2 = false;
        }
    private:

        void stop_and_reset() {
            if(id_turtle.data == 1){
                t1_vel_pub_->publish(stop_turtle);
                is_reversing_t1 = false;
                reverse_timer_t1_.reset();
            }else if(id_turtle.data == 2){
                t2_vel_pub_->publish(stop_turtle);
                is_reversing_t2 = false;
                reverse_timer_t2_.reset();
            }
            id_turtle.data=0;
            reverse_state_pub_->publish(id_turtle);
        }

        geometry_msgs::msg::Twist check_direction_t1(){
            geometry_msgs::msg::Twist reverse_turtle1_vel;
            if(t1_actual_vel.linear.x < 0){
                reverse_turtle1_vel.linear.x = 1;
            }else{
                reverse_turtle1_vel.linear.x = -1;
            } 
            return reverse_turtle1_vel;
        }

        geometry_msgs::msg::Twist check_direction_t2(){
            geometry_msgs::msg::Twist reverse_turtle2_vel;
            if(t2_actual_vel.linear.x < 0){
                reverse_turtle2_vel.linear.x = 1;
            }else{
                reverse_turtle2_vel.linear.x = -1;
            } 
            return reverse_turtle2_vel;
        }
        
        
        void distance_boundaries_timer(){
            geometry_msgs::msg::Twist reverse_vel;

            distance.data = sqrt(pow((t2_pose.x-t1_pose.x),2) + pow((t2_pose.y-t1_pose.y),2));
            std::cout << "Distanza:" << distance.data <<std::endl;
            distance_pub_->publish(distance);

            if(distance.data < 1.0){
                if(id_turtle.data == 1 && !is_reversing_t1){
                    is_reversing_t1 = true;
                    reverse_state_pub_->publish(id_turtle);
                    reverse_vel = check_direction_t1();
                    t1_vel_pub_->publish(reverse_vel);

                    reverse_timer_t1_ = this->create_wall_timer(
                        std::chrono::milliseconds(1000),
                        std::bind(&DistanceController::stop_and_reset, this)
                    );

                }
                if(id_turtle.data == 2 && !is_reversing_t2){
                    is_reversing_t2 = true;
                    reverse_state_pub_->publish(id_turtle);
                    reverse_vel = check_direction_t2();
                    t2_vel_pub_->publish(check_direction_t1());

                    reverse_timer_t2_ = this->create_wall_timer(
                        std::chrono::milliseconds(1000),
                        std::bind(&DistanceController::stop_and_reset, this)
                    );
                }
            }

            if(t1_pose.x > 10.0 || t1_pose.y > 10.0 || t1_pose.x < 1.0 || t1_pose.y < 1.0){
                is_reversing_t1 = true;
                reverse_state_pub_->publish(id_turtle);
                reverse_vel = check_direction_t1();
                t1_vel_pub_->publish(reverse_vel);

                reverse_timer_t1_= this->create_wall_timer(
                    std::chrono::milliseconds(1000),
                    std::bind(&DistanceController::stop_and_reset, this)
                );
            }

            if(t2_pose.x > 10.0 || t2_pose.y > 10.0 || t2_pose.x < 1.0 || t2_pose.y < 1.0){
                is_reversing_t2 = true;
                reverse_state_pub_->publish(id_turtle);
                reverse_vel = check_direction_t2();
                t2_vel_pub_->publish(reverse_vel);

                reverse_timer_t2_ = this->create_wall_timer(
                    std::chrono::milliseconds(1000),
                    std::bind(&DistanceController::stop_and_reset, this)
                );
            }
        }

        void id_turtle_callback(const std_msgs::msg::Int32::SharedPtr msg){
            id_turtle.data = msg->data;
        }

        void turtle1_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg){
            if (!is_reversing_t1){
                t1_actual_vel.linear.x = msg->linear.x;
                t1_actual_vel.angular.z = msg->linear.z;
            }
        }

        void turtle2_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg){
            if (!is_reversing_t2){
                t2_actual_vel.linear.x = msg->linear.x;
                t2_actual_vel.angular.z = msg->linear.z;
            }
        }

        void turtle1_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t1_pose.x = msg->x;
            t1_pose.y = msg->y;
        }

        void turtle2_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t2_pose.x = msg->x;
            t2_pose.y = msg->y;
        }
        //TIMERS
        //rclcpp::TimerBase::SharedPtr distance_timer_;
        //rclcpp::TimerBase::SharedPtr boundaries_timer_;
        rclcpp::TimerBase::SharedPtr check_timer;
        rclcpp::TimerBase::SharedPtr reverse_timer_t1_;
        rclcpp::TimerBase::SharedPtr reverse_timer_t2_;

        //PUBLISHER
        rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr distance_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;
        rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr stop_msg_pub_;
        rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr reverse_state_pub_;
        
        //SUBSCRIBERS
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t1_pose_sub_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t2_pose_sub_;
        rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr t1_vel_sub_;
        rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr t2_vel_sub_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr id_turtle_managed_sub_;

        //VARIABLES
        geometry_msgs::msg::Twist stop_turtle;
        geometry_msgs::msg::Twist t1_actual_vel;
        geometry_msgs::msg::Twist t2_actual_vel;
        turtlesim::msg::Pose t1_pose;
        turtlesim::msg::Pose t2_pose;
        std_msgs::msg::Float32 distance;
        std_msgs::msg::Int32 id_turtle;
        bool is_reversing_t1;
        bool is_reversing_t2;

        
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistanceController>());
    rclcpp::shutdown();
    return 0;
}