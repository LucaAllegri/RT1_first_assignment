#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/bool.hpp" 
#include <math.h>
using std::placeholders::_1;

#include <iostream>
class Distance_Check: public rclcpp::Node{
    public:
        Distance_Check(): Node("distance_check"){
            t1_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
            t2_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle2/cmd_vel", 10);
            distance_pub_ = this->create_publisher<std_msgs::msg::Float32>("distance", 10);
            boundaries_pub_ = this->create_publisher<std_msgs::msg::Bool>("boundaries", 10);

            t1_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&Distance_Check::turtle1_pose, this, _1));
            t2_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&Distance_Check::turtle2_pose, this, _1));

            timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), std::bind(&Distance_Check::timer_callback, this));

            stop_turtle.linear.x = 0.0;
            stop_turtle.linear.y = 0.0;
            stop_turtle.linear.z = 0.0;

            stop_turtle.angular.x = 0.0;
            stop_turtle.angular.y = 0.0;
            stop_turtle.angular.z = 0.0;

            boundaries.data = false;
        }
    private:
        void timer_callback(){
            distance.data = sqrt(pow((pos_t2.x-pos_t1.x),2) + pow((pos_t2.y-pos_t1.y),2));
            std::cout << "Distanza:" << distance.data <<std::endl;
            if(distance.data < 1.0){
                t1_vel_pub_->publish(stop_turtle);
                t2_vel_pub_->publish(stop_turtle);
                RCLCPP_INFO(this->get_logger(), "Tartarughe troppo vicine! Arresto del nodo in corso.");
                distance_pub_->publish(distance);
                rclcpp::shutdown();
            }
            distance_pub_->publish(distance);
        }
        void turtle1_pose(const turtlesim::msg::Pose::SharedPtr msg){
            //RCLCPP_INFO(this->get_logger(), "The position of the turtle 1 (x,y): '%f', '%f", msg->x, msg->y);
            pos_t1.x = msg->x;
            pos_t1.y = msg->y;
            
            if(pos_t1.x > 10.0 || pos_t1.x < 1.0 || pos_t1.y > 10.0 || pos_t1.y < 1.0){
                t1_vel_pub_->publish(stop_turtle);
                boundaries.data = true;
                RCLCPP_INFO(this->get_logger(), "Turtle 1 troppo vicina ad un bordo! Arresto del nodo in corso.");
                boundaries_pub_->publish(boundaries);
                rclcpp::shutdown();
            }
            boundaries_pub_->publish(boundaries);
        } 
        void turtle2_pose(const turtlesim::msg::Pose::SharedPtr msg){
            //RCLCPP_INFO(this->get_logger(), "The position of the turtle 2 (x,y): '%f', '%f", msg->x, msg->y);
            pos_t2.x = msg->x;
            pos_t2.y = msg->y;

            //CHECK BOUNDARIES
            if(pos_t2.x > 10.0 || pos_t2.x < 1.0 || pos_t2.y > 10.0 || pos_t2.y < 1.0){
                t2_vel_pub_->publish(stop_turtle);
                boundaries.data = true;
                RCLCPP_INFO(this->get_logger(), "Turtle 2 troppo vicina ad un bordo! Arresto del nodo in corso.");
                boundaries_pub_->publish(boundaries);
                rclcpp::shutdown();
            }
            boundaries_pub_->publish(boundaries);
        } 
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t1_pose_sub_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t2_pose_sub_;

        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t1_vel_pub_;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr t2_vel_pub_;
        rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr distance_pub_;
        rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr boundaries_pub_;

        turtlesim::msg::Pose pos_t1;
        turtlesim::msg::Pose pos_t2;

        geometry_msgs::msg::Twist stop_turtle;

        rclcpp::TimerBase::SharedPtr timer_;

        std_msgs::msg::Float32 distance;
        std_msgs::msg::Bool boundaries;
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Distance_Check>();

    while(rclcpp::ok()){
        rclcpp::spin_some(node);
    }
    
    return 0;
}



/*CHIEDERE SE QUANDO UNA TARTARUGA SI AVVICINA TROPPO 
ALL'ALTRA LE FERMO TUTTE DUE O AGGIUNGO UN'ALTRA COMUNICAZIONE 
TRA I DUE NODI PER FERMARE LA SOLA TARTARUGA IN MOVIMENTO
*/