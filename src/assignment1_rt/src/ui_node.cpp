#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/bool.hpp" 
#include <math.h>
using std::placeholders::_1;

class Input_Controller : public rclcpp::Node{
    public:
        Input_Controller(): Node("input_controller"){
            t1_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            t2_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
            distance_sub_ = this->create_subscription<std_msgs::msg::Float32>("distance", 10, std::bind(&Input_Controller::check_distance, this, _1));
            boundaries_sub_ = this->create_subscription<std_msgs::msg::Bool>("boundaries", 10, std::bind(&Input_Controller::check_boundaries, this, _1));

            timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&Input_Controller::input_loop, this));

            stop_turtle.linear.x = 0.0;
            stop_turtle.linear.y = 0.0;
            stop_turtle.linear.z = 0.0;

            stop_turtle.angular.x = 0.0;
            stop_turtle.angular.y = 0.0;
            stop_turtle.angular.z = 0.0;

            actual_distance.data =11.0;

            boundaries.data = false;
        }

    private:

        void check_distance(const std_msgs::msg::Float32::SharedPtr msg){
            actual_distance.data = msg->data;
        }

        void check_boundaries(const std_msgs::msg::Bool::SharedPtr msg){
            boundaries.data = msg->data;
        }


        void input_loop(){
            while(actual_distance.data > 1.0 && !boundaries.data){
                std::cout<< "Quale tartaruga vuoi muovere?\n1) Turtle 1\n2) Turtle 2\n:";
                std::cin >> n_turtle;
                if(n_turtle == 1 || n_turtle ==2){
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
                    }else if(n_turtle ==2){
                        t2_publisher_->publish(vel_input);
                    }

                }else{
                    std::cout<<"Devi inserire 1 o 2!\n";
                }
                t1_publisher_->publish(stop_turtle);
                t2_publisher_->publish(stop_turtle);
            }

            if(actual_distance.data < 1.0){
                std::cout<<"LE TARTARUGHE SONO TROPPO VICINE!!!"<<std::endl;
            }else if(boundaries.data){
                std::cout<<"UNA DELLE DUE TARTARUGHE SI E' AVVICINATA TROPPO AD UN BORDO!!!"<<std::endl;
            }

        }

        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_publisher_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_publisher_;

        rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr distance_sub_;
        rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr boundaries_sub_;

        geometry_msgs::msg::Twist vel_input;

        rclcpp::TimerBase::SharedPtr timer_;

        geometry_msgs::msg::Twist stop_turtle;

        std_msgs::msg::Float32 actual_distance;
        std_msgs::msg::Bool boundaries;
        int n_turtle;


};


int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Input_Controller>());
    rclcpp::shutdown();
    return 0;
}