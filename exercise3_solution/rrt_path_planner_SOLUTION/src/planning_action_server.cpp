#include <memory>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <rrt_path_planner/rrt_planner.h>
#include <geometry_msgs/msg/point_stamped.hpp>
#include <rrt_path_planner/typedef.h>
class PlanPathAction : public rclcpp::Node
{
protected:
  Server::SharedPtr action_server_;
  std::string action_name_;
  PlanPath::Feedback feedback_;
  PlanPath::Result result_;
  rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr start_pub_;
  rclcpp::Publisher<geometry_msgs::msg::PointStamped>::SharedPtr end_pub_;

  void ClearPreviousResult()
  {
    feedback_ = PlanPath::Feedback();
    result_ = PlanPath::Result();
  }

public:
  using GoalHandle = rclcpp_action::ServerGoalHandle<PlanPath>;
  PlanPathAction() : Node("planning_action_server")
  {
    action_server_ = rclcpp_action::create_server<PlanPath>(
        this,
        "plan_path",
        std::bind(&PlanPathAction::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&PlanPathAction::handle_cancel, this, std::placeholders::_1),
        std::bind(&PlanPathAction::executeCB, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Planning Action Server started.");

    start_pub_ = this->create_publisher<geometry_msgs::msg::PointStamped>(
        "start", rclcpp::QoS(rclcpp::KeepLast(1)).transient_local()
    );

    end_pub_ = this->create_publisher<geometry_msgs::msg::PointStamped>(
        "end", rclcpp::QoS(rclcpp::KeepLast(1)).transient_local()
    );
  }

  rclcpp_action::GoalResponse handle_goal(
      const rclcpp_action::GoalUUID &uuid,
      std::shared_ptr<const PlanPath::Goal> goal)
  {
    (void)uuid;
    // publish the goal to another topic
    geometry_msgs::msg::PointStamped start;
    start.header.frame_id = "map";
    start.point = goal->start.pose.position;

    geometry_msgs::msg::PointStamped end;
    end.header.frame_id = "map";
    end.point = goal->end.pose.position;

    start_pub_->publish(start);
    end_pub_->publish(end);

    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  rclcpp_action::CancelResponse handle_cancel(
      const std::shared_ptr<GoalHandle> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "Received a cancel request.");
    (void)goal_handle;
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  geometry_msgs::msg::PoseStamped PointToPoseStamped(Point point)
  {
    geometry_msgs::msg::PoseStamped poseStamped;
    poseStamped.header.frame_id = "map";
    poseStamped.pose.position.x = point.x;
    poseStamped.pose.position.y = point.y;
    poseStamped.pose.position.z = 0.0;
    poseStamped.pose.orientation.x = 0.0;
    poseStamped.pose.orientation.y = 0.0;
    poseStamped.pose.orientation.z = 0.0;
    poseStamped.pose.orientation.w = 1.0;
    return poseStamped;
  }

  void executeCB(const std::shared_ptr<GoalHandle> goal_handle)
  {
    ClearPreviousResult();
    rclcpp::Rate r(4);
    bool no_success = true;

    auto goal = goal_handle->get_goal();
    Point start(goal->start.pose.position.x, goal->start.pose.position.y);
    Point end(goal->end.pose.position.x, goal->end.pose.position.y);
    RRTPlanner planner(start, end);

    std::vector<Point> path = planner.plan();

    if (path.empty())
    {
      RCLCPP_WARN(this->get_logger(), "NO PATH TO FOLLOW");
      goal_handle->abort(std::make_shared<PlanPath::Result>(result_));
      return;
    }

    for (size_t i = 0; i < path.size(); i++)
    {
      if (goal_handle->is_canceling())
      {
        goal_handle->canceled(std::make_shared<PlanPath::Result>(result_));
        RCLCPP_INFO(this->get_logger(), "Goal canceled.");
        return;
      }

      Point point = path[i];
      feedback_.current_path.header.frame_id = "map";
      feedback_.current_path.poses.push_back(PointToPoseStamped(point));
      feedback_.percent_complete = static_cast<double>(i + 1) / path.size();
      goal_handle->publish_feedback(std::make_shared<PlanPath::Feedback>(feedback_));

      r.sleep();
    }

    result_.path = feedback_.current_path;
    goal_handle->succeed(std::make_shared<PlanPath::Result>(result_));
    RCLCPP_INFO(this->get_logger(), "Path planning completed successfully.");
  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<PlanPathAction>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}