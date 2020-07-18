#include "topologicalsorter.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace testing;
using namespace std;

using namespace rectangle;
using namespace rectangle::util;

TEST(topologicalsorter, SORT)
{
    TopologicalSorter sorter;
    vector<int> result;

    {
        sorter.clear();
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::EmptyGraph);
    }

    {
        sorter.clear();
        sorter.setN(4);
        sorter.addEdge(0, 1);
        sorter.addEdge(1, 2);
        sorter.addEdge(2, 3);
        sorter.addEdge(3, 0);
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::LoopDetected);
    }

    {
        sorter.clear();
        sorter.setN(5);
        sorter.addEdge(0, 1);
        sorter.addEdge(1, 2);
        sorter.addEdge(2, 3);
        sorter.addEdge(3, 0);
        sorter.addEdge(3, 4);
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::LoopDetected);
    }

    {
        sorter.clear();
        sorter.setN(4);
        sorter.addEdge(0, 1);
        sorter.addEdge(1, 2);
        sorter.addEdge(2, 3);
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::Success);
        EXPECT_EQ(result, vector<int>({3, 2, 1, 0}));
    }

    {
        sorter.clear();
        sorter.setN(4);
        sorter.addEdge(0, 1);
        sorter.addEdge(0, 2);
        sorter.addEdge(0, 3);
        sorter.addEdge(1, 2);
        sorter.addEdge(3, 2);
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::Success);
        EXPECT_EQ(result, vector<int>({2, 1, 3, 0}));
    }

    {
        sorter.clear();
        sorter.setN(8);
        sorter.addEdge(5, 0);
        sorter.addEdge(6, 5);
        sorter.addEdge(7, 7);
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::LoopDetected);
    }

    {
        sorter.clear();
        sorter.setN(4);
        sorter.addEdge(1, 0);
        sorter.addEdge(2, 1);
        sorter.addEdge(2, 3);
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::Success);
        EXPECT_EQ(result, vector<int>({0, 3, 1, 2}));
    }

    {
        sorter.clear();
        sorter.setN(3);
        sorter.addEdge(1, 0);
        sorter.addEdge(0, 1);
        sorter.addEdge(0, 2);
        EXPECT_EQ(sorter.sort(result), TopologicalSorter::SortResult::LoopDetected);
    }
}