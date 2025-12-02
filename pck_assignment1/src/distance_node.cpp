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
            check_timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&DistanceController::distance_boundaries_timer_callback, this));

            //PUBLISHERS
            distance_pub_ = this->create_publisher<std_msgs::msg::Float32>("/distance", 10);
            t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);

            //SUBSCRIBERS
            t1_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&DistanceController::turtle1_pose_callback, this, _1));
            t2_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&DistanceController::turtle2_pose_callback, this, _1));
            id_turtle_managed_sub_ = this->create_subscription<std_msgs::msg::Int32>("/id_turtle_moved", 10, std::bind(&DistanceController::id_turtle_callback, this, _1));

            //VARIABLES
            stop_turtle.linear.x = 0.0;
            stop_turtle.angular.y = 0.0;
        }
    private:

        void distance_boundaries_timer_callback(){

            distance.data = sqrt(pow((t2_pose.x-t1_pose.x),2) + pow((t2_pose.y-t1_pose.y),2));
            std::cout << "Distanza:" << distance.data <<std::endl;
            distance_pub_->publish(distance);

            if(distance.data < 1.0){
                if(id_turtle.data == 1){
                    t1_vel_pub_->publish(stop_turtle);
                }
                if(id_turtle.data == 1){
                    t2_vel_pub_->publish(stop_turtle);
                }
            }

            if(t1_pose.x > 10.0 || t1_pose.y > 10.0 || t1_pose.x < 1.0 || t1_pose.y < 1.0){
                if(id_turtle.data == 1){
                    t1_vel_pub_->publish(stop_turtle);
                }
                if(id_turtle.data == 1){
                    t2_vel_pub_->publish(stop_turtle);
                }
            }

            if(t2_pose.x > 10.0 || t2_pose.y > 10.0 || t2_pose.x < 1.0 || t2_pose.y < 1.0){
                if(id_turtle.data == 1){
                    t1_vel_pub_->publish(stop_turtle);
                }
                if(id_turtle.data == 1){
                    t2_vel_pub_->publish(stop_turtle);
                }
            }
        }
        /*
        void boundaries_timer_callback(){
            if(t1_pose.x > 10.0 || t1_pose.y > 10.0 || t1_pose.x < 1.0 || t1_pose.y < 1.0){
                stop.data=true;
            }
            if(t2_pose.x > 10.0 || t2_pose.y > 10.0 || t2_pose.x < 1.0 || t2_pose.y < 1.0){
                stop.data=true;
            }
            stop_pub_->publish(stop);
        }*/

        void id_turtle_callback(const std_msgs::msg::Int32::SharedPtr msg){
            id_turtle = *msg;
        }

        void turtle1_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t1_pose = *msg;
        }

        void turtle2_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t2_pose = *msg;
        }
        //TIMERS
        rclcpp::TimerBase::SharedPtr check_timer_;

        //PUBLISHER
        rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr distance_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;
        
        //SUBSCRIBERS
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t1_pose_sub_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t2_pose_sub_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr id_turtle_managed_sub_;

        //VARIABLES
        geometry_msgs::msg::Twist stop_turtle;
        turtlesim::msg::Pose t1_pose;
        turtlesim::msg::Pose t2_pose;
        std_msgs::msg::Float32 distance;
        std_msgs::msg::Int32 id_turtle;

        
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistanceController>());
    rclcpp::shutdown();
    return 0;
}