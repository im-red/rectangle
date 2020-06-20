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
    Object(int i) : m_category(Category::Int), m_intData(i) {}
    Object(float f) : m_category(Category::Float), m_floatData(f) {}
    Object(const std::string &s) : m_category(Category::String), m_stringData(s) {}
    Object(Category cat, int elementCount) : m_category(cat)
    {
        assert(cat == Category::Struct || cat == Category::List);
        m_vData.reserve(static_cast<size_t>(elementCount));
        for (int i = 0; i < elementCount; i++)
        {
            m_vData.push_back(new Object());
        }
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
        for (auto &o : m_vData)
        {
            delete o;
        }
        m_vData.clear();
        for (auto &o : rhs.m_vData)
        {
            m_vData.push_back(new Object(*o));
        }
    }

    bool operator==(const Object &rhs)
    {
        if (m_category != rhs.m_category)
        {
            return false;
        }
        switch (m_category)
        {
        case Category::Int:     return m_intData == rhs.m_intData;
        case Category::Float:   return m_floatData == rhs.m_floatData;
        case Category::String:  return m_stringData == rhs.m_stringData;
        case Category::Struct:
        case Category::List:
        {
            if (m_vData.size() != rhs.m_vData.size())
            {
                return false;
            }
            size_t len = m_vData.size();
            for (size_t i = 0; i < len; i++)
            {
                if (*m_vData[i] != *rhs.m_vData[i])
                {
                    return false;
                }
            }
            return true;
        }
        default:
        {
            return true;
        }
        }
    }
    bool operator!=(const Object &rhs)
    {
        return !operator==(rhs);
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

    Object *field(int index) const
    {
        assert(m_category == Category::Struct);
        return m_vData[static_cast<size_t>(index)];
    }

    Object *at(int index) const
    {
        assert(m_category == Category::List);
        return m_vData[static_cast<size_t>(index)];
    }
    void append(Object *o)
    {
        assert(m_category == Category::List);
        m_vData.push_back(o);
    }

    Category category() const;
    void setCategory(const Category &category);

    std::string toString() const;

private:
    Category m_category = Category::Invalid;

    int m_intData;
    float m_floatData;
    std::string m_stringData;
    std::vector<Object *> m_vData;
};
