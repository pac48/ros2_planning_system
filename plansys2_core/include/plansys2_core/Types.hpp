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

#ifndef PLANSYS2_CORE__TYPES_HPP_
#define PLANSYS2_CORE__TYPES_HPP_

#include "plansys2_msgs/msg/node.hpp"
#include "plansys2_msgs/msg/param.hpp"
#include "plansys2_msgs/msg/tree.hpp"
#include "plansys2_pddl_parser/Utils.h"

#include <string>
#include <vector>

namespace plansys2
{
class Instance : public plansys2_msgs::msg::Param
{
public:
  Instance() : plansys2_msgs::msg::Param() {}
  explicit Instance(const std::string & name, const std::string & type = {})
  : plansys2_msgs::msg::Param(parser::pddl::fromStringParam(name, type))
  {
  }
  Instance(const plansys2_msgs::msg::Param & instance)  // NOLINT(runtime/explicit)
  : plansys2_msgs::msg::Param(instance)
  {
  }
};

class Predicate : public plansys2_msgs::msg::Node
{
public:
  Predicate() : plansys2_msgs::msg::Node() {}
  explicit Predicate(const std::string & pred)
  : plansys2_msgs::msg::Node(parser::pddl::fromStringPredicate(pred))
  {
  }
  Predicate(const plansys2_msgs::msg::Node & pred)  // NOLINT(runtime/explicit)
  : plansys2_msgs::msg::Node(pred)
  {
  }
  // Match both first and last names in case
  // of collisions.
  //  bool operator==(const Predicate& p) const
  //  {
  //    return parser::pddl::checkNodeEquality(*this, p);
  //  }
};
class PredicateHashFunction
{
public:
  std::hash<std::string> hasher;
  // Use sum of lengths of first and last names
  // as hash function.
  size_t operator()(const Predicate & p) const { return hasher(p.name) + p.parameters.size(); }
};

class Function : public plansys2_msgs::msg::Node
{
public:
  Function() : plansys2_msgs::msg::Node() {}
  explicit Function(const std::string & func)
  : plansys2_msgs::msg::Node(parser::pddl::fromStringFunction(func))
  {
  }
  Function(const plansys2_msgs::msg::Node & func)  // NOLINT(runtime/explicit)
  : plansys2_msgs::msg::Node(func)
  {
  }
};

class Goal : public plansys2_msgs::msg::Tree
{
public:
  Goal() : plansys2_msgs::msg::Tree() {}
  explicit Goal(const std::string & goal) : plansys2_msgs::msg::Tree(parser::pddl::fromString(goal))
  {
  }
  Goal(const plansys2_msgs::msg::Tree & goal)  // NOLINT(runtime/explicit)
  : plansys2_msgs::msg::Tree(goal)
  {
  }
};

class Or : public plansys2_msgs::msg::Tree
{
public:
  Or() : plansys2_msgs::msg::Tree() {}
  explicit Or(const std::string & or_condition)
  : plansys2_msgs::msg::Tree(parser::pddl::fromString(or_condition))
  {
  }
  Or(const plansys2_msgs::msg::Tree & or_condition)  // NOLINT(runtime/explicit)
  : plansys2_msgs::msg::Tree(or_condition)
  {
  }
};

class OneOf : public plansys2_msgs::msg::Tree
{
public:
  OneOf() : plansys2_msgs::msg::Tree() {}
  explicit OneOf(const std::string & condition)
  : plansys2_msgs::msg::Tree(parser::pddl::fromString(condition))
  {
  }
  OneOf(const plansys2_msgs::msg::Tree & condition)  // NOLINT(runtime/explicit)
  : plansys2_msgs::msg::Tree(condition)
  {
  }
};

class Unknown : public plansys2_msgs::msg::Tree
{
public:
  Unknown() : plansys2_msgs::msg::Tree() {}
  explicit Unknown(const std::string & condition)
  : plansys2_msgs::msg::Tree(parser::pddl::fromString(condition))
  {
  }
  Unknown(const plansys2_msgs::msg::Tree & condition)  // NOLINT(runtime/explicit)
  : plansys2_msgs::msg::Tree(condition)
  {
  }
};

template <class toT, class fromT>
std::vector<toT> convertVector(const std::vector<fromT> & in_vector)
{
  std::vector<toT> ret;
  for (const auto & item : in_vector) {
    ret.push_back(item);
  }
  return ret;
}

}  // namespace plansys2

#endif  // PLANSYS2_CORE__TYPES_HPP_
