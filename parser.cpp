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

#include <assert.h>

using namespace std;

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

    try
    {
        m_document = parseDocument();
    }
    catch (ParseException &e)
    {
        m_document.reset();
        fprintf(stderr, "%s\n", e.what());
    }

    return move(m_document);
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
        char buf[512];
        snprintf(buf, sizeof(buf), "expect %s but actual %s at line %d column %d(%s)",
                 Lexer::tokenTypeString(tokenType).c_str(),
                 Lexer::tokenTypeString(curTokenType()).c_str(),
                 curToken().line, curToken().column, curToken().str.c_str());
        throw ParseException(buf);
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
    case Lexer::T_DEF:
    {
        def = parseComponentDefination();
        break;
    }
    case Lexer::T_IDENTIFIER:
    {
        instance = parseComponentInstance();
        break;
    }
    default:
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect 'def'/Identifier at line %d column %d",
                 tok.line, tok.column);
        throw ParseException(buf);
    }
    }
    match(Lexer::T_EOF);

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

    match(Lexer::T_DEF);
    match(Lexer::T_IDENTIFIER);
    typeName = token(m_index - 1).str;

    match(Lexer::T_L_BRACE);
    parseMemberItemList(defination);
    match(Lexer::T_R_BRACE);

    if (!trying())
    {
        defination->name = typeName;
    }

    return defination;
}

static const set<int> memberItemFirst =
{
    Lexer::T_INT,
    Lexer::T_VOID,
    Lexer::T_FLOAT,
    Lexer::T_STRING,
    Lexer::T_LIST,
    Lexer::T_ENUM,
    Lexer::T_IDENTIFIER
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
    Lexer::T_INT,
    Lexer::T_FLOAT,
    Lexer::T_STRING,
    Lexer::T_LIST
};
static const set<int> propertyDefinationFirst = propertyTypeFirst;

static const set<int> typeFirst =
{
    Lexer::T_INT,
    Lexer::T_FLOAT,
    Lexer::T_STRING,
    Lexer::T_LIST,
    Lexer::T_VOID,
    Lexer::T_IDENTIFIER
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

    if (curToken().is(Lexer::T_ENUM))
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
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "(MemberItem)expect enumDefination/propertyDefination/functionDefination at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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
    catch (ParseException &e)
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
    catch (ParseException &e)
    {
        util::condPrint(option::showLLTry, "tryMemberItemAlt2 fail: %s\n", e.what());
        result = false;
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
    match(Lexer::T_IDENTIFIER);
    name = token(m_index - 1).str;

    match(Lexer::T_COLON);
    initExpr = parseInitializer();

    unique_ptr<PropertyDecl> propertyDecl;

    if (!trying())
    {
        propertyDecl.reset(new PropertyDecl);
        propertyDecl->name = name;
        propertyDecl->type = move(ti);
        propertyDecl->expr = move(initExpr);
    }

    return propertyDecl;
}

std::shared_ptr<TypeInfo> Parser::parsePropertyType()
{
    shared_ptr<TypeInfo> result;

    switch(curTokenType())
    {
    case Lexer::T_INT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Int));
        match(curTokenType());
        break;
    }
    case Lexer::T_FLOAT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Float));
        match(curTokenType());
        break;
    }
    case Lexer::T_STRING:
    {
        result.reset(new TypeInfo(TypeInfo::Category::String));
        match(curTokenType());
        break;
    }
    case Lexer::T_LIST:
    {
        result = parseListType();
        break;
    }
    default:
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a type token at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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
    case Lexer::T_INT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Int));
        match(curTokenType());
        break;
    }
    case Lexer::T_VOID:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Void));
        match(curTokenType());
        break;
    }
    case Lexer::T_FLOAT:
    {
        result.reset(new TypeInfo(TypeInfo::Category::Float));
        match(curTokenType());
        break;
    }
    case Lexer::T_STRING:
    {
        result.reset(new TypeInfo(TypeInfo::Category::String));
        match(curTokenType());
        break;
    }
    case Lexer::T_IDENTIFIER:
    {
        result.reset(new CustomTypeInfo(curToken().str));
        match(curTokenType());
        break;
    }
    case Lexer::T_LIST:
    {
        result = parseListType();
        break;
    }
    default:
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a type token at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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
    match(Lexer::T_LIST);
    match(Lexer::T_LT);
    shared_ptr<TypeInfo> ele = parsePropertyType();
    match(Lexer::T_GT);

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
    case Lexer::T_STRING_LITERAL:
    {
        match(curTokenType());
        string s = token(m_index - 1).str;
        stringExpr.reset(new StringLiteral(s));
        break;
    }
    case Lexer::T_NUMBER_LITERAL:
    {
        match(curTokenType());
        string s = token(m_index - 1).str;
        if (s.find('.') == string::npos)
        {
            int i = stoi(s);
            intExpr.reset(new IntegerLiteral(i));
        }
        else
        {
            float f = stof(s);
            floatExpr.reset(new FloatLiteral(f));
        }
        break;
    }
    default:
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a literal token at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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
    match(Lexer::T_IDENTIFIER);
    name = token(m_index - 1).str;
    match(Lexer::T_L_PAREN);
    if (curToken().isIn(paramListFirst))
    {
        parseParamList(paramList);
    }
    match(Lexer::T_R_PAREN);
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
    }
    return decl;
}

void Parser::parseParamList(std::vector<std::unique_ptr<ParamDecl>> &paramList)
{
    vector<unique_ptr<ParamDecl>> pl;

    pl.push_back(parseParamItem());
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
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
    match(Lexer::T_IDENTIFIER);
    name = token(m_index - 1).str;

    unique_ptr<ParamDecl> decl;
    if (!trying())
    {
        decl.reset(new ParamDecl(name, move(ti)));
    }

    return decl;
}

std::unique_ptr<Stmt> Parser::parseCompoundStatement()
{
    vector<unique_ptr<Stmt>> stmts;

    match(Lexer::T_L_BRACE);
    parseBlockItemList(stmts);
    match(Lexer::T_R_BRACE);

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        stmt.reset(new CompoundStmt(move(stmts)));
    }

    return stmt;
}

static const set<int> blockItemFirst =
{
    Lexer::T_INT,
    Lexer::T_VOID,
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
    if (tryBlockItemAlt1())
    {
        stmt = parseDeclaration();
    }
    else if (tryBlockItemAlt2())
    {
        stmt = parseStatement();
    }
    else
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf),
                 "expect a delcaration/statement for blockItem at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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
    catch (ParseException &e)
    {
        util::condPrint(option::showLLTry, "tryBlockItemAlt1 fail: %s\n", e.what());
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
    catch (ParseException &e)
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

    decl->type = parseType();
    match(Lexer::T_IDENTIFIER);
    decl->name = token(m_index - 1).str;
    if (curToken().is(Lexer::T_ASSIGN))
    {
        match(Lexer::T_ASSIGN);
        decl->expr = parseInitializer();
    }
    match(Lexer::T_SEMICOLON);

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        stmt.reset(new DeclStmt(move(decl)));
    }

    return stmt;
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

std::unique_ptr<Expr> Parser::parseInitializer()
{
    unique_ptr<Expr> expr;

    if (curToken().isIn(expressionFirst))
    {
        expr = parseExpression();
    }
    else if (curToken().is(Lexer::T_L_BRACE))
    {
        match(Lexer::T_L_BRACE);
        if (curToken().isIn(initializerListFirst))
        {
            expr = parseInitializerList();
        }
        else
        {
            expr.reset(new InitListExpr({}));
        }
        match(Lexer::T_R_BRACE);
    }
    else
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a initializer at line %d column %d",
                 tok.line, tok.column);
        throw ParseException(buf);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseInitializerList()
{
    vector<unique_ptr<Expr>> exprs;

    exprs.push_back(parseInitializer());
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
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
    while (curToken().is(Lexer::T_OR_OR))
    {
        match(Lexer::T_OR_OR);
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
    while (curToken().is(Lexer::T_AND_AND))
    {
        match(Lexer::T_AND_AND);
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
        { Lexer::T_AND_AND,     BinaryOperatorExpr::Op::LogicalAnd },
        { Lexer::T_OR_OR,       BinaryOperatorExpr::Op::LogicalOr },
        { Lexer::T_EQUAL,       BinaryOperatorExpr::Op::Equal },
        { Lexer::T_NOT_EQUAL,   BinaryOperatorExpr::Op::NotEqual },
        { Lexer::T_LT,          BinaryOperatorExpr::Op::LessThan },
        { Lexer::T_GT,          BinaryOperatorExpr::Op::GreaterThan },
        { Lexer::T_LE,          BinaryOperatorExpr::Op::LessEqual },
        { Lexer::T_GE,          BinaryOperatorExpr::Op::GreaterEqual },
        { Lexer::T_PLUS,        BinaryOperatorExpr::Op::Plus },
        { Lexer::T_MINUS,       BinaryOperatorExpr::Op::Minus },
        { Lexer::T_STAR,        BinaryOperatorExpr::Op::Multiply },
        { Lexer::T_SLASH,       BinaryOperatorExpr::Op::Divide },
        { Lexer::T_REMAINDER,   BinaryOperatorExpr::Op::Remainder}
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
    while (curToken().isIn({Lexer::T_EQUAL, Lexer::T_NOT_EQUAL}))
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
    while (curToken().isIn({Lexer::T_LT, Lexer::T_GT, Lexer::T_LE, Lexer::T_GE}))
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
    while (curToken().isIn({Lexer::T_PLUS, Lexer::T_MINUS}))
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
    while (curToken().isIn({Lexer::T_STAR, Lexer::T_SLASH, Lexer::T_REMAINDER}))
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

static UnaryOperatorExpr::Op tokenTypeToUnaryOpType(int tokenType)
{
    static const map<int, UnaryOperatorExpr::Op> MAP =
    {
        { Lexer::T_PLUS,    UnaryOperatorExpr::Op::Positive },
        { Lexer::T_MINUS,   UnaryOperatorExpr::Op::Negative },
        { Lexer::T_NOT,     UnaryOperatorExpr::Op::Not }
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
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a unaryExpression at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }

    return expr;
}

void Parser::parseUnaryOperator()
{
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
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect '+'/'-'/'!' at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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

    subExprs.push_back(parsePrimaryExpression());
    while (curToken().isIn({Lexer::T_L_BRACKET, Lexer::T_L_PAREN, Lexer::T_DOT}))
    {
        int type = curTokenType();
        if (type == Lexer::T_L_BRACKET)
        {
            match(Lexer::T_L_BRACKET);

            unique_ptr<ListSubscriptExpr> lse(new ListSubscriptExpr);
            lse->indexExpr = parseExpression();

            subExprs.push_back(unique_ptr<Expr>(lse.release()));
            types.push_back(Subscript);

            match(Lexer::T_R_BRACKET);
        }
        else if (type == Lexer::T_L_PAREN)
        {
            match(Lexer::T_L_PAREN);
            unique_ptr<CallExpr> callExpr(new CallExpr);
            if (curToken().isIn(argumentExpressionListFirst))
            {
                parseArgumentExpressionList(callExpr);
            }
            subExprs.push_back(unique_ptr<Expr>(callExpr.release()));
            types.push_back(Call);
            match(Lexer::T_R_PAREN);
        }
        else
        {
            match(Lexer::T_DOT);
            match(Lexer::T_IDENTIFIER);

            unique_ptr<MemberExpr> memberExpr(new MemberExpr);
            memberExpr->name = token(m_index - 1).str;

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
    case Lexer::T_IDENTIFIER:
    {
        match(Lexer::T_IDENTIFIER);
        string idName = token(m_index - 1).str;
        refExpr.reset(new RefExpr);
        dynamic_cast<RefExpr *>(refExpr.get())->name = idName;
        break;
    }
    case Lexer::T_STRING_LITERAL:
    case Lexer::T_NUMBER_LITERAL:
    {
        literalExpr = parseLiteral();
        break;
    }
    case Lexer::T_L_PAREN:
    {
        match(Lexer::T_L_PAREN);
        parenExpr = parseExpression();
        match(Lexer::T_R_PAREN);
        break;
    }
    default:
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect Identifier/literal/'(' at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
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
    Lexer::T_IDENTIFIER,
    Lexer::T_STRING_LITERAL,
    Lexer::T_NUMBER_LITERAL,
    Lexer::T_L_PAREN
};

std::unique_ptr<Stmt> Parser::parseStatement()
{
    unique_ptr<Stmt> s;

    switch (curTokenType())
    {
    case Lexer::T_L_BRACE:  s = parseCompoundStatement();   break;
    case Lexer::T_IF:       s = parseSelectionStatement();  break;
    case Lexer::T_WHILE:    s = parseIterationStatement();  break;
    case Lexer::T_CONTINUE:
    case Lexer::T_BREAK:
    case Lexer::T_RETURN:   s = parseJumpStatement();       break;
    default:
    {
        if (curToken().isIn(assignStatementFirst))
        {
            s = parseExprStatement();
        }
        else
        {
            char buf[512];
            Token tok = curToken();
            snprintf(buf, sizeof(buf), "expect a statement at line %d column %d(%s)",
                     tok.line, tok.column, tok.str.c_str());
            throw ParseException(buf);
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

    match(Lexer::T_IF);
    match(Lexer::T_L_PAREN);
    condition = parseExpression();
    match(Lexer::T_R_PAREN);
    thenStmt = parseCompoundStatement();
    if (curToken().is(Lexer::T_ELSE))
    {
        match(Lexer::T_ELSE);
        if (curToken().is(Lexer::T_L_BRACE))
        {
            elseStmt = parseCompoundStatement();
        }
        else if (curToken().is(Lexer::T_IF))
        {
            elseStmt = parseSelectionStatement();
        }
        else
        {
            char buf[512];
            Token tok = curToken();
            snprintf(buf, sizeof(buf), "expect a 'if'/compoundStatement at line %d column %d(%s)",
                     tok.line, tok.column, tok.str.c_str());
            throw ParseException(buf);
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

    match(Lexer::T_WHILE);
    match(Lexer::T_L_PAREN);
    condition = parseExpression();
    match(Lexer::T_R_PAREN);
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

    switch (curTokenType())
    {
    case Lexer::T_CONTINUE:
    {
        match(Lexer::T_CONTINUE);
        match(Lexer::T_SEMICOLON);
        s.reset(new ContinueStmt);
        break;
    }
    case Lexer::T_BREAK:
    {
        match(Lexer::T_BREAK);
        match(Lexer::T_SEMICOLON);
        s.reset(new BreakStmt);
        break;
    }
    case Lexer::T_RETURN:
    {
        match(Lexer::T_RETURN);
        if (curToken().isIn(expressionFirst))
        {
            unique_ptr<Expr> expr = parseExpression();
            s.reset(new ReturnStmt(move(expr)));
        }
        else
        {
            s.reset(new ReturnStmt(unique_ptr<Expr>()));
        }
        match(Lexer::T_SEMICOLON);
        break;
    }
    default:
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a 'continue'/'break'/'return' at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
    }
    }

    unique_ptr<Stmt> stmt;
    if (!trying())
    {
        assert(s);
        stmt = move(s);
    }

    return stmt;
}

std::unique_ptr<Stmt> Parser::parseExprStatement()
{
    unique_ptr<Expr> left;
    unique_ptr<Expr> right;

    left = parsePostfixExpression();
    if (curToken().is(Lexer::T_ASSIGN))
    {
        match(Lexer::T_ASSIGN);
        right = parseExpression();
    }
    match(Lexer::T_SEMICOLON);

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

    match(Lexer::T_ENUM);
    match(Lexer::T_IDENTIFIER);
    enumName = token(m_index - 1).str;

    match(Lexer::T_L_BRACE);
    parseEnumConstantList(enumDecl);
    match(Lexer::T_R_BRACE);

    if (!trying())
    {
        enumDecl->name = enumName;
    }

    return enumDecl;
}

void Parser::parseEnumConstantList(std::unique_ptr<EnumDecl> &enumDecl)
{
    parseEnumConstant(enumDecl);
    while (curToken().is(Lexer::T_COMMA))
    {
        match(Lexer::T_COMMA);
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
    }

    match(Lexer::T_IDENTIFIER);
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
    }

    match(Lexer::T_IDENTIFIER);
    typeName = token(m_index - 1).str;
    match(Lexer::T_L_BRACE);
    parseBindingItemList(instanceDecl);
    match(Lexer::T_R_BRACE);

    if (!trying())
    {
        instanceDecl->componentName = typeName;
    }
    return instanceDecl;
}

void Parser::parseBindingItemList(std::unique_ptr<ComponentInstanceDecl> &instanceDecl)
{
    parseBindingItem(instanceDecl);
    while (curToken().is(Lexer::T_IDENTIFIER))
    {
        parseBindingItem(instanceDecl);
    }
}

void Parser::parseBindingItem(std::unique_ptr<ComponentInstanceDecl> &instanceDecl)
{
    string name;
    unique_ptr<Expr> expr;
    unique_ptr<ComponentInstanceDecl> child;

    match(Lexer::T_IDENTIFIER);
    name = token(m_index - 1).str;
    switch (curTokenType())
    {
    case Lexer::T_COLON:
    {
        match(Lexer::T_COLON);
        expr = parseInitializer();
        break;
    }
    case Lexer::T_L_BRACE:
    {
        m_index--;
        child = parseComponentInstance();
        break;
    }
    default:
    {
        char buf[512];
        Token tok = curToken();
        snprintf(buf, sizeof(buf), "expect a ':'/'{' after Identifier at line %d column %d(%s)",
                 tok.line, tok.column, tok.str.c_str());
        throw ParseException(buf);
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
        }
    }
}
