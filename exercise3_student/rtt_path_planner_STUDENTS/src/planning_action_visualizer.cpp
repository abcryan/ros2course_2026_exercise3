#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <nav_msgs/msg/path.hpp>
#include <rrt_path_planner/typedef.h>

class PathVisualizer : public rclcpp::Node
{
protected:
  rclcpp::Subscription<PlanPath::Impl::FeedbackMessage>::SharedPtr path_sub_;

  rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_viz_pub_;

  // Visualize the current path
  void visualizationCB(const PlanPath::Impl::FeedbackMessage::ConstSharedPtr msg)
  {
    path_viz_pub_->publish(msg->feedback.current_path);
  }

public:
  PathVisualizer(const std::string &viz_topic_name, const std::string &action_name)
      : Node("planning_action_visualizer")
  {
    path_sub_ = this->create_subscription<PlanPath::Impl::FeedbackMessage>(
        action_name + "/_action/feedback", 10,
        std::bind(&PathVisualizer::visualizationCB, this, std::placeholders::_1));

    path_viz_pub_ = this->create_publisher<nav_msgs::msg::Path>(
        viz_topic_name, rclcpp::QoS(rclcpp::KeepLast(1)).transient_local()
    );

  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PathVisualizer>("path_viz", "plan_path");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}