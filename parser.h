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

#include "ast.h"
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
        ExprStatement,
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

    void incTrying();
    void decTrying();
    bool trying() const { return m_trying > 0; }

private:
    std::unique_ptr<DocumentDecl> parseDocument();
    std::unique_ptr<ClassDefinationDecl> parseClassDefination();
    void parseMemberItemList(std::unique_ptr<ClassDefinationDecl> &defination);
    void parseMemberItem(std::unique_ptr<ClassDefinationDecl> &defination);
    bool tryMemberItemAlt1();
    bool tryMemberItemAlt2();
    std::unique_ptr<PropertyDecl> parsePropertyDefination();
    TypeInfo parsePropertyType();
    TypeInfo parseType();
    void parseListType();
    std::unique_ptr<Expr> parseLiteral();
    std::unique_ptr<FunctionDecl> parseFunctionDefination();
    void parseParamList(std::vector<std::unique_ptr<ParamDecl> > &paramList);
    std::unique_ptr<ParamDecl> parseParamItem();
    std::unique_ptr<Stmt> parseCompoundStatement();
    void parseBlockItemList(std::vector<std::unique_ptr<Stmt> > &stmts);
    void parseBlockItem(std::vector<std::unique_ptr<Stmt>> &stmts);
    bool tryBlockItemAlt1();
    bool tryBlockItemAlt2();
    std::unique_ptr<Stmt> parseDeclaration();
    std::unique_ptr<Expr> parseInitializer();
    std::unique_ptr<Expr> parseInitializerList();
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseLogicalOrExpression();
    std::unique_ptr<Expr> parseLogicalAndExpression();
    std::unique_ptr<Expr> parseEqualityExpression();
    std::unique_ptr<Expr> parseRelationalExpression();
    std::unique_ptr<Expr> parseAdditiveExpression();
    std::unique_ptr<Expr> parseMultiplicativeExpression();
    std::unique_ptr<Expr> parseUnaryExpression();
    void parseUnaryOperator();
    std::unique_ptr<Expr> parsePostfixExpression();
    std::unique_ptr<Expr> parsePrimaryExpression();
    void parseArgumentExpressionList(std::unique_ptr<CallExpr> &callExpr);
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseSelectionStatement();
    std::unique_ptr<Stmt> parseIterationStatement();
    std::unique_ptr<Stmt> parseJumpStatement();
    std::unique_ptr<Stmt> parseExprStatement();
    std::unique_ptr<EnumDecl> parseEnumDefination();
    void parseEnumConstantList(std::unique_ptr<EnumDecl> &enumDecl);
    void parseEnumConstant(std::unique_ptr<EnumDecl> &enumDecl);
    std::unique_ptr<ClassInstanceDecl> parseClassInstance();
    void parseBindingItemList(std::unique_ptr<ClassInstanceDecl> &instanceDecl);
    void parseBindingItem(std::unique_ptr<ClassInstanceDecl> &instanceDecl);

private:
    std::vector<Token> m_tokens;
    int m_index = 0;
    std::map<int, std::map<int, int>> m_memory;
    int m_trying = 0;
    std::unique_ptr<DocumentDecl> m_document;
};

