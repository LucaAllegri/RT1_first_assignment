#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "std_msgs/msg/float32.hpp"
#include <math.h>

#include <iostream>
class Distance_Check: public rclcpp::Node{
    public:
        Distance_Check(): Node("distance_check"){
            publisher_ = this->create_publisher<std_msgs::msg::Float32>("distance", 10);
            turtle1_pose_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&TurtlesimController::turtle1_pose, this, _1));
            turtle2_pose_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&TurtlesimController::turtle2_pose, this, _1));
            timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&Distance_Check::timer_callback, this));
        }
    private:
        void timer_callback(){
            distance = sqrt((pos_t2.x-pos_t1.x)^2 + (pos_t2.y-pos_t1.y)^2);
            publisher_->publish(distance);
        }
        void turtle1_pose(const turtlesim::msg::Pose::SharedPtr msg){
            pos_t1.x = msg->x;
            pos_t1.y = msg->y;
            if(pos_t1.x > 10.0 || pos_t1.x < 1.0 || pos_t1.y > 10.0 || pos_t1.y < 1.0){

            }
        } 
        void turtle2_pose(const turtlesim::msg::Pose::SharedPtr msg){
            pos_t2.x = msg->x;
            pos_t2.y = msg->y;
            if(pos_t1.x > 10.0 || pos_t1.x < 1.0 || pos_t1.y > 10.0 || pos_t1.y < 1.0){
                
            }
        } 
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr turtle1_pose_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr turtle2_pose_;
        rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
        turtlesim::msg::Pose pos_t1;
        turtlesim::msg::Pose pos_t2;
        std_msgs::msg::Float32 distance;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Distance_Check>());
    rclcpp::shutdown();
    return 0;
}