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

            input_timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&Input_Controller::input_timer_callback, this));
            stop_timer_ = nullptr;
            
            stop_turtle.linear.x = 0.0;
            stop_turtle.linear.y = 0.0;
            stop_turtle.linear.z = 0.0;

            stop_turtle.angular.x = 0.0;
            stop_turtle.angular.y = 0.0;
            stop_turtle.angular.z = 0.0;

            moving_turtle_ = 0;

        }

    private:

        void check_distance(const std_msgs::msg::Float32::SharedPtr msg){
            actual_distance.data = msg->data;
        }

        void check_boundaries(const std_msgs::msg::Bool::SharedPtr msg){
            boundaries.data = msg->data;
        }

        void stop_timer_callback(){
            if(moving_turtle_ == 1){
                t1_publisher_->publish(stop_turtle);
                RCLCPP_INFO(this->get_logger(), "Stop for turtle1 after 1 second.");
            } else if(moving_turtle_ == 2){
                t2_publisher_->publish(stop_turtle);
                RCLCPP_INFO(this->get_logger(), "Stop for turtle2 after 1 second.");
            }
            moving_turtle_ = 0;
            stop_timer_->cancel();
        }


        void input_timer_callback(){
            std::cout<< "Quale tartaruga vuoi muovere?\n1) Turtle 1\n2) Turtle 2\n:";
            std::cin >> n_turtle;
            if(n_turtle == 1 || n_turtle ==2){
                std::cout<< "Inserisci Velocità Lineare\nx:";
                std::cin >> vel_input.linear.x;
                vel_input.linear.y=0;
                vel_input.linear.z=0;

                std::cout<< "Inserisci Velocità Angolare\nz:";
                std::cin >> vel_input.angular.z;
                vel_input.angular.x=0;
                vel_input.angular.y=0;

                moving_turtle_ = n_turtle;

                if(n_turtle == 1){
                    t1_publisher_->publish(vel_input);
                }else if(n_turtle ==2){
                    t2_publisher_->publish(vel_input);
                }

                stop_timer_ = this->create_wall_timer(
                    std::chrono::milliseconds(1000), 
                    std::bind(&Input_Controller::stop_timer_callback, this)
                );

                if(actual_distance.data < 1 || boundaries.data){   //turtle come back
                    if (vel_input.angular.z != 0 && vel_input.angular.x != 0){
                        vel_input.linear.x = vel_input.linear.x - 1;
                        vel_input.linear.z = vel_input.linear.z - 1;
                    }
                    if (vel_input.angular.z != 0){
                        vel_input.linear.x = 0;
                        vel_input.linear.z = vel_input.linear.z - 1;
                    }
                    if (vel_input.angular.x != 0){
                        vel_input.linear.x = vel_input.linear.x - 1;
                        vel_input.linear.z = 0;
                        
                    }
                    
                    if(n_turtle == 1){
                        t1_publisher_->publish(vel_input);
                    }else if(n_turtle ==2){
                        t2_publisher_->publish(vel_input);
                    }
                }

            }else{
                std::cout<<"Devi inserire 1 o 2!\n";
            }

            
        }

        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_publisher_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_publisher_;

        rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr distance_sub_;
        rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr boundaries_sub_;

        geometry_msgs::msg::Twist vel_input;

        rclcpp::TimerBase::SharedPtr input_timer_;
        rclcpp::TimerBase::SharedPtr stop_timer_;

        geometry_msgs::msg::Twist stop_turtle;

        std_msgs::msg::Float32 actual_distance;
        std_msgs::msg::Bool boundaries;

        int moving_turtle_;
        int n_turtle;

};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Input_Controller>();

    while(rclcpp::ok()){
        rclcpp::spin_some(node);
    }

    rclcpp::shutdown();
    return 0;
}