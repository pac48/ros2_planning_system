// Copyright 2020 Intelligent Robotics Lab
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

#include <string>
#include <memory>
#include <vector>
#include <set>
#include <algorithm>
#include <list>
#include <tuple>
#include <map>
#include <utility>
#include <plansys2_core/Utils.hpp>

#include "plansys2_executor/bt_builder_plugins/contingent_bt_builder.hpp"

#include "plansys2_problem_expert/Utils.hpp"
#include "plansys2_pddl_parser/Utils.h"

#include "rclcpp/rclcpp.hpp"

namespace plansys2 {

  ContingentBTBuilder::ContingentBTBuilder() {
    domain_client_ = std::make_shared<plansys2::DomainExpertClient>();
    problem_client_ = std::make_shared<plansys2::ProblemExpertClient>();
  }

  void
  ContingentBTBuilder::initialize(
      const std::string &bt_action_1,
      const std::string &bt_action_2,
      int precision) {
    if (!bt_action_1.empty()) {
      bt_action_ = bt_action_1;
    } else {
      bt_action_ =
          R""""(<Sequence name="ACTION_ID">
            WAIT_PREV_ACTIONS
            <ApplyAtStartEffect action="ACTION_ID"/>
            <ReactiveSequence name="ACTION_ID">
              <CheckOverAllReq action="ACTION_ID"/>
              <ExecuteAction action="ACTION_ID"/>
            </ReactiveSequence>
            <CheckAtEndReq action="ACTION_ID"/>
            <ApplyAtEndEffect action="ACTION_ID"/>
          </Sequence>
      )"""";
    }
  }

  std::string
  ContingentBTBuilder::get_tree(const plansys2_msgs::msg::Plan &current_plan) {
    auto graph = plansys2::decode_plan(current_plan);
    for (auto item: current_plan.items) {
      bt_ += item.action + "\n";
    }

    return bt_;
  }


  std::string
  ContingentBTBuilder::get_dotgraph(
      std::shared_ptr<std::map<std::string, ActionExecutionInfo>> action_map,
      bool enable_legend,
      bool enable_print_graph) {


    // create xdot graph
    std::stringstream ss;
    ss << "digraph plan {\n";

    int tab_level = 1;
    // dotgraph formatting options
    ss << t(tab_level);
    ss << "node[shape=box];\n";
    ss << t(tab_level);
    ss << "rankdir=TB;\n";

    // define all the levels and nodes
    ss << t(tab_level);
    ss << "subgraph cluster_0 {\n";

    tab_level = 2;
    ss << t(tab_level);
    ss << "label = \"Time: 0.0\";\n";
    ss << t(tab_level);
    ss << "style = rounded;\n";
    ss << t(tab_level);
    ss << "color = yellow3;\n";
    ss << t(tab_level);
    ss << "bgcolor = lemonchiffon;\n";
    ss << t(tab_level);
    ss << "labeljust = l;\n";

    tab_level = 3;
    for (auto &node: graph_->roots) {
      ss << get_node_dotgraph(node, action_map, tab_level);
    }
    tab_level = 2;

    ss << t(tab_level);
    ss << "}\n";

    int max_level = 0;
    int max_node = 0;
    for (auto &level: graph_->levels) {
      if (!level.second.empty()) {
        ss << t(tab_level);
        ss << "subgraph cluster_" << level.second.front()->level_num << " {\n";
        max_level = std::max(max_level, level.second.front()->level_num);

        tab_level = 2;
        ss << t(tab_level);
        ss << "label = \"Time: " << level.second.front()->action.time << "\";\n";
        ss << t(tab_level);
        ss << "style = rounded;\n";
        ss << t(tab_level);
        ss << "color = yellow3;\n";
        ss << t(tab_level);
        ss << "bgcolor = lemonchiffon;\n";
        ss << t(tab_level);
        ss << "labeljust = l;\n";

        tab_level = 3;
        for (auto &node: level.second) {
          max_node = std::max(max_node, node->node_num);
          ss << get_node_dotgraph(node, action_map, tab_level);
        }
        tab_level = 2;

        ss << t(tab_level);
        ss << "}\n";
      }
    }

    // define the edges
    std::set<std::string> edges;
    for (const auto &graph_root: graph_->roots) {
      get_flow_dotgraph(graph_root, edges);
    }

    tab_level = 1;
    for (const auto &edge: edges) {
      ss << t(tab_level) << edge;
    }

    if (enable_legend) {
      max_level++;
      max_node++;
      addDotGraphLegend(ss, tab_level, max_level, max_node);
    }

    ss << "}";

    return ss.str();
  }

  std::string
  ContingentBTBuilder::get_flow_tree(
      GraphNode::Ptr node,
      std::list<std::string> &used_nodes,
      int level) {
    std::string ret;
    int l = level;


    return ret;
  }


  void
  ContingentBTBuilder::get_flow_dotgraph(
      GraphNode::Ptr node,
      std::set<std::string> & edges)
  {
    for (const auto & arc : node->out_arcs) {
      std::string edge = std::to_string(node->node_num) + "->" + std::to_string(arc->node_num) +
                         ";\n";
      edges.insert(edge);
      get_flow_dotgraph(arc, edges);
    }
  }



  std::string
  ContingentBTBuilder::get_node_dotgraph(
      GraphNode::Ptr node, std::shared_ptr<std::map<std::string,
      ActionExecutionInfo>> action_map, int level) {
    std::stringstream ss;
    ss << t(level);
    ss << node->node_num << " [label=\"" << parser::pddl::nameActionsToString(node->action.action) <<
       "\"";
    ss << "labeljust=c,style=filled";

    auto status = get_action_status(node->action, action_map);
    switch (status) {
      case ActionExecutor::RUNNING:
        ss << ",color=blue,fillcolor=skyblue";
        break;
      case ActionExecutor::SUCCESS:
        ss << ",color=green4,fillcolor=seagreen2";
        break;
      case ActionExecutor::FAILURE:
        ss << ",color=red,fillcolor=pink";
        break;
      case ActionExecutor::CANCELLED:
        ss << ",color=red,fillcolor=pink";
        break;
      case ActionExecutor::IDLE:
      case ActionExecutor::DEALING:
      default:
        ss << ",color=yellow3,fillcolor=lightgoldenrod1";
        break;
    }
    ss << "];\n";
    return ss.str();
  }

  ActionExecutor::Status ContingentBTBuilder::get_action_status(
      ActionStamped action_stamped,
      std::shared_ptr<std::map<std::string, ActionExecutionInfo>> action_map) {
    auto index = "(" + parser::pddl::nameActionsToString(action_stamped.action) + "):" +
                 std::to_string(static_cast<int>(action_stamped.time * 1000));
    if ((*action_map)[index].action_executor) {
      return (*action_map)[index].action_executor->get_internal_status();
    } else {
      return ActionExecutor::IDLE;
    }
  }

  void ContingentBTBuilder::addDotGraphLegend(
      std::stringstream &ss, int tab_level, int level_counter,
      int node_counter) {
    int legend_counter = level_counter;
    int legend_node_counter = node_counter;
    ss << t(tab_level);
    ss << "subgraph cluster_" << legend_counter++ << " {\n";
    tab_level++;
    ss << t(tab_level);
    ss << "label = \"Legend\";\n";

    ss << t(tab_level);
    ss << "subgraph cluster_" << legend_counter++ << " {\n";
    tab_level++;
    ss << t(tab_level);
    ss << "label = \"Plan Timestep (sec): X.X\";\n";
    ss << t(tab_level);
    ss << "style = rounded;\n";
    ss << t(tab_level);
    ss << "color = yellow3;\n";
    ss << t(tab_level);
    ss << "bgcolor = lemonchiffon;\n";
    ss << t(tab_level);
    ss << "labeljust = l;\n";
    ss << t(tab_level);
    ss << legend_node_counter++ <<
       " [label=\n\"Finished action\n\",labeljust=c,style=filled,color=green4,fillcolor=seagreen2];\n";
    ss << t(tab_level);
    ss << legend_node_counter++ <<
       " [label=\n\"Failed action\n\",labeljust=c,style=filled,color=red,fillcolor=pink];\n";
    ss << t(tab_level);
    ss << legend_node_counter++ <<
       " [label=\n\"Current action\n\",labeljust=c,style=filled,color=blue,fillcolor=skyblue];\n";
    ss << t(tab_level);
    ss << legend_node_counter++ << " [label=\n\"Future action\n\",labeljust=c,style=filled," <<
       "color=yellow3,fillcolor=lightgoldenrod1];\n";
    tab_level--;
    ss << t(tab_level);
    ss << "}\n";

    ss << t(tab_level);
    for (int i = node_counter; i < legend_node_counter; i++) {
      if (i > node_counter) {
        ss << "->";
      }
      ss << i;
    }
    ss << " [style=invis];\n";

    tab_level--;
    ss << t(tab_level);
    ss << "}\n";
  }

  std::string
  ContingentBTBuilder::t(int level) {
    std::string ret;
    for (int i = 0; i < level; i++) {
      ret = ret + "  ";
    }
    return ret;
  }


}  // namespace plansys2
#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(plansys2::ContingentBTBuilder, plansys2::BTBuilder)