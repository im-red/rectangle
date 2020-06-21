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

#include "structinfo.h"

#include <algorithm>

#include <assert.h>

using namespace std;

namespace builtin
{

StructInfo::StructInfo(const std::string &name, const std::map<std::string, TypeInfo::Category> &fields)
    : m_name(name)
{
    int i = 0;
    for (auto &field : fields)
    {
        m_name2field.emplace(make_pair(field.first, FieldInfo(i, field.first, field.second)));
        i += 1;
    }
}

int StructInfo::fieldIndex(const std::string &name) const
{
    auto iter = m_name2field.find(name);
    assert(iter != m_name2field.end());
    return iter->second.index;
}

int StructInfo::fieldCount() const
{
    return static_cast<int>(m_name2field.size());
}

FieldInfo StructInfo::fieldAt(int index) const
{
    assert(index >= 0 && index < static_cast<int>(m_name2field.size()));
    auto iter = find_if(m_name2field.begin(), m_name2field.end(),
                        [index](const pair<string, FieldInfo> &p)
    {
        return p.second.index == index;
    });
    assert(iter != m_name2field.end());
    return iter->second;
}

std::string StructInfo::name() const
{
    return m_name;
}

const StructInfo rectInfo = StructInfo("rect", {
                                     { "x",                 TypeInfo::Category::Int },
                                     { "y",                 TypeInfo::Category::Int },
                                     { "width",             TypeInfo::Category::Int },
                                     { "height",            TypeInfo::Category::Int },
                                     { "fill_color",        TypeInfo::Category::String },
                                     { "stroke_width",      TypeInfo::Category::Int },
                                     { "stroke_color",      TypeInfo::Category::String },
                                     { "stroke_dasharray",  TypeInfo::Category::String },
                                 });

const StructInfo textInfo = StructInfo("text", {
                                     { "x",     TypeInfo::Category::Int },
                                     { "y",     TypeInfo::Category::Int },
                                     { "size",  TypeInfo::Category::Int },
                                     { "text",  TypeInfo::Category::String },
                                 });
const StructInfo ptInfo = StructInfo("pt", {
                                   { "x",           TypeInfo::Category::Int },
                                   { "y",           TypeInfo::Category::Int },
                                   { "radius",      TypeInfo::Category::Int },
                                   { "fill_color",  TypeInfo::Category::String },
                               });

std::vector<const StructInfo *> infoList = {&rectInfo, &textInfo, &ptInfo};

}
