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

#pragma once

#include <map>
#include <vector>
#include <string>

#include <assert.h>

class Object;

Object operator+(const Object &lhs, const Object &rhs);
Object operator-(const Object &lhs, const Object &rhs);
Object operator*(const Object &lhs, const Object &rhs);
Object operator/(const Object &lhs, const Object &rhs);
Object operator%(const Object &lhs, const Object &rhs);
bool operator==(const Object &lhs, const Object &rhs);
bool operator!=(const Object &lhs, const Object &rhs);
bool operator>(const Object &lhs, const Object &rhs);
bool operator<(const Object &lhs, const Object &rhs);
bool operator>=(const Object &lhs, const Object &rhs);
bool operator<=(const Object &lhs, const Object &rhs);
bool operator&&(const Object &lhs, const Object &rhs);
bool operator||(const Object &lhs, const Object &rhs);

class Object
{
public:
    enum class Category
    {
        Invalid,
        Int,
        Float,
        String,
        Struct,
        List
    };

public:
    Object() {}
    explicit Object(int i) : m_category(Category::Int), m_intData(i) {}
    explicit Object(float f) : m_category(Category::Float), m_floatData(f) {}
    explicit Object(const std::string &s) : m_category(Category::String), m_stringData(s) {}
    Object(Category cat, int elementCount) : m_category(cat)
    {
        assert(cat == Category::Struct || cat == Category::List);
        m_vData.resize(static_cast<size_t>(elementCount));
    }
    ~Object();

    Object(const Object &rhs)
    {
        copy(rhs);
    }
    Object &operator=(const Object &rhs)
    {
        copy(rhs);
        return *this;
    }
    void copy(const Object &rhs)
    {
        m_category = rhs.m_category;
        m_intData = rhs.m_intData;
        m_floatData = rhs.m_floatData;
        m_stringData = rhs.m_stringData;
        m_vData = rhs.m_vData;
    }

    int intData() const;
    void setIntData(int intData);

    float floatData() const;
    void setFloatData(float floatData);

    std::string stringData() const;
    void setStringData(const std::string &stringData);

    int elementCount() const
    {
        assert(m_category == Category::Struct || m_category == Category::List);
        return static_cast<int>(m_vData.size());
    }
    Object &element(int index)
    {
        assert(m_category == Category::Struct || m_category == Category::List);
        return m_vData[static_cast<size_t>(index)];
    }
    const Object &element(int index) const
    {
        assert(m_category == Category::Struct || m_category == Category::List);
        return m_vData[static_cast<size_t>(index)];
    }

    Object &field(int index)
    {
        assert(m_category == Category::Struct);
        return m_vData[static_cast<size_t>(index)];
    }
    const Object &field(int index) const
    {
        assert(m_category == Category::Struct);
        return m_vData[static_cast<size_t>(index)];
    }

    Object &at(int index)
    {
        assert(m_category == Category::List);
        return m_vData[static_cast<size_t>(index)];
    }
    const Object &at(int index) const
    {
        assert(m_category == Category::List);
        return m_vData[static_cast<size_t>(index)];
    }

    void append(const Object &o)
    {
        assert(m_category == Category::List);
        m_vData.push_back(o);
    }

    Category category() const;
    void setCategory(const Category &category);

    std::string toString() const;

private:
    Category m_category = Category::Invalid;

    int m_intData = 0;
    float m_floatData = 0.0f;
    std::string m_stringData;
    std::vector<Object> m_vData;
};

class ObjectPointer
{
public:
    ObjectPointer(Object *o, bool tmp);
    ~ObjectPointer();

    operator Object() const;

    ObjectPointer(ObjectPointer &&rhs);
    ObjectPointer &operator=(ObjectPointer &&rhs);

    Object *get() const;
    bool isTmp() const;

private:
    ObjectPointer(const ObjectPointer &) = delete;
    ObjectPointer &operator=(const ObjectPointer &) = delete;

private:
    Object *m_object;
    bool m_tmp;
};
