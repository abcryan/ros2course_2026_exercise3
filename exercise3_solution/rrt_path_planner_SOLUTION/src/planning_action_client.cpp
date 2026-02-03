#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <std_msgs/msg/bool.hpp>
#include <rrt_path_planner/typedef.h>

class PlanningClient : public rclcpp::Node
{
protected:
  using GoalHandle = rclcpp_action::ClientGoalHandle<PlanPath>;

  std::string action_name_;
  Client::SharedPtr action_client_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr status_pub_;

  void doneCb(const GoalHandle::WrappedResult &result)
  {
    RCLCPP_INFO(this->get_logger(), "Finished in state [%s]", 
                result.code == rclcpp_action::ResultCode::SUCCEEDED ? "SUCCEEDED" : "FAILED");

    std_msgs::msg::Bool status;
    status.data = (result.code == rclcpp_action::ResultCode::SUCCEEDED);
    status_pub_->publish(status);
  }

  void feedbackCb(const GoalHandle::SharedPtr goal_handle, std::shared_ptr<const PlanPath::Feedback> feedback)
  {
    RCLCPP_INFO(this->get_logger(), "Got Feedback: %.2f percent complete", feedback->percent_complete * 100);
  }

  void goalResponseCb(const GoalHandle::SharedPtr &goal_handle)
  {
    if (!goal_handle)
    {
        RCLCPP_ERROR(this->get_logger(), "Goal rejected by server.");
    }
    else
    {
        RCLCPP_INFO(this->get_logger(), "Goal accepted by server, waiting for result...");
    }
  }

public:
  PlanningClient(const std::string &action_name, const std::string &status_topic_name)
      : Node("planning_action_client"), action_name_(action_name)
  {
    action_client_ = rclcpp_action::create_client<PlanPath>(this, action_name_);
    status_pub_ = this->create_publisher<std_msgs::msg::Bool>(status_topic_name, 1);
    this->declare_parameter("start.x", rclcpp::PARAMETER_DOUBLE);
    this->declare_parameter("start.y", rclcpp::PARAMETER_DOUBLE);
    this->declare_parameter("end.x", rclcpp::PARAMETER_DOUBLE);
    this->declare_parameter("end.y", rclcpp::PARAMETER_DOUBLE);
    

    while (!action_client_->wait_for_action_server(std::chrono::seconds(5)) && rclcpp::ok())
    {
      RCLCPP_WARN(this->get_logger(), "Waiting for action server...");
    }
    RCLCPP_INFO(this->get_logger(), "Connected to action server.");
  }

  void sendGoal(const PlanPath::Goal &goal)
  {
    auto send_goal_options = rclcpp_action::Client<PlanPath>::SendGoalOptions();
    send_goal_options.result_callback = std::bind(&PlanningClient::doneCb, this, std::placeholders::_1);
    send_goal_options.goal_response_callback = std::bind(&PlanningClient::goalResponseCb, this, std::placeholders::_1);
    send_goal_options.feedback_callback = std::bind(&PlanningClient::feedbackCb, this, std::placeholders::_1, std::placeholders::_2);

    action_client_->async_send_goal(goal, send_goal_options);
  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PlanningClient>("plan_path", "planner_status");

  // Sleep for 0.5s, wait for RViz
  rclcpp::sleep_for(std::chrono::milliseconds(500));

  auto goal = PlanPath::Goal();
  auto start = geometry_msgs::msg::PoseStamped();
  auto end = geometry_msgs::msg::PoseStamped();

  start.header.frame_id = "map";
  node->get_parameter("start.x", start.pose.position.x);
  node->get_parameter("start.y", start.pose.position.y);

  end.header.frame_id = "map";
  node->get_parameter("end.x", end.pose.position.x);
  node->get_parameter("end.y", end.pose.position.y);

  goal.start = start;
  goal.end = end;

  node->sendGoal(goal);

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}