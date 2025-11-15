#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/float32.hpp"
#include <math.h>
using std::placeholders::_1;

class Input_Controller : public rclcpp::Node{
    public:
        Input_Controller(): Node("input_controller"){
            t1_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            t2_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
            distance_sub_ = this->create_subscription<std_msgs::msg::Float32>("distance", 10, std::bind(&Input_Controller::check_distance, this, _1));
            timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&Input_Controller::input_loop, this));
            stop_turtle.linear.x = 0.0;
            stop_turtle.linear.y = 0.0;
            stop_turtle.linear.z = 0.0;

            stop_turtle.angular.x = 0.0;
            stop_turtle.angular.y = 0.0;
            stop_turtle.angular.z = 0.0;
            actual_distance =11.0;
        }

    private:
        void input_loop(){
            while(actual_distance > 1.0){
                std::cout<< "Quale tartaruga vuoi muovere?\n1) Turtle 1\n2) Turtle 2\n:";
                std::cin >> n_turtle;

                std::cout<< "Inserisci Velocità Lineare\nx:";
                std::cin >> vel_input.linear.x;
                std::cout<< "y:";
                std::cin >> vel_input.linear.y;
                std::cout<< "z:";
                std::cin >> vel_input.linear.z;

                std::cout<< "Inserisci Velocità Angolare\nx:";
                std::cin >> vel_input.angular.x;
                std::cout<< "y:";
                std::cin >> vel_input.angular.y;
                std::cout<< "z:";
                std::cin >> vel_input.angular.z;

                if(n_turtle == 1){
                    t1_publisher_->publish(vel_input);
                    t2_publisher_->publish(stop_turtle);
                }else if(n_turtle ==2){
                    t2_publisher_->publish(vel_input);
                    t1_publisher_->publish(stop_turtle);
                }else{
                    std::cout<<"Devi inserire 1 o 2!\n";
                }

            }

            std::cout<< "LE DUE TARTARUGHE SONO TROPPO VICINE!!";

        }

        void check_distance(const std_msgs::msg::Float32::SharedPtr msg){
            actual_distance = msg->data;
        }

        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_publisher_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_publisher_;
        rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr distance_sub_;
        geometry_msgs::msg::Twist vel_input;
        rclcpp::TimerBase::SharedPtr timer_;
        geometry_msgs::msg::Twist stop_turtle;
        float actual_distance;
        int n_turtle;

};


int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Input_Controller>());
    rclcpp::shutdown();
    return 0;
}