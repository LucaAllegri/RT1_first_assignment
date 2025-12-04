#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/int32.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <math.h>
using std::placeholders::_1;

class DistanceController: public rclcpp::Node{
    public:
        DistanceController(): Node("distance_controller"){
            
            //TIMERS
            check_timer = this->create_wall_timer(std::chrono::milliseconds(150), std::bind(&DistanceController::distance_boundaries_timer, this));

            //PUBLISHERS
            distance_pub_ = this->create_publisher<std_msgs::msg::Float32>("/distance", 10);
            t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            t2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
            reverse_state_pub_ = this->create_publisher<std_msgs::msg::Int32>("/is_reversing", 10);

            //SUBSCRIBERS
            t1_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&DistanceController::turtle1_pose_callback, this, _1));
            t2_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&DistanceController::turtle2_pose_callback, this, _1));
            intermediate_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>("/intermediate_vel", 10, std::bind(&DistanceController::intermediate_vel_callback, this, _1));
            id_turtle_managed_sub_ = this->create_subscription<std_msgs::msg::Int32>("/id_turtle_moved", 10, std::bind(&DistanceController::id_turtle_callback, this, _1));

            //VARIABLES
            stop_turtle.linear.x = 0.0;
            stop_turtle.angular.z = 0.0;
            command_input.linear.x = 0.0;
            command_input.angular.z = 0.0;
            id_turtle.data = 0;
            is_reversing_t1 = false;
            is_reversing_t2 = false;
            t1_pose_received_ = false;
            t2_pose_received_ = false;
        }
    private:

        void stop() {
            if(id_turtle.data == 1){
                t1_vel_pub_->publish(stop_turtle);
                is_reversing_t1 = false;
            }else if(id_turtle.data == 2){
                t2_vel_pub_->publish(stop_turtle);
                is_reversing_t2 = false;

            }
            id_turtle.data=0;
            reverse_state_pub_->publish(id_turtle);
        }

        geometry_msgs::msg::Twist check_direction_turtle(){
            geometry_msgs::msg::Twist reverse_turtle_vel;
            if(command_input.linear.x < 0){
                reverse_turtle_vel.linear.x = 1;
            }else{
                reverse_turtle_vel.linear.x = -1;
            }
            if(command_input.angular.z != 0){
                reverse_turtle_vel.angular.z = -command_input.angular.z;
            }
            return reverse_turtle_vel;
        }

        bool is_t1_in_danger(){
            bool boundary_danger = t1_pose.x > 10.0 || t1_pose.y > 10.0 || t1_pose.x < 1.0 || t1_pose.y < 1.0;
            bool proximity_danger = (id_turtle.data == 1 && distance.data < 1.0);
            return boundary_danger || proximity_danger;
        }

        bool is_t2_in_danger(){
            bool boundary_danger = t2_pose.x > 10.0 || t2_pose.y > 10.0 || t2_pose.x < 1.0 || t2_pose.y < 1.0;
            bool proximity_danger = (id_turtle.data == 2 && distance.data < 1.0);
            return boundary_danger || proximity_danger;
        }
        
        void distance_boundaries_timer(){

            if (!t1_pose_received_ || !t2_pose_received_) {
                return; 
            }
            geometry_msgs::msg::Twist reverse_vel;

            distance.data = sqrt(pow((t2_pose.x-t1_pose.x),2) + pow((t2_pose.y-t1_pose.y),2));
            std::cout << "Distance:" << distance.data <<std::endl;
            distance_pub_->publish(distance);


            if(is_t1_in_danger()){
                if (!is_reversing_t1) {
                    is_reversing_t1 = true;    //problem if turtle doesn't come back in one only timer's step
                    reverse_state_pub_->publish(id_turtle);
                    reverse_vel = check_direction_turtle();
                    t1_vel_pub_->publish(reverse_vel);
                }
            }else{
                if(is_reversing_t1){
                    stop();
                }
            }

            if(is_t2_in_danger()){
                if (!is_reversing_t2) {
                    is_reversing_t2 = true;    //problem if turtle doesn't come back in one only timer's step
                    reverse_state_pub_->publish(id_turtle);
                    reverse_vel = check_direction_turtle();
                    t2_vel_pub_->publish(reverse_vel);
                }
            }else{
                if(is_reversing_t2){
                    stop();
                }
            }
        }

        void intermediate_vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg){
            command_input = *msg;
            if(id_turtle.data == 1 && !is_reversing_t1) {
                t1_vel_pub_->publish(command_input);
            } else if (id_turtle.data == 2 && !is_reversing_t2) {
                t2_vel_pub_->publish(command_input);
            }
        }

        void id_turtle_callback(const std_msgs::msg::Int32::SharedPtr msg){
            id_turtle.data = msg->data;
        }

        void turtle1_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t1_pose.x = msg->x;
            t1_pose.y = msg->y;
            t1_pose_received_ = true;
        }

        void turtle2_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t2_pose.x = msg->x;
            t2_pose.y = msg->y;
            t2_pose_received_ = true;
        }
        //TIMERS
        rclcpp::TimerBase::SharedPtr check_timer;

        //PUBLISHER
        rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr distance_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;
        rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr reverse_state_pub_;
        
        //SUBSCRIBERS
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t1_pose_sub_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t2_pose_sub_;
        rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr intermediate_vel_sub_;
        rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr id_turtle_managed_sub_;

        //VARIABLES
        geometry_msgs::msg::Twist command_input;
        geometry_msgs::msg::Twist stop_turtle;
        std_msgs::msg::Float32 distance;
        std_msgs::msg::Int32 id_turtle;
        turtlesim::msg::Pose t1_pose;
        turtlesim::msg::Pose t2_pose;
        bool t1_pose_received_;
        bool t2_pose_received_;
        bool is_reversing_t1;
        bool is_reversing_t2;
        
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistanceController>());
    rclcpp::shutdown();
    return 0;
}