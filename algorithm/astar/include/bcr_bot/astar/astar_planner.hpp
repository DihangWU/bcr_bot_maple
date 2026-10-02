#ifndef BCR_BOT__ASTAR__ASTAR_PLANNER_HPP_
#define BCR_BOT__ASTAR__ASTAR_PLANNER_HPP_
#include <memory>
#include <string>
#include "bcr_bot/astar/astar.hpp"
#include "nav2_core/global_planner.hpp"
#include "nav2_costmap_2d/costmap_2d_ros.hpp"
#include "tf2_ros/buffer.h"
namespace bcr_bot {
class AStarPlanner : public nav2_core::GlobalPlanner {
public:
  void configure(const rclcpp_lifecycle::LifecycleNode::WeakPtr &, std::string,
    std::shared_ptr<tf2_ros::Buffer>, std::shared_ptr<nav2_costmap_2d::Costmap2DROS>) override;
  void cleanup() override; void activate() override; void deactivate() override;
  nav_msgs::msg::Path createPlan(const geometry_msgs::msg::PoseStamped &, const geometry_msgs::msg::PoseStamped &, std::function<bool()>) override;
private:
  rclcpp_lifecycle::LifecycleNode::SharedPtr node_; std::shared_ptr<tf2_ros::Buffer> tf_;
  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros_; nav2_costmap_2d::Costmap2D * costmap_{nullptr};
  std::string name_; std::string global_frame_; astar::Options options_; double max_planning_time_{2.0};
};
}  // namespace bcr_bot
#endif
