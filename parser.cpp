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

static const int BUF_LEN = 512;

Parser::Parser()
{

}

void Parser::setTokens(const std::vector<Token> &tokens)
{
    m_tokens = tokens;
    m_index = 0;
    m_indent = 0;
    m_memory.clear();
    m_parseResult.clear();
}

bool Parser::parse()
{
    parseRule(Document, 0);
    return true;
}

bool Parser::parseRule(Parser::ParserRule rule, int index)
{
    bool result = true;
    m_index = index;
    try
    {
        switch(rule)
        {
        case Document:                  parseDocument();                    break;
        case ClassDefination:           parseClassDefination();             break;
        case MemberItemList:            parseMemberItemList();              break;
        case MemberItem:                parseMemberItem();                  break;
        case PropertyDefination:        parsePropertyDefination();          break;
        case Type:                      parseType();                        break;
        case PropertyType:              parsePropertyType();                break;
        case ListType:                  parseListType();                    break;
        case Literal:                   parseLiteral();                     break;
        case FunctionDefination:        parseFunctionDefination();          break;
        case ParamList:                 parseParamList();                   break;
        case ParamItem:                 parseParamItem();                   break;
        case CompoundStatement:         parseCompoundStatement();           break;
        case BlockItemList:             parseBlockItemList();               break;
        case BlockItem:                 parseBlockItem();                   break;
        case Declaration:               parseDeclaration();                 break;
        case Initializer:               parseInitializer();                 break;
        case InitializerList:           parseInitializerList();             break;
        case Expression:                parseExpression();                  break;
        case LogicalOrExpression:       parseLogicalOrExpression();         break;
        case LogicalAndExpression:      parseLogicalAndExpression();        break;
        case EqualityExpression:        parseEqualityExpression();          break;
        case RelationalExpression:      parseRelationalExpression();        break;
        case AdditiveExpression:        parseAdditiveExpression();          break;
        case MultiplicativeExpression:  parseMultiplicativeExpression();    break;
        case UnaryExpression:           parseUnaryExpression();             break;
        case UnaryOperator:             parseUnaryOperator();               break;
        case PostfixExpression:         parsePostfixExpression();           break;
        case PrimaryExpression:         parsePrimaryExpression();           break;
        case ArgumentExpressionList:    parseArgumentExpressionList();      break;
        case Statement:                 parseStatement();                   break;
        case SelectionStatement:        parseSelectionStatement();          break;
        case IterationStatement:        parseIterationStatement();          break;
        case JumpStatement:             parseJumpStatement();               break;
        case AssignmentStatement:       parseAssignmentStatement();         break;
        case EnumDefination:            parseEnumDefination();              break;
        case EnumConstantList:          parseEnumConstantList();            break;
        case EnumConstant:              parseEnumConstant();                break;
        case ClassInstance:             parseClassInstance();               break;
        case BindingItemList:           parseBindingItemList();             break;
        case BindingItem:               parseBindingItem();                 break;
        default :
        {
            fprintf(stderr, "Invalid ParserRule: %d\n", rule);
            assert(false);
        }
        }
    }
    catch (ParseException e)
    {
        result = false;
        fprintf(stderr, "%s\n", e.what());
    }
    for (int i = static_cast<int>(m_parseResult.size() - 1); i >= 0; i--)
    {
        printf("%s\n", m_parseResult[static_cast<size_t>(i)].c_str());
    }
    return result;
}

int Parser::tokenType(int i) const
{
    return token(i).type;
}

Token Parser::token(int i) const
{
    assert(i >= 0 && i < static_cast<int>(m_tokens.size()));
    return m_tokens[static_cast<size_t>(i)];
}

void Parser::consume()
{
    if (m_index < static_cast<int>(m_tokens.size()))
    {
        m_index++;
    }
}

void Parser::match(int tokenType)
{
    if (tokenType == curTokenType())
    {
        consume();
    }
    else
    {
        char buf[BUF_LEN];
        snprintf(buf, sizeof(buf), "expect %s but actual %s at line %d column %d(%s)",
                 Lexer::tokenTypeString(tokenType).c_str(),
                 Lexer::tokenTypeString(curTokenType()).c_str(),
                 curToken().line, curToken().column, curToken().str.c_str());
        throw ParseException(buf);
    }
}

int Parser::getMemory(int index, Parser::ParserRule rule)
{
    assert(index >= 0 && index < static_cast<int>(m_tokens.size()));
    assert(rule >= 0 && rule < RuleCount);

    auto iter = m_memory[index].find(rule);
    if (iter == m_memory[index].end())
    {
        return MemoryUnknown;
    }
    else
    {
        return iter->second;
    }
}

void Parser::updateMemory(int index, Parser::ParserRule rule, int result)
{
    assert(index >= 0 && index < static_cast<int>(m_tokens.size()));
    assert(rule >= 0 && rule < RuleCount);

    m_memory[index][rule] = result;
}

void Parser::pushParseResult(Parser::ParserRule rule, int indent, int begin, int end)
{
    string result(static_cast<size_t>(indent), ' ');
    result += (parserRuleString(rule) + ": ");
    for (int i = begin; i < end; i++)
    {
        result += (token(i).str + " ");
    }
    m_parseResult.push_back(result);
}

void Parser::pushParseResult(Parser::ParserRule rule, const string &info)
{
    string result(static_cast<size_t>(m_indent), ' ');
    result += (parserRuleString(rule) + ": ");
    result += info;
    m_parseResult.push_back(result);
}

void Parser::incIndent()
{
    m_indent++;
}

void Parser::decIndent()
{
    m_indent--;
}

void Parser::incTrying()
{
    m_trying++;
}

void Parser::decTrying()
{
    m_trying--;
}

void Parser::parseDocument()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch(curTokenType())
    {
    case Lexer::T_DEF: parseClassDefination(); break;
    case Lexer::T_IDENTIFIER: parseClassInstance(); break;
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect 'def'/Identifier at line %d column %d",
                 tok.line, tok.column);
        throw ParseException(buf);
    }
    }
    match(Lexer::T_EOF);

    if (!trying())
    {
        decIndent();
        pushParseResult(Document, m_indent, oldIndex, m_index);
    }
}

void Parser::parseClassDefination()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_DEF);
    match(Lexer::T_IDENTIFIER);
    match(Lexer::T_L_BRACE);
    parseMemberItemList();
    match(Lexer::T_R_BRACE);

    if (!trying())
    {
        decIndent();
        pushParseResult(ClassDefination, m_indent, oldIndex, m_index);
    }
}

void Parser::parseMemberItemList()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    set<int> first = {Lexer::T_INT, Lexer::T_VOID, Lexer::T_POINT,
                      Lexer::T_FLOAT, Lexer::T_STRING, Lexer::T_LIST,
                      Lexer::T_ENUM, Lexer::T_IDENTIFIER};

    while (curToken().isIn(first))
    {
        parseMemberItem();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(MemberItemList, m_indent, oldIndex, m_index);
    }
}

static const set<int> propertyTypeFirst =
{
    Lexer::T_INT,
    Lexer::T_POINT,
    Lexer::T_FLOAT,
    Lexer::T_STRING,
    Lexer::T_LIST
};
static const set<int> propertyDefinationFirst = propertyTypeFirst;

static const set<int> typeFirst =
{
    Lexer::T_INT,
    Lexer::T_POINT,
    Lexer::T_FLOAT,
    Lexer::T_STRING,
    Lexer::T_LIST,
    Lexer::T_VOID,
    Lexer::T_IDENTIFIER
};
static const set<int> &functionDefinationFirst = typeFirst;

//memberItem  // Int | Void | Point | Float | String | List | Enum | Identifier
//    : propertyDefination    // Int | Point | Float | String | List
//    | functionDefination    // Int | Void | Point | Float | String | List | Identifier
//    | enumDefination        // Enum
//    ;

void Parser::parseMemberItem()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    if (curToken().is(Lexer::T_ENUM))
    {
        parseEnumDefination();
    }
    else if (tryMemberItemAlt1())
    {
        parsePropertyDefination();
    }
    else if (tryMemberItemAlt2())
    {
        parseFunctionDefination();
    }
    else
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "(MemberItem)expect enumDefination/propertyDefination/functionDefination at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(MemberItem, m_indent, oldIndex, m_index);
    }
}

bool Parser::tryMemberItemAlt1()
{
    int indexBackup = m_index;
    incTrying();

    bool result = true;
    try
    {
        parsePropertyDefination();
    }
    catch (ParseException e)
    {
        fprintf(stderr, "tryMemberItemAlt1 fail: %s\n", e.what());
        result = false;
    }

    decTrying();
    m_index = indexBackup;

    return result;
}

bool Parser::tryMemberItemAlt2()
{
    int indexBackup = m_index;
    incTrying();

    bool result = true;
    try
    {
        parseFunctionDefination();
    }
    catch (ParseException e)
    {
        fprintf(stderr, "tryMemberItemAlt2 fail: %s\n", e.what());
        result = false;
    }

    decTrying();
    m_index = indexBackup;

    return result;
}

void Parser::parsePropertyDefination()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parsePropertyType();
    match(Lexer::T_IDENTIFIER);
    if (curToken().is(Lexer::T_COLON))
    {
        match(Lexer::T_COLON);
        parseInitializer();
    }
    else if (curToken().is(Lexer::T_DOT))
    {
        match(Lexer::T_DOT);
        match(Lexer::T_IDENTIFIER);
        match(Lexer::T_COLON);
        parseInitializer();
    }
    else
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect ':'/'.' at line %d column %d",
                 tok.line, tok.column);
        throw ParseException(buf);
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(PropertyDefination, m_indent, oldIndex, m_index);
    }
}

void Parser::parsePropertyType()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch(curTokenType())
    {
    case Lexer::T_INT:
    case Lexer::T_POINT:
    case Lexer::T_FLOAT:
    case Lexer::T_STRING:
    {
        match(curTokenType());
        break;
    }
    case Lexer::T_LIST: parseListType(); break;
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a type token at line %d column %d",
                 tok.line, tok.column);
        throw ParseException(buf);
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(PropertyType, m_indent, oldIndex, m_index);
    }
}

void Parser::parseType()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch(curTokenType())
    {
    case Lexer::T_INT:
    case Lexer::T_VOID:
    case Lexer::T_POINT:
    case Lexer::T_FLOAT:
    case Lexer::T_STRING:
    case Lexer::T_IDENTIFIER:
    {
        match(curTokenType());
        break;
    }
    case Lexer::T_LIST: parseListType(); break;
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a type token at line %d column %d",
                 tok.line, tok.column);
        throw ParseException(buf);
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(Type, m_indent, oldIndex, m_index);
    }
}

void Parser::parseListType()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_LIST);
    match(Lexer::T_LT);
    parsePropertyType();
    match(Lexer::T_GT);

    if (!trying())
    {
        decIndent();
        pushParseResult(ListType, m_indent, oldIndex, m_index);
    }
}

void Parser::parseLiteral()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch(curTokenType())
    {
    case Lexer::T_STRING_LITERAL:
    case Lexer::T_NUMBER_LITERAL:
    {
        match(curTokenType());
        break;
    }
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a literal token at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(Literal, m_indent, oldIndex, m_index);
    }
}

static const set<int> &paramListFirst = typeFirst;

void Parser::parseFunctionDefination()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseType();
    match(Lexer::T_IDENTIFIER);
    match(Lexer::T_L_PAREN);
    if (curToken().isIn(paramListFirst))
    {
        parseParamList();
    }
    match(Lexer::T_R_PAREN);
    parseCompoundStatement();

    if (!trying())
    {
        decIndent();
        pushParseResult(FunctionDefination, m_indent, oldIndex, m_index);
    }
}

void Parser::parseParamList()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseParamItem();
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
        parseParamItem();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(ParamList, m_indent, oldIndex, m_index);
    }
}

void Parser::parseParamItem()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseType();
    match(Lexer::T_IDENTIFIER);

    if (!trying())
    {
        decIndent();
        pushParseResult(ParamItem, m_indent, oldIndex, m_index);
    }
}

void Parser::parseCompoundStatement()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_L_BRACE);
    parseBlockItemList();
    match(Lexer::T_R_BRACE);

    if (!trying())
    {
        decIndent();
        pushParseResult(CompoundStatement, m_indent, oldIndex, m_index);
    }
}

static const set<int> blockItemFirst =
{
    Lexer::T_INT,
    Lexer::T_VOID,
    Lexer::T_POINT,
    Lexer::T_FLOAT,
    Lexer::T_STRING,
    Lexer::T_LIST,
    Lexer::T_IDENTIFIER,
    Lexer::T_L_BRACE,
    Lexer::T_IF,
    Lexer::T_WHILE,
    Lexer::T_CONTINUE,
    Lexer::T_BREAK,
    Lexer::T_RETURN,
    Lexer::T_STRING_LITERAL,
    Lexer::T_NUMBER_LITERAL,
    Lexer::T_L_PAREN
};

void Parser::parseBlockItemList()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    while (curToken().isIn(blockItemFirst))
    {
        parseBlockItem();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(BlockItemList, m_indent, oldIndex, m_index);
    }
}

//blockItem
//    : declaration   // Int | Void | Point | Float | String | List | Identifier
//    | statement     // LBrace | If | While | Continue | Break | Return | Identifier | StringLiteral | NumberLiteral | LParen
//    ;

void Parser::parseBlockItem()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    if (tryBlockItemAlt1())
    {
        parseDeclaration();
    }
    else if (tryBlockItemAlt2())
    {
        parseStatement();
    }
    else
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf),
                 "expect a delcaration/statement for blockItem at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(BlockItem, m_indent, oldIndex, m_index);
    }
}

bool Parser::tryBlockItemAlt1()
{
    int indexBackup = m_index;
    incTrying();

    bool result = true;
    try
    {
        parseDeclaration();
    }
    catch (ParseException e)
    {
        fprintf(stderr, "tryBlockItemAlt1 fail: %s\n", e.what());
        result = false;
    }

    decTrying();
    m_index = indexBackup;

    return result;
}

bool Parser::tryBlockItemAlt2()
{
    int indexBackup = m_index;
    incTrying();

    bool result = true;
    try
    {
        parseStatement();
    }
    catch (ParseException e)
    {
        fprintf(stderr, "tryBlockItemAlt2 fail: %s\n", e.what());
        result = false;
    }

    decTrying();
    m_index = indexBackup;

    return result;
}

void Parser::parseDeclaration()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseType();
    match(Lexer::T_IDENTIFIER);
    if (curToken().is(Lexer::T_ASSIGN))
    {
        match(Lexer::T_ASSIGN);
        parseInitializer();
    }
    match(Lexer::T_SEMICOLON);

    if (!trying())
    {
        decIndent();
        pushParseResult(Declaration, m_indent, oldIndex, m_index);
    }
}

static const set<int> expressionFirst =
{
    Lexer::T_IDENTIFIER,
    Lexer::T_STRING_LITERAL,
    Lexer::T_NUMBER_LITERAL,
    Lexer::T_L_PAREN,
    Lexer::T_PLUS,
    Lexer::T_MINUS,
    Lexer::T_NOT
};

static const set<int> initializerListFirst =
{
    Lexer::T_IDENTIFIER,
    Lexer::T_STRING_LITERAL,
    Lexer::T_NUMBER_LITERAL,
    Lexer::T_L_PAREN,
    Lexer::T_PLUS,
    Lexer::T_MINUS,
    Lexer::T_NOT,
    Lexer::T_L_BRACE
};

void Parser::parseInitializer()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    if (curToken().isIn(expressionFirst))
    {
        parseExpression();
    }
    else if (curToken().is(Lexer::T_L_BRACE))
    {
        match(Lexer::T_L_BRACE);
        if (curToken().isIn(initializerListFirst))
        {
            parseInitializerList();
        }
        match(Lexer::T_R_BRACE);
    }
    else
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a initializer at line %d column %d",
                 tok.line, tok.column);
        throw ParseException(buf);
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(Initializer, m_indent, oldIndex, m_index);
    }
}

void Parser::parseInitializerList()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseInitializer();
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
        parseInitializer();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(InitializerList, m_indent, oldIndex, m_index);
    }
}

//expression  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
//    : logicalOrExpression                   // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
//    | postfixExpression Assign expression   // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
//    ;

void Parser::parseExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseLogicalOrExpression();

    if (!trying())
    {
        decIndent();
        pushParseResult(Expression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseLogicalOrExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseLogicalAndExpression();
    while (curToken().is(Lexer::T_OR_OR))
    {
        match(Lexer::T_OR_OR);
        parseLogicalAndExpression();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(LogicalOrExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseLogicalAndExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseEqualityExpression();
    while (curToken().is(Lexer::T_AND_AND))
    {
        match(Lexer::T_AND_AND);
        parseEqualityExpression();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(LogicalAndExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseEqualityExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseRelationalExpression();
    while (curToken().isIn({Lexer::T_EQUAL, Lexer::T_NOT_EQUAL}))
    {
        match(curTokenType());
        parseRelationalExpression();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(EqualityExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseRelationalExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseAdditiveExpression();
    while (curToken().isIn({Lexer::T_LT, Lexer::T_GT, Lexer::T_LE, Lexer::T_GE}))
    {
        match(curTokenType());
        parseAdditiveExpression();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(RelationalExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseAdditiveExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseMultiplicativeExpression();
    while (curToken().isIn({Lexer::T_PLUS, Lexer::T_MINUS}))
    {
        match(curTokenType());
        parseMultiplicativeExpression();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(AdditiveExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseMultiplicativeExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseUnaryExpression();
    while (curToken().isIn({Lexer::T_STAR, Lexer::T_SLASH, Lexer::T_REMAINDER}))
    {
        match(curTokenType());
        parseUnaryExpression();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(MultiplicativeExpression, m_indent, oldIndex, m_index);
    }
}

static const set<int> postfixExpressionFirst =
{
    Lexer::T_IDENTIFIER,
    Lexer::T_STRING_LITERAL,
    Lexer::T_NUMBER_LITERAL,
    Lexer::T_L_PAREN
};

static const set<int> unaryOperatorFirst =
{
    Lexer::T_PLUS,
    Lexer::T_MINUS,
    Lexer::T_NOT
};

void Parser::parseUnaryExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    if (curToken().isIn(postfixExpressionFirst))
    {
        parsePostfixExpression();
    }
    else if (curToken().isIn(unaryOperatorFirst))
    {
        parseUnaryOperator();
        parseUnaryExpression();
    }
    else
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a unaryExpression at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(UnaryExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseUnaryOperator()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch (curTokenType())
    {
    case Lexer::T_PLUS:
    case Lexer::T_MINUS:
    case Lexer::T_NOT:
    {
        match(curTokenType());
        break;
    }
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect '+'/'-'/'!' at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(UnaryOperator, m_indent, oldIndex, m_index);
    }
}

static const set<int> argumentExpressionListFirst = expressionFirst;
void Parser::parsePostfixExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parsePrimaryExpression();
    while (curToken().isIn({Lexer::T_L_BRACKET, Lexer::T_L_PAREN, Lexer::T_DOT}))
    {
        int type = curTokenType();
        if (type == Lexer::T_L_BRACKET)
        {
            match(Lexer::T_L_BRACKET);
            parseExpression();
            match(Lexer::T_R_BRACKET);
        }
        else if (type == Lexer::T_L_PAREN)
        {
            match(Lexer::T_L_PAREN);
            if (curToken().isIn(argumentExpressionListFirst))
            {
                parseArgumentExpressionList();
            }
            match(Lexer::T_R_PAREN);
        }
        else
        {
            match(Lexer::T_DOT);
            match(Lexer::T_IDENTIFIER);
        }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(PostfixExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parsePrimaryExpression()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch (curTokenType())
    {
    case Lexer::T_IDENTIFIER: match(Lexer::T_IDENTIFIER); break;
    case Lexer::T_STRING_LITERAL:
    case Lexer::T_NUMBER_LITERAL:
    {
        parseLiteral();
        break;
    }
    case Lexer::T_L_PAREN:
    {
        match(Lexer::T_L_PAREN);
        parseExpression();
        match(Lexer::T_R_PAREN);
        break;
    }
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect Identifier/literal/'(' at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(PrimaryExpression, m_indent, oldIndex, m_index);
    }
}

void Parser::parseArgumentExpressionList()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseExpression();
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
        parseExpression();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(ArgumentExpressionList, m_indent, oldIndex, m_index);
    }
}

static const set<int> assignStatementFirst =
{
    Lexer::T_IDENTIFIER,
    Lexer::T_STRING_LITERAL,
    Lexer::T_NUMBER_LITERAL,
    Lexer::T_L_PAREN
};

void Parser::parseStatement()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch (curTokenType())
    {
    case Lexer::T_L_BRACE:  parseCompoundStatement();   break;
    case Lexer::T_IF:       parseSelectionStatement();  break;
    case Lexer::T_WHILE:    parseIterationStatement();  break;
    case Lexer::T_CONTINUE:
    case Lexer::T_BREAK:
    case Lexer::T_RETURN:   parseJumpStatement();       break;
    default:
    {
        if (curToken().isIn(assignStatementFirst))
        {
            parseAssignmentStatement();
        }
        else
        {
            char buf[BUF_LEN];
            Token tok = curToken();
            snprintf(buf, sizeof(buf), "expect a statement at line %d column %d(%s)",
                     tok.line, tok.column, tok.str.c_str());
            throw ParseException(buf);
        }
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(Statement, m_indent, oldIndex, m_index);
    }
}

void Parser::parseSelectionStatement()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_IF);
    match(Lexer::T_L_PAREN);
    parseExpression();
    match(Lexer::T_R_PAREN);
    parseCompoundStatement();
    if (curToken().is(Lexer::T_ELSE))
    {
        match(Lexer::T_ELSE);
        if (curToken().is(Lexer::T_L_BRACE))
        {
            parseCompoundStatement();
        }
        else if (curToken().is(Lexer::T_IF))
        {
            parseSelectionStatement();
        }
        else
        {
            char buf[BUF_LEN];
            Token tok = curToken();
            snprintf(buf, sizeof(buf), "expect a 'if'/compoundStatement at line %d column %d(%s)",
                     tok.line, tok.column, tok.str.c_str());
            throw ParseException(buf);
        }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(SelectionStatement, m_indent, oldIndex, m_index);
    }
}

void Parser::parseIterationStatement()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_WHILE);
    match(Lexer::T_L_PAREN);
    parseExpression();
    match(Lexer::T_R_PAREN);
    parseCompoundStatement();

    if (!trying())
    {
        decIndent();
        pushParseResult(IterationStatement, m_indent, oldIndex, m_index);
    }
}

void Parser::parseJumpStatement()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    switch (curTokenType())
    {
    case Lexer::T_CONTINUE:
    {
        match(Lexer::T_CONTINUE);
        match(Lexer::T_SEMICOLON);
        break;
    }
    case Lexer::T_BREAK:
    {
        match(Lexer::T_BREAK);
        match(Lexer::T_SEMICOLON);
        break;
    }
    case Lexer::T_RETURN:
    {
        match(Lexer::T_RETURN);
        if (curToken().isIn(expressionFirst))
        {
            parseExpression();
        }
        match(Lexer::T_SEMICOLON);
        break;
    }
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a 'continue'/'break'/'return' at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(JumpStatement, m_indent, oldIndex, m_index);
    }
}

void Parser::parseAssignmentStatement()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parsePostfixExpression();
    if (curToken().is(Lexer::T_ASSIGN))
    {
        match(Lexer::T_ASSIGN);
        parseExpression();
    }
    match(Lexer::T_SEMICOLON);

    if (!trying())
    {
        decIndent();
        pushParseResult(AssignmentStatement, m_indent, oldIndex, m_index);
    }
}

void Parser::parseEnumDefination()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_ENUM);
    match(Lexer::T_IDENTIFIER);
    match(Lexer::T_L_BRACE);
    parseEnumConstantList();
    match(Lexer::T_R_BRACE);

    if (!trying())
    {
        decIndent();
        pushParseResult(EnumDefination, m_indent, oldIndex, m_index);
    }
}

void Parser::parseEnumConstantList()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseEnumConstant();
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
        parseEnumConstant();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(EnumConstantList, m_indent, oldIndex, m_index);
    }
}

void Parser::parseEnumConstant()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_IDENTIFIER);

    if (!trying())
    {
        decIndent();
        pushParseResult(EnumConstant, m_indent, oldIndex, m_index);
    }
}

void Parser::parseClassInstance()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_IDENTIFIER);
    match(Lexer::T_L_BRACE);
    parseBindingItemList();
    match(Lexer::T_R_BRACE);

    if (!trying())
    {
        decIndent();
        pushParseResult(ClassInstance, m_indent, oldIndex, m_index);
    }
}

void Parser::parseBindingItemList()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    parseBindingItem();
    while (curToken().is(Lexer::T_IDENTIFIER))
    {
        parseBindingItem();
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(BindingItemList, m_indent, oldIndex, m_index);
    }
}

void Parser::parseBindingItem()
{
    int oldIndex = m_index;
    if (!trying())
    {
        incIndent();
    }

    match(Lexer::T_IDENTIFIER);
    switch (curTokenType())
    {
    case Lexer::T_COLON:
    {
        match(Lexer::T_COLON);
        parseInitializer();
        break;
    }
    case Lexer::T_DOT:
    {
        match(Lexer::T_DOT);
        match(Lexer::T_IDENTIFIER);
        match(Lexer::T_COLON);
        parseInitializer();
        break;
    }
    case Lexer::T_L_BRACE:
    {
        m_index--;
        parseClassInstance();
        break;
    }
    default:
    {
        char buf[BUF_LEN];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a ':'/'.'/'{' after Identifier at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }
    }

    if (!trying())
    {
        decIndent();
        pushParseResult(BindingItem, m_indent, oldIndex, m_index);
    }
}
