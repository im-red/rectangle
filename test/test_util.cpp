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

#include "util.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace testing;
using namespace std;

using namespace rectangle;
using namespace rectangle::util;

TEST(util, SPLIT_INTO_LINES)
{
    {
        string s = "aa\r\nbb\r\n\r\n";
        vector<string> result = { "aa", "bb", "", "" };
        EXPECT_EQ(splitIntoLines(s), result);
    }
    {
        string s = "aa\rbb\r\r";
        vector<string> result = { "aa", "bb", "", "" };
        EXPECT_EQ(splitIntoLines(s), result);
    }
    {
        string s = "aa\nbb\n\n";
        vector<string> result = { "aa", "bb", "", "" };
        EXPECT_EQ(splitIntoLines(s), result);
    }
    {
        string s = "aa\r\nbb";
        vector<string> result = { "aa", "bb" };
        EXPECT_EQ(splitIntoLines(s), result);
    }
    {
        string s = "aa\rbb";
        vector<string> result = { "aa", "bb" };
        EXPECT_EQ(splitIntoLines(s), result);
    }
    {
        string s = "aa\nbb";
        vector<string> result = { "aa", "bb" };
        EXPECT_EQ(splitIntoLines(s), result);
    }
}