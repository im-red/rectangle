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