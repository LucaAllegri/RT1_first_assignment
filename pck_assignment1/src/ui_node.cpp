#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/bool.hpp"
#include "std_msgs/msg/int32.hpp"
#include <iostream>
using std::placeholders::_1;

class InputController : public rclcpp::Node{ 
    public:
    InputController(): Node("input_controller"){ 

        //TIMERS 
        input_timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&InputController::input_timer_callback, this));
        //stop_timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&InputController::stop_timer_callback, this));

        //PUBLISHERS
        t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        t2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
        id_turtle_managed_pub_ = this->create_publisher<std_msgs::msg::Int32>("/id_turtle_moved", 10);
        
        //SUBSCRIBERS
        //stop_sub_ = this->create_subscription<std_msgs::msg::Bool>("/stop_message", 10, std::bind(&InputController::stop_callback, this, _1));

        //VARIABLES
        stop_turtle.linear.x = 0.0;
        stop_turtle.angular.z = 0.0;
    }
    
    private:

        void input_timer_callback(){
            std::cout<< "Quale tartaruga vuoi muovere?\n1) Turtle 1\n2) Turtle 2\n:";
            std::cin >> n_turtle;
            if(n_turtle == 1 || n_turtle ==2){
                std::cout<< "Inserisci Velocità Lineare\nx:";
                std::cin >> vel_input.linear.x;

                std::cout<< "Inserisci Velocità Angolare\nz:";
                std::cin >> vel_input.angular.z;

                moved_turtle.data = n_turtle;

                if(n_turtle == 1){
                    t1_vel_pub_->publish(vel_input);
                    id_turtle_managed_pub_->publish(moved_turtle);
                }else if(n_turtle ==2){
                    t2_vel_pub_->publish(vel_input);
                    id_turtle_managed_pub_->publish(moved_turtle);
                }

            }else{
                std::cout<<"Devi inserire 1 o 2!\n";
            }
        } 

        //TIMERS
        rclcpp::TimerBase::SharedPtr input_timer_;
        rclcpp::TimerBase::SharedPtr stop_timer_;

        //PUBLISHERS
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;
        rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr id_turtle_managed_pub_;

        //SUBSCRIBERS

        //VARIABLES
        int n_turtle;
        geometry_msgs::msg::Twist vel_input;
        geometry_msgs::msg::Twist stop_turtle;
        //std_msgs::msg::Bool stop_message;
        std_msgs::msg::Int32 moved_turtle;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<InputController>());
    rclcpp::shutdown();
    return 0;
}
 
