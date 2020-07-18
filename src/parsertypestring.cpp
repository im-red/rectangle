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

#include "parser.h"

#include <assert.h>

using namespace std;

#define ENUM_ELEMENT(x) { x, #x }

namespace rectangle
{
namespace frontend
{

std::string Parser::parserRuleString(ParserRule rule)
{
    static const std::map<int, std::string> STRING_MAP =
    {
        ENUM_ELEMENT(Document),
        ENUM_ELEMENT(ComponentDefination),
        ENUM_ELEMENT(MemberItemList),
        ENUM_ELEMENT(MemberItem),
        ENUM_ELEMENT(PropertyDefination),
        ENUM_ELEMENT(PropertyType),
        ENUM_ELEMENT(Type),
        ENUM_ELEMENT(ListType),
        ENUM_ELEMENT(Literal),
        ENUM_ELEMENT(FunctionDefination),
        ENUM_ELEMENT(ParamList),
        ENUM_ELEMENT(ParamItem),
        ENUM_ELEMENT(CompoundStatement),
        ENUM_ELEMENT(BlockItemList),
        ENUM_ELEMENT(BlockItem),
        ENUM_ELEMENT(Declaration),
        ENUM_ELEMENT(Initializer),
        ENUM_ELEMENT(InitializerList),
        ENUM_ELEMENT(Expression),
        ENUM_ELEMENT(LogicalOrExpression),
        ENUM_ELEMENT(LogicalAndExpression),
        ENUM_ELEMENT(EqualityExpression),
        ENUM_ELEMENT(RelationalExpression),
        ENUM_ELEMENT(AdditiveExpression),
        ENUM_ELEMENT(MultiplicativeExpression),
        ENUM_ELEMENT(UnaryExpression),
        ENUM_ELEMENT(UnaryOperator),
        ENUM_ELEMENT(PostfixExpression),
        ENUM_ELEMENT(PrimaryExpression),
        ENUM_ELEMENT(ArgumentExpressionList),
        ENUM_ELEMENT(Statement),
        ENUM_ELEMENT(SelectionStatement),
        ENUM_ELEMENT(IterationStatement),
        ENUM_ELEMENT(JumpStatement),
        ENUM_ELEMENT(ExprStatement),
        ENUM_ELEMENT(EnumDefination),
        ENUM_ELEMENT(EnumConstantList),
        ENUM_ELEMENT(EnumConstant),
        ENUM_ELEMENT(ComponentInstance),
        ENUM_ELEMENT(BindingItemList),
        ENUM_ELEMENT(BindingItem)
    };

    assert(STRING_MAP.size() == RuleCount);
    assert(rule >= 0 && rule < RuleCount);

    return STRING_MAP.at(rule);
}

}
}
