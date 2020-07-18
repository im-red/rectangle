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

#include "typeinfo.h"

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace rectangle
{
namespace backend
{
namespace builtin
{

struct FieldInfo
{
    FieldInfo(int index_, const std::string &name_, const std::shared_ptr<TypeInfo> &type_)
        : index(index_), name(name_), type(type_)
    {}
    int index;
    std::string name;
    std::shared_ptr<TypeInfo> type;
};

class StructInfo
{
public:
    StructInfo(const std::string &name, const std::vector<std::pair<std::string, std::shared_ptr<TypeInfo> > > &fields);

    int fieldIndex(const std::string &name) const;
    int fieldCount() const;
    FieldInfo fieldAt(int index) const;

    std::string name() const;

private:
    std::string m_name;
    std::map<std::string, FieldInfo> m_name2field;
};

extern const StructInfo sceneInfo;
extern const StructInfo rectInfo;
extern const StructInfo textInfo;
extern const StructInfo ellipseInfo;
extern const StructInfo polygonInfo;
extern const StructInfo lineInfo;
extern const StructInfo polylineInfo;

extern std::vector<const StructInfo *> infoList;

}
}
}
