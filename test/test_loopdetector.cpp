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

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace testing;
using namespace std;

using namespace rectangle;
using namespace rectangle::util;

TEST(loopdetector, DETECT)
{
    LoopDetector detector;
    int node;

    {
        detector.clear();
        EXPECT_EQ(detector.detect(node), false);
    }

    {
        detector.clear();
        detector.addEdge(0, 1);
        detector.addEdge(1, 2);
        detector.addEdge(2, 3);
        detector.addEdge(3, 0);
        EXPECT_EQ(detector.detect(node), true);
    }

    {
        detector.clear();
        detector.addEdge(0, 1);
        detector.addEdge(1, 2);
        detector.addEdge(2, 3);
        detector.addEdge(3, 0);
        detector.addEdge(3, 4);
        EXPECT_EQ(detector.detect(node), true);
    }

    {
        detector.clear();
        detector.addEdge(0, 1);
        detector.addEdge(1, 2);
        detector.addEdge(2, 3);
        EXPECT_EQ(detector.detect(node), false);
    }

    {
        detector.clear();
        detector.addEdge(0, 1);
        detector.addEdge(0, 2);
        detector.addEdge(0, 3);
        detector.addEdge(1, 2);
        detector.addEdge(3, 2);
        EXPECT_EQ(detector.detect(node), false);
    }

    {
        detector.clear();
        detector.addEdge(5, 0);
        detector.addEdge(6, 5);
        detector.addEdge(7, 7);
        EXPECT_EQ(detector.detect(node), true);
    }

    {
        detector.clear();
        detector.addEdge(1, 0);
        detector.addEdge(2, 1);
        detector.addEdge(2, 3);
        EXPECT_EQ(detector.detect(node), false);
    }

    {
        detector.clear();
        detector.addEdge(1, 0);
        detector.addEdge(0, 1);
        detector.addEdge(0, 2);
        EXPECT_EQ(detector.detect(node), true);
    }

    {
        detector.clear();
        detector.addEdge(0, 1);
        detector.addEdge(1, 2);
        detector.addEdge(2, 3);
        detector.addEdge(2, 0);
        detector.addEdge(3, 4);
        detector.addEdge(3, 1);
        EXPECT_EQ(detector.detect(node), true);
    }
}