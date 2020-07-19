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
#include "option.h"
#include "util.h"
#include "typeinfo.h"
#include "exception.h"

#include <assert.h>

using namespace std;
using namespace rectangle::backend;
using namespace rectangle::diag;

namespace rectangle
{
namespace frontend
{

Parser::Parser()
{

}

void Parser::clear()
{
    m_tokens.clear();
    m_document.reset();
    m_index = 0;
    m_trying = 0;
}

std::unique_ptr<DocumentDecl> Parser::parse(const std::vector<Token> &tokens)
{
    clear();

    m_tokens = tokens;
    m_document = parseDocument();

    return move(m_document);
}

Token::TokenType Parser::tokenType(int i) const
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

void Parser::match(Token::TokenType tokenType)
{
    if (tokenType == curTokenType())
    {
        consume();
    }
    else
    {
        char buf[512];
        snprintf(buf, sizeof(buf), "Expect a %s but actual a %s",
                 Token::tokenTypeString(tokenType).c_str(),
                 Token::tokenTypeString(curTokenType()).c_str());
        throw SyntaxError(buf, curToken());
    }
}

void Parser::incTrying()
{
    m_trying++;
}

void Parser::decTrying()
{
    m_trying--;
}

std::unique_ptr<DocumentDecl> Parser::parseDocument()
{
    unique_ptr<ComponentDefinationDecl> def;
    unique_ptr<ComponentInstanceDecl> instance;

    switch(curTokenType())
    {
    case Token::T_DEF:
    {
        def = parseComponentDefination();
        break;
    }
    case Token::T_IDENTIFIER:
    {
        instance = parseComponentInstance();
        break;
    }
    default:
    {
        const char *msg = "Expect a 'def' / Identifier";
        throw SyntaxError(msg, curToken());
    }
    }
    match(Token::T_EOF);

    unique_ptr<DocumentDecl> doc;
    if (!trying())
    {
        if (def)
        {
            doc.reset(def.release());
        }
        else if (instance)
        {
            doc.reset(instance.release());
        }
        else
        {
            assert(false);
        }
    }

    return doc;
}

std::unique_ptr<ComponentDefinationDecl> Parser::parseComponentDefination()
{
    unique_ptr<ComponentDefinationDecl> defination;
    string typeName;

    if (!trying())
    {
        defination.reset(new ComponentDefinationDecl);
    }

    match(Token::T_DEF);
    match(Token::T_IDENTIFIER);
    Token tok = token(m_index - 1);
    typeName = token(m_index - 1).str;

    match(Token::T_L_BRACE);
    parseMemberItemList(defination);
    match(Token::T_R_BRACE);

    if (!trying())
    {
        defination->name = typeName;
        defination->tok = tok;
    }

    return defination;
}

static const set<int> memberItemFirst =
{
    Token::T_INT,
    Token::T_VOID,
    Token::T_FLOAT,
    Token::T_STRING,
    Token::T_LIST,
    Token::T_ENUM,
    Token::T_IDENTIFIER
};

void Parser::parseMemberItemList(unique_ptr<ComponentDefinationDecl> &defination)
{
    while (curToken().isIn(memberItemFirst))
    {
        parseMemberItem(defination);
    }
}

static const set<int> propertyTypeFirst =
{
    Token::T_INT,
    Token::T_FLOAT,
    Token::T_STRING,
    Token::T_LIST
};
static const set<int> propertyDefinationFirst = propertyTypeFirst;

static const set<int> typeFirst =
{
    Token::T_INT,
    Token::T_FLOAT,
    Token::T_STRING,
    Token::T_LIST,
    Token::T_VOID,
    Token::T_IDENTIFIER
};

//memberItem  // Int | Void | Float | String | List | Enum | Identifier
//    : propertyDefination    // Int | Float | String | List
//    | functionDefination    // Int | Void | Float | String | List | Identifier
//    | enumDefination        // Enum
//    ;

void Parser::parseMemberItem(unique_ptr<ComponentDefinationDecl> &defination)
{
    unique_ptr<EnumDecl> enumDecl;
    unique_ptr<PropertyDecl> propertyDecl;
    unique_ptr<FunctionDecl> functionDecl;

    if (curToken().is(Token::T_ENUM))
    {
        enumDecl = parseEnumDefination();
    }
    else if (tryMemberItemAlt1())
    {
        propertyDecl = parsePropertyDefination();
    }
    else if (tryMemberItemAlt2())
    {
        functionDecl = parseFunctionDefination();
        functionDecl->component = defination.get();
    }
    else
    {
        const char *msg = "Expect a enumDefination / propertyDefination / functionDefination";
        throw SyntaxError(msg, curToken());
    }

    if (!trying())
    {
        if (enumDecl)
        {
            defination->enumList.push_back(move(enumDecl));
        }
        else if (propertyDecl)
        {
            propertyDecl->componentDefination = defination.get();
            defination->propertyList.push_back(move(propertyDecl));
        }
        else if (functionDecl)
        {
            defination->methodList.push_back(move(functionDecl));
        }
        else
        {
            assert(false);
        }
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
    catch (exception &e)
    {
        util::condPrint(option::showLLTry, "tryMemberItemAlt1 fail: %s\n", e.what());
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
    catch (exception &e)
    {
        util::condPrint(option::showLLTry, "tryMemberItemAlt2 fail: %s\n", e.what());
        result = false;
        throw;
    }

    decTrying();
    m_index = indexBackup;

    return result;
}

std::unique_ptr<PropertyDecl> Parser::parsePropertyDefination()
{
    string name;
    shared_ptr<TypeInfo> ti;
    unique_ptr<Expr> initExpr;

    ti = parsePropertyType();
    match(Token::T_IDENTIFIER);
    Token tok = token(m_index - 1);
    name = token(m_index - 1).str;

    match(Token::T_COLON);
    initExpr = parseInitializer();

    unique_ptr<PropertyDecl> propertyDecl;

    if (!trying())
    {
        propertyDecl.reset(new PropertyDecl);
        propertyDecl->name = name;
        propertyDecl->type = move(ti);
        propertyDecl->expr = move(initExpr);
        propertyDecl->tok = tok;
    }

    return propertyDecl;
}

std::shared_ptr<TypeInfo> Parser::parsePropertyType()
{
    shared_ptr<TypeInfo> result;

    switch(curTokenType())
    {
    case Token::T_INT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Int));
        match(curTokenType());
        break;
    }
    case Token::T_FLOAT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Float));
        match(curTokenType());
        break;
    }
    case Token::T_STRING:
    {
        result.reset(new TypeInfo(TypeInfo::Category::String));
        match(curTokenType());
        break;
    }
    case Token::T_LIST:
    {
        result = parseListType();
        break;
    }
    default:
    {
        const char *msg = "Expect a type token";
        throw SyntaxError(msg, curToken());
    }
    }

    if (trying())
    {
        result.reset();
    }

    return result;
}

std::shared_ptr<TypeInfo> Parser::parseType()
{
    shared_ptr<TypeInfo> result;

    switch(curTokenType())
    {
    case Token::T_INT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Int));
        match(curTokenType());
        break;
    }
    case Token::T_VOID:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Void));
        match(curTokenType());
        break;
    }
    case Token::T_FLOAT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Float));
        match(curTokenType());
        break;
    }
    case Token::T_STRING:
    {
        result.reset(new TypeInfo(TypeInfo::Category::String));
        match(curTokenType());
        break;
    }
    case Token::T_IDENTIFIER:
    {
        result.reset(new CustomTypeInfo(curToken().str));
        match(curTokenType());
        break;
    }
    case Token::T_LIST:
    {
        result = parseListType();
        break;
    }
    default:
    {
        const char *msg = "Expect a type token";
        throw SyntaxError(msg, curToken());
    }
    }

    if (trying())
    {
        result.reset();
    }

    return result;
}

std::shared_ptr<TypeInfo> Parser::parseListType()
{
    match(Token::T_LIST);
    match(Token::T_LT);
    shared_ptr<TypeInfo> ele = parsePropertyType();
    match(Token::T_GT);

    shared_ptr<TypeInfo> result;
    if (!trying())
    {
        result.reset(new ListTypeInfo(ele));
    }

    return result;
}

std::unique_ptr<Expr> Parser::parseLiteral()
{
    unique_ptr<Expr> stringExpr;
    unique_ptr<Expr> intExpr;
    unique_ptr<Expr> floatExpr;

    switch(curTokenType())
    {
    case Token::T_STRING_LITERAL:
    {
        match(curTokenType());
        string s = token(m_index - 1).str;
        stringExpr.reset(new StringLiteral(s));
        stringExpr->tok = token(m_index - 1);
        break;
    }
    case Token::T_NUMBER_LITERAL:
    {
        match(curTokenType());
        string s = token(m_index - 1).str;
        if (s.find('.') == string::npos)
        {
            int i = stoi(s);
            intExpr.reset(new IntegerLiteral(i));
            intExpr->tok = token(m_index - 1);
        }
        else
        {
            float f = stof(s);
            floatExpr.reset(new FloatLiteral(f));
            floatExpr->tok = token(m_index - 1);
        }
        break;
    }
    default:
    {
        const char *msg = "Expect a literal token";
        throw SyntaxError(msg, curToken());
    }
    }

    unique_ptr<Expr> expr;
    if (!trying())
    {
        if (stringExpr)
        {
            expr = move(stringExpr);
        }
        else if (intExpr)
        {
            expr = move(intExpr);
        }
        else if (floatExpr)
        {
            expr = move(floatExpr);
        }
        else
        {
            assert(false);
        }
    }

    return expr;
}

static const set<int> &paramListFirst = typeFirst;

std::unique_ptr<FunctionDecl> Parser::parseFunctionDefination()
{
    string name;
    shared_ptr<TypeInfo> ti;
    vector<unique_ptr<ParamDecl>> paramList;
    unique_ptr<Stmt> body;

    ti = parseType();
    match(Token::T_IDENTIFIER);
    Token tok = token(m_index - 1);
    name = token(m_index - 1).str;
    match(Token::T_L_PAREN);
    if (curToken().isIn(paramListFirst))
    {
        parseParamList(paramList);
    }
    match(Token::T_R_PAREN);
    body = parseCompoundStatement();

    unique_ptr<FunctionDecl> decl;
    if (!trying())
    {
        CompoundStmt *stmts = dynamic_cast<CompoundStmt *>(body.get());
        assert(stmts != nullptr);
        if (stmts->stmtList.back()->category != Stmt::Category::Return)
        {
            stmts->stmtList.emplace_back(new ReturnStmt(std::unique_ptr<Expr>()));
        }
        decl.reset(new FunctionDecl(name,
                                    move(ti),
                                    move(paramList),
                                    unique_ptr<CompoundStmt>(dynamic_cast<CompoundStmt *>(body.release()))));
        decl->tok = tok;
    }
    return decl;
}

void Parser::parseParamList(std::vector<std::unique_ptr<ParamDecl>> &paramList)
{
    vector<unique_ptr<ParamDecl>> pl;

    pl.push_back(parseParamItem());
    while (curToken().is(Token::T_COMMA))
    {
        match(Token::T_COMMA);
        pl.push_back(parseParamItem());
    }

    if (!trying())
    {
        paramList = move(pl);
    }
}

std::unique_ptr<ParamDecl> Parser::parseParamItem()
{
    shared_ptr<TypeInfo> ti;
    string name;

    ti = parseType();
    match(Token::T_IDENTIFIER);
    Token tok = token(m_index - 1);
    name = tok.str;

    unique_ptr<ParamDecl> decl;
    if (!trying())
    {
        decl.reset(new ParamDecl(name, move(ti)));
        decl->tok = tok;
    }

    return decl;
}

std::unique_ptr<Stmt> Parser::parseCompoundStatement()
{
    vector<unique_ptr<Stmt>> stmts;

    match(Token::T_L_BRACE);
    parseBlockItemList(stmts);
    match(Token::T_R_BRACE);

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        stmt.reset(new CompoundStmt(move(stmts)));
    }

    return stmt;
}

static const set<int> blockItemFirst =
{
    Token::T_INT,
    Token::T_VOID,
    Token::T_FLOAT,
    Token::T_STRING,
    Token::T_LIST,
    Token::T_IDENTIFIER,
    Token::T_L_BRACE,
    Token::T_IF,
    Token::T_WHILE,
    Token::T_CONTINUE,
    Token::T_BREAK,
    Token::T_RETURN,
    Token::T_STRING_LITERAL,
    Token::T_NUMBER_LITERAL,
    Token::T_L_PAREN
};

void Parser::parseBlockItemList(std::vector<std::unique_ptr<Stmt>> &stmts)
{
    while (curToken().isIn(blockItemFirst))
    {
        parseBlockItem(stmts);
    }
}

//blockItem
//    : declaration   // Int | Void | Float | String | List | Identifier
//    | statement     // LBrace | If | While | Continue | Break | Return | Identifier | StringLiteral | NumberLiteral | LParen
//    ;

void Parser::parseBlockItem(std::vector<std::unique_ptr<Stmt>> &stmts)
{
    unique_ptr<Stmt> stmt;
    if (tryBlockItemAlt2())
    {
        stmt = parseStatement();
    }
    else if (tryBlockItemAlt1())
    {
        stmt = parseDeclaration();
    }
    else
    {
        const char *msg = "Expect a delcaration / statement";
        throw SyntaxError(msg, curToken());
    }

    if (!trying())
    {
        stmts.push_back(move(stmt));
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
    catch (exception &e)
    {
        util::condPrint(option::showLLTry, "tryBlockItemAlt1 fail: %s\n", e.what());
        result = false;
        throw;
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
    catch (exception &e)
    {
        util::condPrint(option::showLLTry, "tryBlockItemAlt2 fail: %s\n", e.what());
        result = false;
    }

    decTrying();
    m_index = indexBackup;

    return result;
}

std::unique_ptr<Stmt> Parser::parseDeclaration()
{
    unique_ptr<VarDecl> decl(new VarDecl);

    decl->typeTok = curToken();
    decl->type = parseType();
    match(Token::T_IDENTIFIER);
    Token tok = token(m_index - 1);
    decl->tok = tok;
    decl->name = tok.str;
    if (curToken().is(Token::T_ASSIGN))
    {
        match(Token::T_ASSIGN);
        decl->expr = parseInitializer();
    }
    match(Token::T_SEMICOLON);

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        stmt.reset(new DeclStmt(move(decl)));
    }

    return stmt;
}

static const set<int> expressionFirst =
{
    Token::T_IDENTIFIER,
    Token::T_STRING_LITERAL,
    Token::T_NUMBER_LITERAL,
    Token::T_L_PAREN,
    Token::T_PLUS,
    Token::T_MINUS,
    Token::T_NOT
};

static const set<int> initializerListFirst =
{
    Token::T_IDENTIFIER,
    Token::T_STRING_LITERAL,
    Token::T_NUMBER_LITERAL,
    Token::T_L_PAREN,
    Token::T_PLUS,
    Token::T_MINUS,
    Token::T_NOT,
    Token::T_L_BRACE
};

std::unique_ptr<Expr> Parser::parseInitializer()
{
    unique_ptr<Expr> expr;

    if (curToken().isIn(expressionFirst))
    {
        expr = parseExpression();
    }
    else if (curToken().is(Token::T_L_BRACE))
    {
        match(Token::T_L_BRACE);
        if (curToken().isIn(initializerListFirst))
        {
            expr = parseInitializerList();
        }
        else
        {
            expr.reset(new InitListExpr({}));
        }
        match(Token::T_R_BRACE);
    }
    else
    {
        const char *msg = "Expect a initializer";
        throw SyntaxError(msg, curToken());
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseInitializerList()
{
    vector<unique_ptr<Expr>> exprs;

    exprs.push_back(parseInitializer());
    while (curToken().is(Token::T_COMMA))
    {
        match(Token::T_COMMA);
        exprs.push_back(parseInitializer());
    }

    unique_ptr<Expr> expr;
    if (!trying())
    {
        expr.reset(new InitListExpr(move(exprs)));
    }

    return expr;
}

//expression  // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
//    : logicalOrExpression                   // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
//    | postfixExpression Assign expression   // Identifier | StringLiteral | NumberLiteral | LParen | Plus | Minus | Not
//    ;

std::unique_ptr<Expr> Parser::parseExpression()
{
    unique_ptr<Expr> expr = parseLogicalOrExpression();

    return expr;
}

std::unique_ptr<Expr> Parser::parseLogicalOrExpression()
{
    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> andExprs;

    andExprs.push_back(parseLogicalAndExpression());
    while (curToken().is(Token::T_OR_OR))
    {
        match(Token::T_OR_OR);
        andExprs.push_back(parseLogicalAndExpression());
    }

    if (!trying())
    {
        const size_t andExprCount = andExprs.size();
        assert(andExprCount != 0);

        if (andExprCount == 1)
        {
            expr = std::move(andExprs[0]);
        }
        else
        {
            for (size_t i = 0; i < andExprCount - 1; i++)
            {
                BinaryOperatorExpr *op = new BinaryOperatorExpr;
                op->op = BinaryOperatorExpr::Op::LogicalOr;
                op->left = move(andExprs[i]);
                op->right = move(andExprs[i + 1]);
                andExprs[i + 1].reset(op);
            }
            expr = move(andExprs.back());
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseLogicalAndExpression()
{
    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> eqExprs;

    eqExprs.push_back(parseEqualityExpression());
    while (curToken().is(Token::T_AND_AND))
    {
        match(Token::T_AND_AND);
        eqExprs.push_back(parseEqualityExpression());
    }

    if (!trying())
    {
        const size_t eqExprCount = eqExprs.size();
        assert(eqExprCount != 0);

        if (eqExprCount == 1)
        {
            expr = move(eqExprs[0]);
        }
        else
        {
            for (size_t i = 0; i < eqExprCount - 1; i++)
            {
                BinaryOperatorExpr *op = new BinaryOperatorExpr;
                op->op = BinaryOperatorExpr::Op::LogicalAnd;
                op->left = move(eqExprs[i]);
                op->right = move(eqExprs[i + 1]);
                eqExprs[i + 1].reset(op);
            }
            expr = move(eqExprs.back());
        }
    }

    return expr;
}

static BinaryOperatorExpr::Op tokenTypeToBinaryOpType(int tokenType)
{
    static const map<int, BinaryOperatorExpr::Op> MAP =
    {
        { Token::T_AND_AND,     BinaryOperatorExpr::Op::LogicalAnd },
        { Token::T_OR_OR,       BinaryOperatorExpr::Op::LogicalOr },
        { Token::T_EQUAL,       BinaryOperatorExpr::Op::Equal },
        { Token::T_NOT_EQUAL,   BinaryOperatorExpr::Op::NotEqual },
        { Token::T_LT,          BinaryOperatorExpr::Op::LessThan },
        { Token::T_GT,          BinaryOperatorExpr::Op::GreaterThan },
        { Token::T_LE,          BinaryOperatorExpr::Op::LessEqual },
        { Token::T_GE,          BinaryOperatorExpr::Op::GreaterEqual },
        { Token::T_PLUS,        BinaryOperatorExpr::Op::Plus },
        { Token::T_MINUS,       BinaryOperatorExpr::Op::Minus },
        { Token::T_STAR,        BinaryOperatorExpr::Op::Multiply },
        { Token::T_SLASH,       BinaryOperatorExpr::Op::Divide },
        { Token::T_REMAINDER,   BinaryOperatorExpr::Op::Remainder}
    };

    auto iter = MAP.find(tokenType);
    assert(iter != MAP.end());
    return iter->second;
}

std::unique_ptr<Expr> Parser::parseEqualityExpression()
{
    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> relExprs;
    vector<int> tokens;

    relExprs.push_back(parseRelationalExpression());
    while (curToken().isIn({Token::T_EQUAL, Token::T_NOT_EQUAL}))
    {
        match(curTokenType());
        tokens.push_back(token(m_index - 1).type);
        relExprs.push_back(parseRelationalExpression());
    }

    if (!trying())
    {
        const size_t relExprCount = relExprs.size();
        const size_t tokenCount = tokens.size();

        assert(relExprCount == tokenCount + 1);
        assert(relExprCount != 0);

        if (relExprCount == 1)
        {
            expr = move(relExprs[0]);
        }
        else
        {
            for (size_t i = 0; i < relExprCount - 1; i++)
            {
                BinaryOperatorExpr *op = new BinaryOperatorExpr;
                op->op = tokenTypeToBinaryOpType(tokens[i]);
                op->left = move(relExprs[i]);
                op->right = move(relExprs[i + 1]);
                relExprs[i + 1].reset(op);
            }
            expr = move(relExprs.back());
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseRelationalExpression()
{
    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> subExprs;
    vector<int> tokens;

    subExprs.push_back(parseAdditiveExpression());
    while (curToken().isIn({Token::T_LT, Token::T_GT, Token::T_LE, Token::T_GE}))
    {
        match(curTokenType());
        tokens.push_back(token(m_index - 1).type);
        subExprs.push_back(parseAdditiveExpression());
    }

    if (!trying())
    {
        const size_t subExprCount = subExprs.size();
        const size_t tokenCount = tokens.size();
        assert(subExprCount != 0);
        assert(subExprCount == tokenCount + 1);
        if (subExprCount == 1)
        {
            expr = move(subExprs[0]);
        }
        else
        {
            for (size_t i = 0; i < subExprCount - 1; i++)
            {
                BinaryOperatorExpr *op = new BinaryOperatorExpr;
                op->op = tokenTypeToBinaryOpType(tokens[i]);
                op->left = move(subExprs[i]);
                op->right = move(subExprs[i + 1]);
                subExprs[i + 1].reset(op);
            }
            expr = move(subExprs.back());
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseAdditiveExpression()
{
    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> subExprs;
    vector<int> tokens;

    subExprs.push_back(parseMultiplicativeExpression());
    while (curToken().isIn({Token::T_PLUS, Token::T_MINUS}))
    {
        match(curTokenType());
        tokens.push_back(token(m_index - 1).type);
        subExprs.push_back(parseMultiplicativeExpression());
    }

    if (!trying())
    {
        const size_t subExprCount = subExprs.size();
        const size_t tokenCount = tokens.size();
        assert(subExprCount != 0);
        assert(subExprCount == tokenCount + 1);
        if (subExprCount == 1)
        {
            expr = move(subExprs[0]);
        }
        else
        {
            for (size_t i = 0; i < subExprCount - 1; i++)
            {
                BinaryOperatorExpr *op = new BinaryOperatorExpr;
                op->op = tokenTypeToBinaryOpType(tokens[i]);
                op->left = move(subExprs[i]);
                op->right = move(subExprs[i + 1]);
                subExprs[i + 1].reset(op);
            }
            expr = move(subExprs.back());
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseMultiplicativeExpression()
{
    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> subExprs;
    vector<int> tokens;

    subExprs.push_back(parseUnaryExpression());
    while (curToken().isIn({Token::T_STAR, Token::T_SLASH, Token::T_REMAINDER}))
    {
        match(curTokenType());
        tokens.push_back(token(m_index - 1).type);
        subExprs.push_back(parseUnaryExpression());
    }

    if (!trying())
    {
        const size_t subExprCount = subExprs.size();
        const size_t tokenCount = tokens.size();
        assert(subExprCount != 0);
        assert(subExprCount == tokenCount + 1);
        if (subExprCount == 1)
        {
            expr = move(subExprs[0]);
        }
        else
        {
            for (size_t i = 0; i < subExprCount - 1; i++)
            {
                BinaryOperatorExpr *op = new BinaryOperatorExpr;
                op->op = tokenTypeToBinaryOpType(tokens[i]);
                op->left = move(subExprs[i]);
                op->right = move(subExprs[i + 1]);
                subExprs[i + 1].reset(op);
            }
            expr = move(subExprs.back());
        }
    }
    return expr;
}

static const set<int> postfixExpressionFirst =
{
    Token::T_IDENTIFIER,
    Token::T_STRING_LITERAL,
    Token::T_NUMBER_LITERAL,
    Token::T_L_PAREN
};

static const set<int> unaryOperatorFirst =
{
    Token::T_PLUS,
    Token::T_MINUS,
    Token::T_NOT
};

static UnaryOperatorExpr::Op tokenTypeToUnaryOpType(int tokenType)
{
    static const map<int, UnaryOperatorExpr::Op> MAP =
    {
        { Token::T_PLUS,    UnaryOperatorExpr::Op::Positive },
        { Token::T_MINUS,   UnaryOperatorExpr::Op::Negative },
        { Token::T_NOT,     UnaryOperatorExpr::Op::Not }
    };

    auto iter = MAP.find(tokenType);
    assert(iter != MAP.end());
    return iter->second;
}

std::unique_ptr<Expr> Parser::parseUnaryExpression()
{
    unique_ptr<Expr> expr;
    if (curToken().isIn(postfixExpressionFirst))
    {
        expr = parsePostfixExpression();
    }
    else if (curToken().isIn(unaryOperatorFirst))
    {
        UnaryOperatorExpr *op = new UnaryOperatorExpr;
        parseUnaryOperator();
        op->op = tokenTypeToUnaryOpType(token(m_index - 1).type);
        op->expr = parseUnaryExpression();
        expr.reset(op);
    }
    else
    {
        const char *msg = "Expect an unaryExpression";
        throw SyntaxError(msg, curToken());
    }

    return expr;
}

void Parser::parseUnaryOperator()
{
    switch (curTokenType())
    {
    case Token::T_PLUS:
    case Token::T_MINUS:
    case Token::T_NOT:
    {
        match(curTokenType());
        break;
    }
    default:
    {
        const char *msg = "Expect a '+' / '-' / '!'";
        throw SyntaxError(msg, curToken());
    }
    }
}

static const set<int> argumentExpressionListFirst = expressionFirst;
std::unique_ptr<Expr> Parser::parsePostfixExpression()
{
    enum PostfixType
    {
        Call,
        Subscript,
        Member
    };

    unique_ptr<Expr> expr;
    vector<unique_ptr<Expr>> subExprs;
    vector<int> types;

    Token tok = curToken();
    subExprs.push_back(parsePrimaryExpression());
    while (curToken().isIn({Token::T_L_BRACKET, Token::T_L_PAREN, Token::T_DOT}))
    {
        int type = curTokenType();
        if (type == Token::T_L_BRACKET)
        {
            match(Token::T_L_BRACKET);

            unique_ptr<ListSubscriptExpr> lse(new ListSubscriptExpr);
            lse->indexExpr = parseExpression();
            lse->tok = tok;

            subExprs.push_back(unique_ptr<Expr>(lse.release()));
            types.push_back(Subscript);

            match(Token::T_R_BRACKET);
        }
        else if (type == Token::T_L_PAREN)
        {
            match(Token::T_L_PAREN);
            unique_ptr<CallExpr> callExpr(new CallExpr);
            callExpr->tok = tok;
            if (curToken().isIn(argumentExpressionListFirst))
            {
                parseArgumentExpressionList(callExpr);
            }
            subExprs.push_back(unique_ptr<Expr>(callExpr.release()));
            types.push_back(Call);
            match(Token::T_R_PAREN);
        }
        else
        {
            match(Token::T_DOT);
            match(Token::T_IDENTIFIER);

            unique_ptr<MemberExpr> memberExpr(new MemberExpr);
            memberExpr->name = token(m_index - 1).str;
            memberExpr->tok = token(m_index - 1);

            subExprs.push_back(unique_ptr<Expr>(memberExpr.release()));
            types.push_back(Member);
        }
    }

    if (!trying())
    {
        const size_t subExprCount = subExprs.size();
        const size_t typeCount = types.size();
        assert(subExprCount != 0);
        assert(subExprCount == typeCount + 1);

        if (subExprCount == 1)
        {
            expr = move(subExprs[0]);
        }
        else
        {
            for (size_t i = 0; i < subExprCount - 1; i++)
            {
                Expr *p = subExprs[i + 1].get();
                if (types[i] == Call)
                {
                    dynamic_cast<CallExpr *>(p)->funcExpr
                            = move(subExprs[i]);
                }
                else if (types[i] == Subscript)
                {
                    dynamic_cast<ListSubscriptExpr *>(p)->listExpr
                            = move(subExprs[i]);
                }
                else if (types[i] == Member)
                {
                    dynamic_cast<MemberExpr *>(p)->instanceExpr
                            = move(subExprs[i]);
                }
                else
                {
                    assert(false);
                }
            }
            expr = move(subExprs.back());
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimaryExpression()
{
    unique_ptr<Expr> expr;

    unique_ptr<Expr> refExpr;
    unique_ptr<Expr> literalExpr;
    unique_ptr<Expr> parenExpr;

    switch (curTokenType())
    {
    case Token::T_IDENTIFIER:
    {
        match(Token::T_IDENTIFIER);
        Token tok = token(m_index - 1);
        string idName = token(m_index - 1).str;
        refExpr.reset(new RefExpr);
        refExpr->tok = tok;
        dynamic_cast<RefExpr *>(refExpr.get())->name = idName;
        break;
    }
    case Token::T_STRING_LITERAL:
    case Token::T_NUMBER_LITERAL:
    {
        literalExpr = parseLiteral();
        break;
    }
    case Token::T_L_PAREN:
    {
        match(Token::T_L_PAREN);
        parenExpr = parseExpression();
        match(Token::T_R_PAREN);
        break;
    }
    default:
    {
        const char *msg = "Expect a Identifier / literal / '('";
        throw SyntaxError(msg, curToken());
    }
    }

    if (!trying())
    {
        if (refExpr)
        {
            expr = move(refExpr);
        }
        else if (literalExpr)
        {
            expr = move(literalExpr);
        }
        else if (parenExpr)
        {
            expr = move(parenExpr);
        }
    }

    return expr;
}

void Parser::parseArgumentExpressionList(std::unique_ptr<CallExpr> &callExpr)
{
    vector<unique_ptr<Expr>> exprs;

    exprs.push_back(parseExpression());
    while (curToken().is(Token::T_COMMA))
    {
        match(Token::T_COMMA);
        exprs.push_back(parseExpression());
    }

    if (!trying())
    {
        for (size_t i = 0; i < exprs.size(); i++)
        {
            callExpr->paramList.push_back(move(exprs[i]));
        }
    }
}

static const set<int> assignStatementFirst =
{
    Token::T_IDENTIFIER,
    Token::T_STRING_LITERAL,
    Token::T_NUMBER_LITERAL,
    Token::T_L_PAREN
};

std::unique_ptr<Stmt> Parser::parseStatement()
{
    unique_ptr<Stmt> s;

    switch (curTokenType())
    {
    case Token::T_L_BRACE:  s = parseCompoundStatement();   break;
    case Token::T_IF:       s = parseSelectionStatement();  break;
    case Token::T_WHILE:    s = parseIterationStatement();  break;
    case Token::T_CONTINUE:
    case Token::T_BREAK:
    case Token::T_RETURN:   s = parseJumpStatement();       break;
    default:
    {
        if (curToken().isIn(assignStatementFirst))
        {
            s = parseExprStatement();
        }
        else
        {
            const char *msg = "Expect a statement";
            throw SyntaxError(msg, curToken());
        }
    }
    }

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        stmt = move(s);
    }

    return stmt;
}

std::unique_ptr<Stmt> Parser::parseSelectionStatement()
{
    unique_ptr<Expr> condition;
    unique_ptr<Stmt> thenStmt;
    unique_ptr<Stmt> elseStmt;

    match(Token::T_IF);
    match(Token::T_L_PAREN);
    condition = parseExpression();
    match(Token::T_R_PAREN);
    thenStmt = parseCompoundStatement();
    if (curToken().is(Token::T_ELSE))
    {
        match(Token::T_ELSE);
        if (curToken().is(Token::T_L_BRACE))
        {
            elseStmt = parseCompoundStatement();
        }
        else if (curToken().is(Token::T_IF))
        {
            elseStmt = parseSelectionStatement();
        }
        else
        {
            const char *msg = "Expect a 'if' / compoundStatement";
            throw SyntaxError(msg, curToken());
        }
    }

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        stmt.reset(new IfStmt(move(condition), move(thenStmt), move(elseStmt)));
    }

    return stmt;
}

std::unique_ptr<Stmt> Parser::parseIterationStatement()
{
    unique_ptr<Expr> condition;
    unique_ptr<Stmt> bodyStmt;

    match(Token::T_WHILE);
    match(Token::T_L_PAREN);
    condition = parseExpression();
    match(Token::T_R_PAREN);
    bodyStmt = parseCompoundStatement();

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        stmt.reset(new WhileStmt(move(condition), move(bodyStmt)));
    }

    return stmt;
}

std::unique_ptr<Stmt> Parser::parseJumpStatement()
{
    unique_ptr<Stmt> s;

    Token tok = curToken();
    switch (curTokenType())
    {
    case Token::T_CONTINUE:
    {
        match(Token::T_CONTINUE);
        match(Token::T_SEMICOLON);
        s.reset(new ContinueStmt);
        break;
    }
    case Token::T_BREAK:
    {
        match(Token::T_BREAK);
        match(Token::T_SEMICOLON);
        s.reset(new BreakStmt);
        break;
    }
    case Token::T_RETURN:
    {
        match(Token::T_RETURN);
        if (curToken().isIn(expressionFirst))
        {
            unique_ptr<Expr> expr = parseExpression();
            s.reset(new ReturnStmt(move(expr)));
        }
        else
        {
            s.reset(new ReturnStmt(unique_ptr<Expr>()));
        }
        match(Token::T_SEMICOLON);
        break;
    }
    default:
    {
        const char *msg = "Expect a 'continue' / 'break' / 'return'";
        throw SyntaxError(msg, curToken());
    }
    }

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        assert(s);
        stmt = move(s);
        stmt->tok = tok;
    }

    return stmt;
}

std::unique_ptr<Stmt> Parser::parseExprStatement()
{
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;

    left = parsePostfixExpression();
    if (curToken().is(Token::T_ASSIGN))
    {
        match(Token::T_ASSIGN);
        right = parseExpression();
    }
    match(Token::T_SEMICOLON);

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        unique_ptr<Expr> expr;
        if (right)
        {
            expr.reset(new BinaryOperatorExpr(BinaryOperatorExpr::Op::Assign,
                                              move(left), move(right)));
        }
        else
        {
            expr = move(left);
        }
        stmt.reset(new ExprStmt(move(expr)));
    }

    return stmt;
}

std::unique_ptr<EnumDecl> Parser::parseEnumDefination()
{
    unique_ptr<EnumDecl> enumDecl;
    string enumName;

    if (!trying())
    {
        enumDecl.reset(new EnumDecl);
    }

    match(Token::T_ENUM);
    match(Token::T_IDENTIFIER);
    Token tok = token(m_index - 1);
    enumName = token(m_index - 1).str;

    match(Token::T_L_BRACE);
    parseEnumConstantList(enumDecl);
    match(Token::T_R_BRACE);

    if (!trying())
    {
        enumDecl->name = enumName;
        enumDecl->tok = tok;
    }

    return enumDecl;
}

void Parser::parseEnumConstantList(std::unique_ptr<EnumDecl> &enumDecl)
{
    parseEnumConstant(enumDecl);
    while (curToken().is(Token::T_COMMA))
    {
        match(Token::T_COMMA);
        parseEnumConstant(enumDecl);
    }
}

void Parser::parseEnumConstant(std::unique_ptr<EnumDecl> &enumDecl)
{
    unique_ptr<EnumConstantDecl> ecd;
    string ecName;

    if (!trying())
    {
        ecd.reset(new EnumConstantDecl);
        ecd->tok = curToken();
    }

    match(Token::T_IDENTIFIER);
    ecName = token(m_index - 1).str;

    if (!trying())
    {
        ecd->name = ecName;
        enumDecl->constantList.push_back(move(ecd));
    }
}

std::unique_ptr<ComponentInstanceDecl> Parser::parseComponentInstance()
{
    unique_ptr<ComponentInstanceDecl> instanceDecl;
    string typeName;

    if (!trying())
    {
        instanceDecl.reset(new ComponentInstanceDecl);
        instanceDecl->tok = curToken();
    }

    match(Token::T_IDENTIFIER);
    typeName = token(m_index - 1).str;
    match(Token::T_L_BRACE);
    parseBindingItemList(instanceDecl);
    match(Token::T_R_BRACE);

    if (!trying())
    {
        instanceDecl->componentName = typeName;
    }
    return instanceDecl;
}

void Parser::parseBindingItemList(std::unique_ptr<ComponentInstanceDecl> &instanceDecl)
{
    parseBindingItem(instanceDecl);
    while (curToken().is(Token::T_IDENTIFIER))
    {
        parseBindingItem(instanceDecl);
    }
}

void Parser::parseBindingItem(std::unique_ptr<ComponentInstanceDecl> &instanceDecl)
{
    string name;
    unique_ptr<Expr> expr;
    unique_ptr<ComponentInstanceDecl> child;

    Token tok = curToken();

    match(Token::T_IDENTIFIER);
    name = token(m_index - 1).str;
    switch (curTokenType())
    {
    case Token::T_COLON:
    {
        match(Token::T_COLON);
        expr = parseInitializer();
        break;
    }
    case Token::T_L_BRACE:
    {
        m_index--;
        child = parseComponentInstance();
        break;
    }
    default:
    {
        const char *msg = "Expect a ':' / '{'";
        throw SyntaxError(msg, curToken());
    }
    }

    if (!trying())
    {
        if (child)
        {
            child->parent = instanceDecl.get();
            instanceDecl->childrenList.push_back(move(child));
        }
        else
        {
            assert(name != "");
            assert(expr);

            instanceDecl->bindingList.emplace_back(new BindingDecl(name, move(expr)));
            instanceDecl->bindingList.back()->componentInstance = instanceDecl.get();
            instanceDecl->bindingList.back()->tok = tok;
        }
    }
}

}
}
