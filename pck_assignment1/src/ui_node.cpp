#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <iostream>

class InputController : public rclcpp::Node{ 
    public:
    InputController(): Node("input_controller"){ 
        t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        t2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);

        input_timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&InputController::input_timer_callback, this));
        count_ = 0;
    }
    
    private:
        void input_timer_callback(){
            message.data = "Hello, world! " + std::to_string(count_++);
            RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
            publisher_->publish(message);
        } 

        //TIMERS
        rclcpp::TimerBase::SharedPtr input_timer_;

        //PUBLISHERS
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;

        //SUBSCRIBERS

        //VARIABLES
        int count_;
        std_msgs::msg::String message;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<InputController>());
    rclcpp::shutdown();
    return 0;
}
 
