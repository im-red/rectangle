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
std::string BinaryOperatorExpr::typeString(BinaryOperatorExpr::Type type)
{
    static const map<Type, string> MAP =
    {
        { Type::LogicalAnd, "&&" },
        { Type::LogicalOr, "||" },
        { Type::Equal, "==" },
        { Type::NotEqual, "!=" },
        { Type::LessThan, "<" },
        { Type::GreaterThan, ">" },
        { Type::LessEqual, "<=" },
        { Type::GreaterEqual, ">=" },
        { Type::Plus, "+" },
        { Type::Minus, "-" },
        { Type::Multiply, "*" },
        { Type::Divide, "/" },
        { Type::Remainder, "%" },
        { Type::Assign, "=" }
    };

    auto iter = MAP.find(type);
    assert(iter != MAP.end());

    return iter->second;
}

string UnaryOperatorExpr::typeString(UnaryOperatorExpr::Type type)
{
    static const map<Type, string> MAP =
    {
        ENUM_ELEMENT(Type::Positive),
        ENUM_ELEMENT(Type::Negative),
        ENUM_ELEMENT(Type::Not)
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

    static auto iter = MAP.find(category());
    assert(iter != MAP.end());

    return iter->second;
}

TypeInfo::Category TypeInfo::category() const
{
    return m_category;
}

ListTypeInfo::ListTypeInfo(std::unique_ptr<TypeInfo> &&ele)
    : TypeInfo(Category::List)
    , m_elementType(move(ele))
{

}

string ListTypeInfo::toString() const
{
    static const string elementTypeString = m_elementType->toString();
    static const string result = "list<" + elementTypeString + ">";
    return result;
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
