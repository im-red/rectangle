/*********************************************************************************
 * Copyright (C) 2020  Jia Lihong
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ********************************************************************************/

#pragma once

#include <set>
#include <utility>
#include <vector>

namespace rectangle {
namespace util {

class TopologicalSorter {
 public:
  enum class SortResult { Success, LoopDetected, EmptyGraph };

 public:
  explicit TopologicalSorter(int n = 0);

  void setN(int n);
  void clear();
  void addEdge(int from, int to);

  SortResult sort(std::vector<int> &sorted);

 private:
  int m_n = -1;
  std::vector<std::set<int>> m_node2outs;
  std::vector<std::set<int>> m_node2ins;
};

}  // namespace util
}  // namespace rectangle
