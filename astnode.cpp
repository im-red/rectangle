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

void FunctionDecl::doPrint(int indent) const
{
    if (component)
    {
        util::condPrint(option::showAst, "FunctionDecl(%s %s::%s)\n", returnType->toString().c_str(), component->name.c_str(), name.c_str());
    }
    else
    {
        util::condPrint(option::showAst, "FunctionDecl(%s %s)\n", returnType->toString().c_str(), name.c_str());
    }

    for (auto &p : paramList)
    {
        p->print(indent + 1);
    }
    body->print(indent + 1);
}
