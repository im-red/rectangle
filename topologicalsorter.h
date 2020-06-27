#pragma once

#include <vector>
#include <utility>
#include <set>

class TopologicalSorter
{
public:
    enum class SortResult
    {
        Success,
        LoopDetected,
        EmptyGraph
    };

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

