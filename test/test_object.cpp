#include "object.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

TEST(object, CONSTRUCT)
{
    {
        Object *o = new Object(0);
        EXPECT_EQ(o->category(), Object::Category::Int);
        EXPECT_EQ(o->intData(), 0);
    }
    {
        Object *o = new Object(0.0f);
        EXPECT_EQ(o->category(), Object::Category::Float);
        EXPECT_EQ(o->floatData(), 0.0);
    }
    {
        Object *o = new Object("test");
        EXPECT_EQ(o->category(), Object::Category::String);
        EXPECT_EQ(o->stringData(), "test");
    }
    {
        Object *o = new Object(Object::Category::Struct, 5);
        EXPECT_EQ(o->category(), Object::Category::Struct);
        EXPECT_EQ(o->elementCount(), 5);
    }
    {
        Object *o = new Object(Object::Category::List, 5);
        EXPECT_EQ(o->category(), Object::Category::List);
        EXPECT_EQ(o->elementCount(), 5);
    }
}

TEST(object, STRUCT)
{
    Object *o = new Object(Object::Category::Struct, 1);
    o->field(0)->setCategory(Object::Category::Int);
    o->field(0)->setIntData(0);
    EXPECT_EQ(o->field(0)->category(), Object::Category::Int);
    EXPECT_EQ(o->field(0)->intData(), 0);
}

TEST(object, LIST)
{
    Object *o = new Object(Object::Category::List, 0);
    o->append(new Object(0));
    EXPECT_EQ(o->elementCount(), 1);
    EXPECT_EQ(o->at(0)->category(), Object::Category::Int);
    EXPECT_EQ(o->at(0)->intData(), 0);
}

TEST(object, ASSIGN)
{
    Object *o = new Object(Object::Category::List, 0);
    o->append(new Object(0));
    EXPECT_EQ(o->elementCount(), 1);
    EXPECT_EQ(o->at(0)->category(), Object::Category::Int);
    EXPECT_EQ(o->at(0)->intData(), 0);

    Object *copy = new Object(*o);
    EXPECT_TRUE(*o == *copy);
}
