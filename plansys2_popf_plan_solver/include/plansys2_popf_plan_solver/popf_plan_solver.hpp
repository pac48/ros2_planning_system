// Copyright 2019 Intelligent Robotics Lab
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PLANSYS2_POPF_PLAN_SOLVER__POPF_PLAN_SOLVER_HPP_
#define PLANSYS2_POPF_PLAN_SOLVER__POPF_PLAN_SOLVER_HPP_

#include "plansys2_core/PlanSolverBase.hpp"

#include <memory>
#include <optional>
#include <string>

namespace plansys2
{
class POPFPlanSolver : public PlanSolverBase
{
private:
  std::string parameter_name_;
  rclcpp_lifecycle::LifecycleNode::SharedPtr lc_node_;

public:
  POPFPlanSolver();

  void configure(rclcpp_lifecycle::LifecycleNode::SharedPtr &, const std::string &);

  std::optional<plansys2_msgs::msg::Plan> getPlan(
    const std::string & domain, const std::string & problem,
    const std::string & node_namespace = "");

  bool is_valid_domain(const std::string & domain, const std::string & node_namespace = "");
};

}  // namespace plansys2

#endif  // PLANSYS2_POPF_PLAN_SOLVER__POPF_PLAN_SOLVER_HPP_
