#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <iostream>

class InputController : public rclcpp::Node{ 
    public:
    InputController(): Node("input_controller"){ 
        t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        t2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
        
        stop_sub_ = this->create_subscription<turtlesim::msg::Pose>("/stop_message", 10, std::bind(&DistanceController::turtle2_pose_callback, this, _1));


        input_timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&InputController::input_timer_callback, this));
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

                //moving_turtle.data = n_turtle;

                if(n_turtle == 1){
                    t1_vel_pub_->publish(vel_input);
                    //id_turtle_managed_pub_->publish(moving_turtle);
                }else if(n_turtle ==2){
                    t2_vel_pub_->publish(vel_input);
                    //id_turtle_managed_pub_->publish(moving_turtle);
                }

            }else{
                std::cout<<"Devi inserire 1 o 2!\n";
            }


        } 

        //TIMERS
        rclcpp::TimerBase::SharedPtr input_timer_;

        //PUBLISHERS
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;

        //SUBSCRIBERS

        //VARIABLES
        int n_turtle;
        geometry_msgs::msg::Twist vel_input;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<InputController>());
    rclcpp::shutdown();
    return 0;
}
 
