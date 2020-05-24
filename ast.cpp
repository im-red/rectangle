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

#include "ast.h"

#include <map>

#include <assert.h>

using namespace std;

Expr::~Expr()
{

}

Stmt::~Stmt()
{

}

BindingDecl::~BindingDecl()
{

}

PropertyDecl::~PropertyDecl()
{

}

ASTNode::~ASTNode()
{

}

#define ENUM_ELEMENT(x) { x, #x }
std::string BinaryOperatorExpr::typeString(BinaryOperatorExpr::Op type)
{
    static const map<Op, string> MAP =
    {
        { Op::LogicalAnd, "&&" },
        { Op::LogicalOr, "||" },
        { Op::Equal, "==" },
        { Op::NotEqual, "!=" },
        { Op::LessThan, "<" },
        { Op::GreaterThan, ">" },
        { Op::LessEqual, "<=" },
        { Op::GreaterEqual, ">=" },
        { Op::Plus, "+" },
        { Op::Minus, "-" },
        { Op::Multiply, "*" },
        { Op::Divide, "/" },
        { Op::Remainder, "%" },
        { Op::Assign, "=" }
    };

    auto iter = MAP.find(type);
    assert(iter != MAP.end());

    return iter->second;
}

string UnaryOperatorExpr::typeString(UnaryOperatorExpr::Op type)
{
    static const map<Op, string> MAP =
    {
        ENUM_ELEMENT(Op::Positive),
        ENUM_ELEMENT(Op::Negative),
        ENUM_ELEMENT(Op::Not)
    };

    auto iter = MAP.find(type);
    assert(iter != MAP.end());

    return iter->second;
}

TypeInfo::TypeInfo(TypeInfo::Category cat)
    : m_category(cat)
{

}

TypeInfo::~TypeInfo()
{

}

std::string TypeInfo::toString() const
{
    static const map<Category, string> MAP =
    {
        { Category::Int,    "int" },
        { Category::Void,   "void" },
        { Category::Point,  "point" },
        { Category::Float,  "float" },
        { Category::String, "string" },
        { Category::List,   "list" },
        { Category::Custom, "custom" }
    };

    auto iter = MAP.find(category());
    assert(iter != MAP.end());

    return iter->second;
}

TypeInfo::Category TypeInfo::category() const
{
    return m_category;
}

bool TypeInfo::assignCompatible(const std::shared_ptr<TypeInfo> &rhs) const
{
    if (operator==(*rhs))
    {
        return true;
    }
    if (rhs->m_category == TypeInfo::Category::Void)
    {
        return true;
    }
    if (m_category == Category::Point && rhs->m_category == Category::List)
    {
        ListTypeInfo *lti = dynamic_cast<ListTypeInfo *>(rhs.get());
        assert(lti != nullptr);
        if (lti->elementType()->category() == Category::Int)
        {
            return true;
        }
    }
    return false;
}

ListTypeInfo::ListTypeInfo(const std::shared_ptr<TypeInfo> &ele)
    : TypeInfo(Category::List)
    , m_elementType(ele)
{

}

string ListTypeInfo::toString() const
{
    const string elementTypeString = m_elementType->toString();
    const string result = "list<" + elementTypeString + ">";
    return result;
}

bool ListTypeInfo::assignCompatible(const std::shared_ptr<TypeInfo> &rhs) const
{
    if (rhs->category() != Category::List)
    {
        return false;
    }

    ListTypeInfo *lti = dynamic_cast<ListTypeInfo *>(rhs.get());
    assert(lti != nullptr);
    return elementType()->assignCompatible(lti->elementType());
}

CustomTypeInfo::CustomTypeInfo(const string &name)
    : TypeInfo(Category::Custom)
    , m_name(name)
{

}

string CustomTypeInfo::toString() const
{
    return m_name;
}

GroupTypeInfo::GroupTypeInfo(const string &name, const std::shared_ptr<TypeInfo> &component)
    : TypeInfo(TypeInfo::Category::Group), m_name(name), m_componentType(component)
{

}

string GroupTypeInfo::toString() const
{
    return "(" + m_componentType->toString() + "::" + m_name + ")";
}
