#include "parser.h"

#include <assert.h>

using namespace std;

#define ENUM_ELEMENT(x) { x, #x }

std::string Parser::parserRuleString(ParserRule rule)
{
    static const std::map<int, std::string> STRING_MAP =
    {
        ENUM_ELEMENT(Document),
        ENUM_ELEMENT(ClassDefination),
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
        ENUM_ELEMENT(ClassInstance),
        ENUM_ELEMENT(BindingItemList),
        ENUM_ELEMENT(BindingItem)
    };

    assert(STRING_MAP.size() == RuleCount);
    assert(rule >= 0 && rule < RuleCount);

    return STRING_MAP.at(rule);
}

string Parser::parserErrorString(Parser::ParserError err)
{
    (void) err;
    return "";
}
