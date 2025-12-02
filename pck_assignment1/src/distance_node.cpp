 #include "rclcpp/rclcpp.hpp"
 #include "std_msgs/msg/string.hpp"
 #include "turtlesim/msg/pose.hpp"
 using std::placeholders::_1;

class DistanceController: public rclcpp::Node{
    public:
        DistanceController(): Node("distance_controller"){
            t1_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle1/pose", 10, std::bind(&DistanceController::turtle1_pose_callback, this, _1));
            t2_pose_sub_ = this->create_subscription<turtlesim::msg::Pose>("/turtle2/pose", 10, std::bind(&DistanceController::turtle2_pose_callback, this, _1));
        }
    private:
        void topic_callback(const std_msgs::msg::String::SharedPtr msg) const{
            RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
        } 

        void turtle1_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t1_pose.x = msg->x;
            t1_pose.y = msg->y;
            RCLCPP_INFO(this->get_logger(), "Pose Turtle1: '%f' '%f'", t1_pose.x, t1_pose.y);
        }

        void turtle2_pose_callback(const turtlesim::msg::Pose::SharedPtr msg){
            t2_pose.x = msg->x;
            t2_pose.y = msg->y;
            RCLCPP_INFO(this->get_logger(), "Pose Turtle2: '%f' '%f'", t2_pose.x, t2_pose.y);
        }
        //TIMERS


        //PUBLISHER


        //SUBSCRIBERS
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t1_pose_sub_;
        rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr t2_pose_sub_;

        //VARIABLES
        turtlesim::msg::Pose t1_pose;
        turtlesim::msg::Pose t2_pose;
        
};

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DistanceController>());
    rclcpp::shutdown();
    return 0;
}