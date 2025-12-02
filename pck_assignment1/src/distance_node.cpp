 #include "rclcpp/rclcpp.hpp"
 #include "std_msgs/msg/string.hpp"
 #include "turtlesim/msg/pose.hpp"
 #include "std_msgs/msg/bool.hpp"
 #include "std_msgs/msg/float32.hpp"
 #include <math.h>
 using std::placeholders::_1;

class DistanceController: public rclcpp::Node{
    public:
        DistanceController(): Node("distance_controller"){
            stop_pub_ = this->create_publisher<std_msgs::msg::Bool>("/stop_message", 10);
            distance_pub_ = this->create_publisher<std_msgs::msg::Float32>("/distance", 10);

            t1_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&DistanceController::turtle1_pose_callback, this, _1));
            t2_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&DistanceController::turtle2_pose_callback, this, _1));
        
            distance_timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&InputController::distance_timer_callback, this));
            boundaries_timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&InputController::boundaries_timer_callback, this));
        }
    private:

        void distance_timer_callback(){
            stop.data = false;
            
            distance.data = sqrt(pow((t2_pose.x-t1_pose.x),2) + pow((t2_pose.y-t1_pose.y),2));
            std::cout << "Distanza:" << distance.data <<std::endl;
            distance_pub_->publish(distance.data);

            if(distance.data < 1.0){
                stop.data=true;
            }
            stop_pub_->pusblish(stop.data);
        }

        void boundaries_timer_callback(){
            stop = false;
            if(t1_pose.x > 10.0 || t1_pose.y > 10.0 || t1_pose.x < 1.0 || t1_pose.y < 1.0){
                stop=true;
            }
            if(t2_pose.x > 10.0 || t2_pose.y > 10.0 || t2_pose.x < 1.0 || t2_pose.y < 1.0){
                stop=true;
            }
            stop_pub_->pusblish(stop.data);
        }

        void turtle1_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t1_pose.x = msg->x;
            t1_pose.y = msg->y;
        }

        void turtle2_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t2_pose.x = msg->x;
            t2_pose.y = msg->y;
        }
        //TIMERS
        rclcpp::TimerBase::SharedPtr distance_timer_;
        rclcpp::TimerBase::SharedPtr boundaries_timer_;

        //PUBLISHER
        rclcpp::Publisher<std_msgs::msg::bool>::SharedPtr stop_pub_;

        //SUBSCRIBERS
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t1_pose_sub_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t2_pose_sub_;

        //VARIABLES
        turtlesim::msg::Pose t1_pose;
        turtlesim::msg::Pose t2_pose;
        std_msgs::msg::Bool stop;
        std_msgs::msg::Float32 distance;

        
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistanceController>());
    rclcpp::shutdown();
    return 0;
}