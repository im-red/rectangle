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

#include "object.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

using namespace rectangle;
using namespace rectangle::runtime;

TEST(object, CONSTRUCT)
{
    {
        Object o(0);
        EXPECT_EQ(o.category(), Object::Category::Int);
        EXPECT_EQ(o.intData(), 0);
    }
    {
        Object o(0.0f);
        EXPECT_EQ(o.category(), Object::Category::Float);
        EXPECT_EQ(o.floatData(), 0.0);
    }
    {
        Object o("test");
        EXPECT_EQ(o.category(), Object::Category::String);
        EXPECT_EQ(o.stringData(), "test");
    }
    {
        Object o(Object::Category::Struct, 5);
        EXPECT_EQ(o.category(), Object::Category::Struct);
        EXPECT_EQ(o.elementCount(), 5);
    }
    {
        Object o(Object::Category::List, 5);
        EXPECT_EQ(o.category(), Object::Category::List);
        EXPECT_EQ(o.elementCount(), 5);
    }
}

TEST(object, STRUCT)
{
    Object o(Object::Category::Struct, 1);
    o.field(0).setCategory(Object::Category::Int);
    o.field(0).setIntData(0);
    EXPECT_EQ(o.field(0).category(), Object::Category::Int);
    EXPECT_EQ(o.field(0).intData(), 0);
}

TEST(object, LIST)
{
    Object o(Object::Category::List, 0);
    o.append(Object(0));
    EXPECT_EQ(o.elementCount(), 1);
    EXPECT_EQ(o.at(0).category(), Object::Category::Int);
    EXPECT_EQ(o.at(0).intData(), 0);
}

TEST(object, ASSIGN)
{
    Object o(Object::Category::List, 0);
    o.append(Object(0));
    EXPECT_EQ(o.elementCount(), 1);
    EXPECT_EQ(o.at(0).category(), Object::Category::Int);
    EXPECT_EQ(o.at(0).intData(), 0);

    Object copy = o;
    EXPECT_TRUE(o == copy);
}
