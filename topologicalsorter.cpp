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

#include "topologicalsorter.h"

#include <assert.h>

using namespace std;

TopologicalSorter::TopologicalSorter(int n)
    : m_n(n)
{
    setN(n);
}

void TopologicalSorter::setN(int n)
{
    assert(n >= 0);

    clear();

    m_n = n;
    m_node2outs.resize(static_cast<size_t>(n));
    m_node2ins.resize(static_cast<size_t>(n));
}

void TopologicalSorter::clear()
{
    m_n = 0;
    m_node2outs.clear();
    m_node2ins.clear();
}

void TopologicalSorter::addEdge(int from, int to)
{
    assert(from >= 0 && from < m_n && to >= 0 && to < m_n);

    m_node2outs[static_cast<size_t>(from)].insert(to);
    m_node2ins[static_cast<size_t>(to)].insert(from);
}

TopologicalSorter::SortResult TopologicalSorter::sort(std::vector<int> &sorted)
{
    if (m_n == 0)
    {
        return SortResult::EmptyGraph;
    }

    sorted.clear();
    set<int> nodesWithoutOut;
    set<int> nextLayer;
    set<int> remain;
    std::vector<int> node2outCount(static_cast<size_t>(m_n), 0);

    for (size_t i = 0; i < m_node2outs.size(); i++)
    {
        if (m_node2outs[i].size() == 0)
        {
            nodesWithoutOut.insert(static_cast<int>(i));
        }
        else
        {
            remain.insert(static_cast<int>(i));
            node2outCount[i] = static_cast<int>(m_node2outs[i].size());
        }
    }

    if (nodesWithoutOut.size() == 0)
    {
        return SortResult::LoopDetected;
    }

    while (!remain.empty())
    {
        for (auto node : nodesWithoutOut)
        {
            sorted.push_back(node);
            for (auto in : m_node2ins[static_cast<size_t>(node)])
            {
                node2outCount[static_cast<size_t>(in)]--;
                if (node2outCount[static_cast<size_t>(in)] == 0)
                {
                    nextLayer.insert(in);
                    remain.erase(in);
                }
            }
        }
        nodesWithoutOut.clear();
        nodesWithoutOut.swap(nextLayer);
        if (!remain.empty() && nodesWithoutOut.empty())
        {
            sorted.clear();
            return SortResult::LoopDetected;
        }
    }

    for (auto node : nodesWithoutOut)
    {
        sorted.push_back(node);
    }

    return SortResult::Success;
}
