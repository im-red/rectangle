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

#include "astnode.h"
#include "typeinfo.h"

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

string BindingDecl::bindingId() const
{
    assert(componentInstance != nullptr);
    assert(componentInstance->instanceId != "");
    assert(propertyDecl != nullptr);
    assert(propertyDecl->fieldIndex != -1);

    return componentInstance->instanceId + "[" + to_string(propertyDecl->fieldIndex) + "]";
}

int BindingDecl::fieldIndex() const
{
    assert(propertyDecl != nullptr);
    assert(propertyDecl->fieldIndex != -1);

    return propertyDecl->fieldIndex;
}

int BindingDecl::instanceIndex() const
{
    assert(componentInstance != nullptr);
    assert(componentInstance->instanceIndex != -1);

    return componentInstance->instanceIndex;
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

std::vector<ComponentInstanceDecl *> ComponentInstanceDecl::instanceList()
{
    vector<ComponentInstanceDecl *> result;

    vector<ComponentInstanceDecl *> stack({this});
    while (stack.size() != 0)
    {
        ComponentInstanceDecl *back = stack.back();
        stack.pop_back();
        result.push_back(back);
        for (auto &instance : back->childrenList)
        {
            stack.push_back(instance.get());
        }
    }

    return result;
}

std::vector<int> ComponentInstanceDecl::unboundProperty() const
{
    vector<bool> bound(componentDefination->propertyList.size(), false);
    for (auto &b : bindingList)
    {
        if (!b->isId())
        {
            bound[static_cast<size_t>(b->fieldIndex())] = true;
        }
    }
    vector<int> result;
    for (size_t i = 0; i < bound.size(); i++)
    {
        if (bound[i] == false)
        {
            result.push_back(static_cast<int>(i));
        }
    }
    return result;
}
