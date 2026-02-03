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

  void doneCb(const GoalHandle::WrappedResult &result)
  {
    // TASK 3 TODO: print the state (treat anything other than SUCCEEDED as FAILED)

    // TASK 3 TODO: publish the state
  }

  void feedbackCb(const GoalHandle::SharedPtr goal_handle, std::shared_ptr<const PlanPath::Feedback> feedback)
  {
    // TASK 3 TODO: print feedback percentage
  }

  void goalResponseCb(const GoalHandle::SharedPtr &goal_handle)
  {
    // TASK 3 TODO: print goal messages
  }

public:
  PlanningClient(const std::string &action_name, const std::string &status_topic_name)
      : Node("planning_action_client"), action_name_(action_name)
  {
    action_client_ = rclcpp_action::create_client<PlanPath>(this, action_name_);
    
    // TASK 1 TODO: declare parameter for start and goal

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
  // Sleep for 0.5s, wait for RViz
  rclcpp::sleep_for(std::chrono::milliseconds(500));

  auto node = std::make_shared<PlanningClient>("plan_path", "planner_status");

  // TASK 1 TODO: form the action goal

  // TASK 1 TODO: get parameter for start and end

  // TASK 1 TODO: send goal

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}