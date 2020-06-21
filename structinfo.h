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

#include "symbol.h"

#include <map>
#include <string>

namespace builtin
{

struct FieldInfo
{
    FieldInfo(int index_, const std::string &name_, TypeInfo::Category type_)
        : index(index_), name(name_), type(type_)
    {}
    int index;
    std::string name;
    TypeInfo::Category type;
};

class StructInfo
{
public:
    StructInfo(const std::string &name, const std::map<std::string, TypeInfo::Category> &fields);

    int fieldIndex(const std::string &name) const;
    int fieldCount() const;
    FieldInfo fieldAt(int index) const;

    std::string name() const;

private:
    std::string m_name;
    std::map<std::string, FieldInfo> m_name2field;
};

extern const StructInfo rectInfo;
extern const StructInfo textInfo;
extern const StructInfo ptInfo;

extern std::vector<const StructInfo *> infoList;

}
