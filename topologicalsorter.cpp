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
    for (size_t i = 0; i < m_node2outs.size(); i++)
    {
        if (m_node2outs[i].size() == 0)
        {
            nodesWithoutOut.insert(static_cast<int>(i));
        }
    }

    if (nodesWithoutOut.size() == 0)
    {
        return SortResult::LoopDetected;
    }

    enum class NodeColor
    {
        White,
        Gray,
        Black
    };

    vector<NodeColor> colors(static_cast<size_t>(m_n), NodeColor::White);
    vector<int> growing;
    for (auto node : nodesWithoutOut)
    {
        sorted.push_back(node);
        growing.push_back(node);
        colors[static_cast<size_t>(node)] = NodeColor::Gray;
    }

    while (!growing.empty())
    {
        int back = growing.back();
        bool everyInIsBlack = true;
        for (auto in : m_node2ins[static_cast<size_t>(back)])
        {
            if (colors[static_cast<size_t>(in)] == NodeColor::White)
            {
                sorted.push_back(in);
                growing.push_back(in);
                colors[static_cast<size_t>(in)] = NodeColor::Gray;
                everyInIsBlack = false;
                break;
            }
            else if (colors[static_cast<size_t>(in)] == NodeColor::Gray)
            {
                sorted.clear();
                return SortResult::LoopDetected;
            }
            else // Black
            {
                // do nothing
            }
        }
        if (everyInIsBlack)
        {
            colors[static_cast<size_t>(back)] = NodeColor::Black;
            growing.pop_back();
        }
    }

    for (auto c : colors)
    {
        if (c != NodeColor::Black)
        {
            sorted.clear();
            return SortResult::LoopDetected;
        }
    }
    return SortResult::Success;
}
