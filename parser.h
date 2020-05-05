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

#include "lexer.h"

#include <map>
#include <vector>
#include <stdexcept>

class ParseException : public std::runtime_error
{
public:
    explicit ParseException(const std::string &s)
        : std::runtime_error(s)
    {}
};

class Parser
{
public:
    enum MemoryResult
    {
        MemoryFail = -2,
        MemoryUnknown = -1
    };

    enum ParserRule
    {
        Document,
        ClassDefination,
        MemberItemList,
        MemberItem,
        PropertyDefination,
        PropertyType,
        Type,
        ListType,
        Literal,
        FunctionDefination,
        ParamList,
        ParamItem,
        CompoundStatement,
        BlockItemList,
        BlockItem,
        Declaration,
        Initializer,
        InitializerList,
        Expression,
        LogicalOrExpression,
        LogicalAndExpression,
        EqualityExpression,
        RelationalExpression,
        AdditiveExpression,
        MultiplicativeExpression,
        UnaryExpression,
        UnaryOperator,
        PostfixExpression,
        PrimaryExpression,
        ArgumentExpressionList,
        Statement,
        SelectionStatement,
        IterationStatement,
        JumpStatement,
        EnumDefination,
        EnumConstantList,
        EnumConstant,
        ClassInstance,
        BindingItemList,
        BindingItem,
        RuleCount
    };

    enum ParserError
    {

    };

public:
    Parser();

    void setTokens(const std::vector<Token> &tokens);

    bool parse();
    bool parseRule(ParserRule rule, int index);

    static std::string parserRuleString(ParserRule rule);
    static std::string parserErrorString(ParserError err);

private:
    int tokenType(int i) const;
    int curTokenType() const { return tokenType(m_index); }

    Token token(int i) const;
    Token curToken() const { return token(m_index); }

    void consume();
    void match(int tokenType);

    int getMemory(int index, ParserRule rule);
    void updateMemory(int index, ParserRule rule, int result);

    void pushParseResult(ParserRule rule, int indent, int begin, int end);

    void incIndent();
    void decIndent();
    void incTrying();
    void decTrying();
    bool trying() const { return m_trying > 0; }

private:
    void parseDocument();
    void parseClassDefination();
    void parseMemberItemList();
    void parseMemberItem();
    void parsePropertyDefination();
    void parsePropertyType();
    void parseType();
    void parseListType();
    void parseLiteral();
    void parseFunctionDefination();
    void parseParamList();
    void parseParamItem();
    void parseCompoundStatement();
    void parseBlockItemList();
    void parseBlockItem();
    bool tryBlockItemAlt1();
    bool tryBlockItemAlt2();
    void parseDeclaration();
    void parseInitializer();
    void parseInitializerList();
    void parseExpression();
    void parseLogicalOrExpression();
    void parseLogicalAndExpression();
    void parseEqualityExpression();
    void parseRelationalExpression();
    void parseAdditiveExpression();
    void parseMultiplicativeExpression();
    void parseUnaryExpression();
    void parseUnaryOperator();
    void parsePostfixExpression();
    void parsePrimaryExpression();
    void parseArgumentExpressionList();
    void parseStatement();
    void parseSelectionStatement();
    void parseIterationStatement();
    void parseJumpStatement();
    void parseEnumDefination();
    void parseEnumConstantList();
    void parseEnumConstant();
    void parseClassInstance();
    void parseBindingItemList();
    void parseBindingItem();

private:
    std::vector<Token> m_tokens;
    int m_index = 0;
    int m_indent = 0;
    std::map<int, std::map<int, int>> m_memory;
    std::vector<std::string> m_parseResult;
    int m_trying = 0;
};

