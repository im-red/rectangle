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

Object::~Object()
{
    for (auto &o : m_vData)
    {
        delete o;
    }
}

int Object::intData() const
{
    assert(m_category == Category::Int);
    return m_intData;
}

void Object::setIntData(int intData)
{
    assert(m_category == Category::Int);
    m_intData = intData;
}

float Object::floatData() const
{
    assert(m_category == Category::Float);
    return m_floatData;
}

void Object::setFloatData(float floatData)
{
    assert(m_category == Category::Float);
    m_floatData = floatData;
}

std::string Object::stringData() const
{
    assert(m_category == Category::String);
    return m_stringData;
}

void Object::setStringData(const std::string &stringData)
{
    assert(m_category == Category::String);
    m_stringData = stringData;
}

Object::Category Object::category() const
{
    return m_category;
}

void Object::setCategory(const Category &category)
{
    m_category = category;
}
