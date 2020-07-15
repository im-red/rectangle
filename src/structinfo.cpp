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

StructInfo::StructInfo(const std::string &name, const std::vector<std::pair<string, shared_ptr<TypeInfo>> > &fields)
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

static const shared_ptr<TypeInfo> intType = make_shared<TypeInfo>(TypeInfo::Category::Int);
static const shared_ptr<TypeInfo> stringType = make_shared<TypeInfo>(TypeInfo::Category::String);

const StructInfo sceneInfo = StructInfo("svg_scene", {
                                            { "leftMargin",     intType },
                                            { "topMargin",      intType },
                                            { "rightMargin",    intType },
                                            { "bottomMargin",   intType },
                                            { "width",          intType },
                                            { "height",         intType },
                                        });
const StructInfo rectInfo = StructInfo("svg_rect", {
                                           { "x",                   intType },
                                           { "y",                   intType },
                                           { "width",               intType },
                                           { "height",              intType },
                                           { "fill_color",          stringType },
                                           { "stroke_width",        intType },
                                           { "stroke_color",        stringType },
                                           { "stroke_dasharray",    stringType },
                                       });
const StructInfo textInfo = StructInfo("svg_text", {
                                           { "x",       intType },
                                           { "y",       intType },
                                           { "size",    intType },
                                           { "text",    stringType },
                                       });
const StructInfo ellipseInfo = StructInfo("svg_ellipse", {
                                              { "x",                intType },
                                              { "y",                intType },
                                              { "x_radius",         intType },
                                              { "y_radius",         intType },
                                              { "fill_color",       stringType },
                                              { "stroke_width",     intType },
                                              { "stroke_color",     stringType },
                                              { "stroke_dasharray", stringType },
                                          });
static const shared_ptr<TypeInfo> intList = make_shared<ListTypeInfo>(intType);
static const shared_ptr<TypeInfo> listOfIntList = make_shared<ListTypeInfo>(intList);
const StructInfo polygonInfo = StructInfo("svg_polygon", {
                                              { "x",                intType },
                                              { "y",                intType },
                                              { "points",           listOfIntList },
                                              { "fill_color",       stringType },
                                              { "fill_rule",        stringType },
                                              { "stroke_width",     intType },
                                              { "stroke_color",     stringType },
                                              { "stroke_dasharray", stringType },
                                          });
const StructInfo lineInfo = StructInfo("svg_line", {
                                           { "x",                   intType },
                                           { "y",                   intType },
                                           { "dx1",                 intType },
                                           { "dy1",                 intType },
                                           { "dx2",                 intType },
                                           { "dy2",                 intType },
                                           { "stroke_width",        intType },
                                           { "stroke_color",        stringType },
                                           { "stroke_dasharray",    stringType },
                                       });
const StructInfo polylineInfo = StructInfo("svg_polyline", {
                                               { "x",                intType },
                                               { "y",                intType },
                                               { "points",           listOfIntList },
                                               { "stroke_width",     intType },
                                               { "stroke_color",     stringType },
                                               { "stroke_dasharray", stringType },
                                           });

std::vector<const StructInfo *> infoList = {
    &sceneInfo,
    &rectInfo,
    &textInfo,
    &ellipseInfo,
    &polygonInfo,
    &lineInfo,
    &polylineInfo
};

}
