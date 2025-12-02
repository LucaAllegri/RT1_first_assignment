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
        main_timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&InputController::stop_timer_callback, this));

        //PUBLISHERS
        t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        t2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
        id_turtle_managed_pub_ = this->create_publisher<std_msgs::msg::Int32>("/id_turtle_moved", 10);
        
        //SUBSCRIBERS
        stop_msg_sub_ = this->create_subscription<std_msgs::msg::Bool>("/stop_message", 10, std::bind(&InputController::stop_msg_callback, this, _1));
        reverse_state_sub_ = this->create_subscription<std_msgs::msg::Int32>("/is_reversing", 10, std::bind(&InputController::stop_msg_callback, this, _1));

        //VARIABLES
        stop_turtle.linear.x = 0.0;
        stop_turtle.angular.z = 0.0;
        vel_input.linear.x = 0.0;
        vel_input.angular.z = 0.0;
        last_vel.linear.x = 0.0;
        last_vel.angular.z = 0.0;
        is_stopped.data = false;
    }
    
    private:

        void stop_timer_callback(){
            if(is_stopped.data){
                /*if(last_vel.liner.x == 0.0 && last_vel.angular.z == 0.0){  //if both turtles start near each other or near one bounder
                    last_vel.linear.x = -1.0; 
                    t1_vel_pub_->publish(last_vel);
                }*/
                if(last_vel.linear.x < 0.0){
                    if(moved_turtle.data == 1){
                        last_vel.linear.x = 1.0;
                        t1_vel_pub_->publish(last_vel);
                    }else if(moved_turtle.data == 2){
                        last_vel.linear.x = 1.0;
                        t2_vel_pub_->publish(last_vel);
                    }
                }else if(last_vel.linear.x > 0.0){
                    if(moved_turtle.data == 1){
                        last_vel.linear.x = -1.0;
                        t1_vel_pub_->publish(last_vel);
                    }else if(moved_turtle.data == 2){
                        last_vel.linear.x = -1.0;
                        t2_vel_pub_->publish(last_vel);
                    }
                }
            }
        }

        void stop_msg_callback(const std_msgs::msg::Bool::SharedPtr msg){
            is_stopped.data = msg->data;
        }

        void input_timer_callback(){
        
            std::cout<< "Quale tartaruga vuoi muovere?\n1) Turtle 1\n2) Turtle 2\n:";
            std::cin >> n_turtle;
            if(n_turtle == 1 || n_turtle ==2){
                std::cout<< "Inserisci Velocità Lineare\nx:";
                std::cin >> vel_input.linear.x;

                std::cout<< "Inserisci Velocità Angolare\nz:";
                std::cin >> vel_input.angular.z;

                moved_turtle.data = n_turtle;
                last_vel = vel_input;

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

        //TIMERSs
        rclcpp::TimerBase::SharedPtr input_timer_;
        rclcpp::TimerBase::SharedPtr stop_timer_;

        //PUBLISHERS
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;
        rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr id_turtle_managed_pub_;

        //SUBSCRIBERS
        rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr stop_msg_sub_;

        //VARIABLES
        int n_turtle;
        bool is_in_recovery_state;
        geometry_msgs::msg::Twist vel_input;
        geometry_msgs::msg::Twist last_vel;
        geometry_msgs::msg::Twist stop_turtle;
        std_msgs::msg::Bool is_stopped;
        std_msgs::msg::Int32 moved_turtle;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<InputController>());
    rclcpp::shutdown();
    return 0;
}