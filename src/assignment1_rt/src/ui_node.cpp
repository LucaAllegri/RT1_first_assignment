#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <math.h>

class Input_Controller : public rclcpp::Node{
    public:
        Input_Controller(): Node("input_controller"){
            t1_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            t2_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
            timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&Distance_Check::input_loop, this));
        }

    private:
        void input_loop(){
            while(1){
                std::cout<< "Quale tartaruga vuoi muovere?\n
                             1) Turtle 1\n
                             2) Turtle 2\n
                             :";
                std::cin >> n_turtle;

                std::cout<< "Inserisci Velocità Lineare\n
                             x:"
                std::cin >> vel_input.linear.x;
                std::cout<< "\ny:"
                std::cin >> vel_input.linear.y;
                std::cout<< "\nz:"
                std::cin >> vel_input.linear.z;

                std::cout<< "Inserisci Velocità Angolare\n
                             x:"
                std::cin >> vel_input.angular.x;
                std::cout<< "\ny:"
                std::cin >> vel_input.angular.y;
                std::cout<< "\nz:"
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

        }

        void timer_callback(){
            distance = sqrt((pos_t2.x-pos_t1.x)^2 + (pos_t2.y-pos_t1.y)^2);
            publisher_->publish(distance);
        }

        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_publisher_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_publisher_;
        geometry_msgs::msg::Twist vel_input;
        geometry_msgs::msg::Twist stop_turtle = {0.0 , 0.0 , 0.0
                                                 0.0 , 0.0 , 0.0};
        int n_turtle;

}


int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Input_Controller>());
    rclcpp::shutdown();
    return 0;
}