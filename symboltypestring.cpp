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

#include "symbol.h"

#define ENUM_ELEMENT(x) { x, #x }

std::string Symbol::symbolCategoryString(Category category)
{
    static const std::map<Category, std::string> MAP =
    {
        ENUM_ELEMENT(Category::Invalid),
        ENUM_ELEMENT(Category::Variable),
        ENUM_ELEMENT(Category::Parameter),
        ENUM_ELEMENT(Category::Struct),
        ENUM_ELEMENT(Category::Member),
        ENUM_ELEMENT(Category::Component),
        ENUM_ELEMENT(Category::PropertyGroup),
        ENUM_ELEMENT(Category::Property),
        ENUM_ELEMENT(Category::Method),
        ENUM_ELEMENT(Category::InstanceId),
        ENUM_ELEMENT(Category::Enum),
        ENUM_ELEMENT(Category::EnumConstants),
        ENUM_ELEMENT(Category::Function),
        ENUM_ELEMENT(Category::BuiltInType)
    };

    return MAP.at(category);
}

std::string Scope::scopeCategoryString(Category category)
{
    static const std::map<Category, std::string> MAP =
    {
        ENUM_ELEMENT(Category::Invalid),
        ENUM_ELEMENT(Category::Global),
        ENUM_ELEMENT(Category::Local),
        ENUM_ELEMENT(Category::Function),
        ENUM_ELEMENT(Category::Component),
        ENUM_ELEMENT(Category::Method),
        ENUM_ELEMENT(Category::Group),
        ENUM_ELEMENT(Category::Struct),
        ENUM_ELEMENT(Category::Instance)
    };

    return MAP.at(category);
}
