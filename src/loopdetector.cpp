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

#include "loopdetector.h"

#include <vector>

using namespace std;

namespace rectangle {
namespace util {

LoopDetector::LoopDetector() {}

void LoopDetector::clear() { m_node2outs.clear(); }

void LoopDetector::addEdge(int from, int to) { m_node2outs[from].insert(to); }

bool LoopDetector::detect(int &nodeInLoop) {
  enum class Color { White, Gray, Black };

  map<int, Color> node2color;
  for (auto &pair : m_node2outs) {
    node2color[pair.first] = Color::White;
  }

  map<int, set<int>> remain = m_node2outs;
  vector<int> growing;
  while (!remain.empty()) {
    if (growing.empty()) {
      int seed = remain.begin()->first;
      growing.push_back(seed);
      node2color[seed] = Color::Gray;
    }
    int cur = growing.back();
    set<int> remainOuts = remain[cur];
    if (remainOuts.size() == 0) {
      growing.pop_back();
      node2color[cur] = Color::Black;
      remain.erase(cur);
      if (!growing.empty()) {
        int parent = growing.back();
        remain[parent].erase(cur);
      }
    } else {
      int next = *remainOuts.begin();
      if (node2color[next] == Color::White) {
        growing.push_back(next);
        node2color[next] = Color::Gray;
      } else if (node2color[next] == Color::Gray) {
        nodeInLoop = next;
        return true;
      } else {
        remain[cur].erase(next);
      }
    }
  }

  return false;
}

}  // namespace util
}  // namespace rectangle
