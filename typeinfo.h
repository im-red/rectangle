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

#include <string>
#include <memory>

class TypeInfo
{
public:
    enum class Category
    {
        Int,
        Void,
        Float,
        String,
        List,
        Custom,
    };

public:
    explicit TypeInfo(Category cat);
    virtual ~TypeInfo();

    Category category() const;
    bool operator==(const TypeInfo &rhs) const
    {
        return m_category == rhs.m_category && toString() == rhs.toString();
    }
    bool operator!=(const TypeInfo &rhs) const
    {
        return !operator==(rhs);
    }

    virtual std::string toString() const;
    virtual bool assignCompatible(const std::shared_ptr<TypeInfo> &rhs) const;

private:
    Category m_category;
};

class ListTypeInfo : public TypeInfo
{
public:
    explicit ListTypeInfo(const std::shared_ptr<TypeInfo> &ele);
    std::shared_ptr<TypeInfo> elementType() const { return m_elementType; }

    std::string toString() const override;
    bool assignCompatible(const std::shared_ptr<TypeInfo> &rhs) const override;

private:
    std::shared_ptr<TypeInfo> m_elementType;
};

class CustomTypeInfo : public TypeInfo
{
public:
    explicit CustomTypeInfo(const std::string &name);
    std::string name() const { return m_name; }

    std::string toString() const override;

private:
    std::string m_name;
};
