#include "bcr_bot/astar/astar_planner.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <mutex>
#include "nav2_core/planner_exceptions.hpp"
#include "nav2_costmap_2d/cost_values.hpp"
#include "nav2_util/node_utils.hpp"
#include "pluginlib/class_list_macros.hpp"
namespace bcr_bot {
void AStarPlanner::configure(const rclcpp_lifecycle::LifecycleNode::WeakPtr & parent, std::string name,
  std::shared_ptr<tf2_ros::Buffer> tf, std::shared_ptr<nav2_costmap_2d::Costmap2DROS> cm)
{
  node_ = parent.lock(); if (!node_) throw nav2_core::PlannerException("AStarPlanner lifecycle node unavailable");
  name_ = std::move(name); tf_ = std::move(tf); costmap_ros_ = std::move(cm); costmap_ = costmap_ros_->getCostmap(); global_frame_ = costmap_ros_->getGlobalFrameID();
  nav2_util::declare_parameter_if_not_declared(node_, name_ + ".allow_unknown", rclcpp::ParameterValue(true));
  nav2_util::declare_parameter_if_not_declared(node_, name_ + ".use_diagonal", rclcpp::ParameterValue(true));
  nav2_util::declare_parameter_if_not_declared(node_, name_ + ".prevent_corner_cutting", rclcpp::ParameterValue(true));
  nav2_util::declare_parameter_if_not_declared(node_, name_ + ".cost_penalty", rclcpp::ParameterValue(2.0));
  nav2_util::declare_parameter_if_not_declared(node_, name_ + ".max_planning_time", rclcpp::ParameterValue(2.0));
  node_->get_parameter(name_ + ".allow_unknown", options_.allow_unknown); node_->get_parameter(name_ + ".use_diagonal", options_.use_diagonal);
  node_->get_parameter(name_ + ".prevent_corner_cutting", options_.prevent_corner_cutting); node_->get_parameter(name_ + ".cost_penalty", options_.cost_penalty); node_->get_parameter(name_ + ".max_planning_time", max_planning_time_);
  options_.cost_penalty = std::max(0.0, options_.cost_penalty); max_planning_time_ = std::max(0.0, max_planning_time_); options_.lethal_cost = nav2_costmap_2d::INSCRIBED_INFLATED_OBSTACLE;
  RCLCPP_INFO(node_->get_logger(), "Configured %s as cost-aware A* planner", name_.c_str());
}
void AStarPlanner::cleanup() { costmap_ = nullptr; costmap_ros_.reset(); tf_.reset(); node_.reset(); }
void AStarPlanner::activate() { RCLCPP_INFO(node_->get_logger(), "Activating %s", name_.c_str()); }
void AStarPlanner::deactivate() { RCLCPP_INFO(node_->get_logger(), "Deactivating %s", name_.c_str()); }
nav_msgs::msg::Path AStarPlanner::createPlan(const geometry_msgs::msg::PoseStamped & start, const geometry_msgs::msg::PoseStamped & goal, std::function<bool()> cancel)
{
  if (!costmap_ || !node_) throw nav2_core::PlannerException("AStarPlanner is not configured");
  if (start.header.frame_id != global_frame_ || goal.header.frame_id != global_frame_) throw nav2_core::PlannerTFError("Start and goal must use " + global_frame_);
  unsigned int sx, sy, gx, gy; astar::Grid grid; double ox, oy, res;
  { std::unique_lock<nav2_costmap_2d::Costmap2D::mutex_t> lock(*costmap_->getMutex());
    if (!costmap_->worldToMap(start.pose.position.x, start.pose.position.y, sx, sy)) throw nav2_core::StartOutsideMapBounds("Start is outside costmap");
    if (!costmap_->worldToMap(goal.pose.position.x, goal.pose.position.y, gx, gy)) throw nav2_core::GoalOutsideMapBounds("Goal is outside costmap");
    grid.width = costmap_->getSizeInCellsX(); grid.height = costmap_->getSizeInCellsY(); ox = costmap_->getOriginX(); oy = costmap_->getOriginY(); res = costmap_->getResolution();
    const auto * data = costmap_->getCharMap(); grid.costs.assign(data, data + grid.width * grid.height);
  }
  const auto begin = std::chrono::steady_clock::now(); bool timeout = false;
  const auto check = [&]() { if (cancel && cancel()) return true; if (max_planning_time_ > 0.0 && std::chrono::duration<double>(std::chrono::steady_clock::now() - begin).count() > max_planning_time_) { timeout = true; return true; } return false; };
  const auto result = astar::AStar().search(grid, sx, sy, gx, gy, options_, check);
  if (result.status == astar::SearchStatus::kStartOccupied) throw nav2_core::StartOccupied("Start cell is occupied");
  if (result.status == astar::SearchStatus::kGoalOccupied) throw nav2_core::GoalOccupied("Goal cell is occupied");
  if (result.status == astar::SearchStatus::kCancelled) { if (timeout) throw nav2_core::PlannerTimedOut("A* exceeded max_planning_time"); throw nav2_core::PlannerCancelled("A* was cancelled"); }
  if (result.status != astar::SearchStatus::kSuccess) throw nav2_core::NoValidPathCouldBeFound("A* could not find a path");
  nav_msgs::msg::Path path; path.header.frame_id = global_frame_; path.header.stamp = node_->now(); path.poses.reserve(result.path.size());
  for (const auto idx : result.path) { geometry_msgs::msg::PoseStamped p; p.header = path.header; const auto x = idx % grid.width, y = idx / grid.width; p.pose.position.x = ox + (static_cast<double>(x) + .5) * res; p.pose.position.y = oy + (static_cast<double>(y) + .5) * res; p.pose.orientation.w = 1.0; path.poses.push_back(p); }
  if (path.poses.size() == 1U) { path.poses.front() = start; path.poses.front().header = path.header; if (std::hypot(goal.pose.position.x - start.pose.position.x, goal.pose.position.y - start.pose.position.y) > 1e-6) { path.poses.push_back(goal); path.poses.back().header = path.header; } else path.poses.front().pose.orientation = goal.pose.orientation; }
  else { path.poses.front() = start; path.poses.front().header = path.header; path.poses.back() = goal; path.poses.back().header = path.header; }
  for (std::size_t i = 0; i + 1 < path.poses.size(); ++i) { const double yaw = std::atan2(path.poses[i+1].pose.position.y - path.poses[i].pose.position.y, path.poses[i+1].pose.position.x - path.poses[i].pose.position.x); path.poses[i].pose.orientation.z = std::sin(yaw/2.0); path.poses[i].pose.orientation.w = std::cos(yaw/2.0); }
  RCLCPP_INFO(node_->get_logger(), "AStarPlanner generated path with %zu grid cells", result.path.size());
  return path;
}
}  // namespace bcr_bot
PLUGINLIB_EXPORT_CLASS(bcr_bot::AStarPlanner, nav2_core::GlobalPlanner)
