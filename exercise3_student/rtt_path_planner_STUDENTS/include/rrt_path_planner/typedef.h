#pragma once

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <rrt_path_planner/action/plan_path.hpp> //auto-generated

using PlanPath = rrt_path_planner::action::PlanPath;
using Server = rclcpp_action::Server<PlanPath>;
using Client = rclcpp_action::Client<PlanPath>;
