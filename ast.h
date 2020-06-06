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

#include "util.h"
#include "option.h"

#include <string>
#include <memory>
#include <vector>

struct ASTNode
{
    virtual ~ASTNode();
    virtual void print(int indent) const
    {
        std::string space(indent, ' ');
        util::condOutput(option::showAst, "%s", space.c_str());
        doPrint(indent);
    }
    virtual void doPrint(int) const
    {

    }
};

class TypeInfo
{
public:
    enum class Category
    {
        Int,
        Void,
        Point,
        Float,
        String,
        List,
        Group,
        Custom,
    };

public:
    explicit TypeInfo(Category cat);
    virtual ~TypeInfo();

    Category category() const;
    bool operator==(const TypeInfo &rhs) const
    {
        return m_category == rhs.m_category && toString() == rhs.toString();
    }
    bool operator!=(const TypeInfo &rhs) const
    {
        return !operator==(rhs);
    }

    virtual std::string toString() const;
    virtual bool assignCompatible(const std::shared_ptr<TypeInfo> &rhs) const;

private:
    Category m_category;
};

class ListTypeInfo : public TypeInfo
{
public:
    explicit ListTypeInfo(const std::shared_ptr<TypeInfo> &ele);
    std::shared_ptr<TypeInfo> elementType() const { return m_elementType; }

    std::string toString() const override;
    bool assignCompatible(const std::shared_ptr<TypeInfo> &rhs) const override;

private:
    std::shared_ptr<TypeInfo> m_elementType;
};

class CustomTypeInfo : public TypeInfo
{
public:
    explicit CustomTypeInfo(const std::string &name);
    std::string name() const { return m_name; }

    std::string toString() const override;

private:
    std::string m_name;
};

class GroupTypeInfo : public TypeInfo
{
public:
    GroupTypeInfo(const std::string &name, const std::shared_ptr<TypeInfo> &component);
    std::string name() const { return m_name; }
    std::shared_ptr<TypeInfo> componentType() const { return m_componentType; }

    std::string toString() const override;

private:
    std::string m_name;
    std::shared_ptr<TypeInfo> m_componentType;
};

struct Expr : public ASTNode
{
    enum class Category
    {
        Invalid,
        Integer,
        Float,
        String,
        InitList,
        BinaryOperator,
        UnaryOperator,
        Call,
        ListSubscript,
        Member,
        Ref
    };

    Expr(Category cat = Category::Invalid) : category(cat) {}
    virtual ~Expr();

    Category category = Category::Invalid;
    std::shared_ptr<TypeInfo> typeInfo;
};

struct IntegerLiteral : public Expr
{
    explicit IntegerLiteral(int i)
        : Expr(Category::Integer)
        , value(i)
    {

    }
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "IntegerLiteral(%d)\n", value);
    }

    int value;
};

struct FloatLiteral : public Expr
{
    explicit FloatLiteral(float f)
        : Expr(Category::Float)
        , value(f)
    {

    }
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "FloatLiteral(%f)\n", value);
    }

    float value;
};

struct StringLiteral : public Expr
{
    explicit StringLiteral(const std::string &s)
        : Expr(Category::String)
        , value(s)
    {

    }
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "StringLiteral(%s)\n", value.c_str());
    }

    std::string value;
};

struct InitListExpr : public Expr
{
    explicit InitListExpr(std::vector<std::unique_ptr<Expr>> &&l)
        : Expr(Category::InitList)
        , exprList(move(l))
    {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "InitListExpr\n");
        for (auto &e : exprList)
        {
            e->print(indent + 1);
        }
    }

    std::vector<std::unique_ptr<Expr>> exprList;
};

struct BinaryOperatorExpr : public Expr
{
    enum class Op
    {
        LogicalAnd,
        LogicalOr,
        Equal,
        NotEqual,
        LessThan,
        GreaterThan,
        LessEqual,
        GreaterEqual,
        Plus,
        Minus,
        Multiply,
        Divide,
        Remainder,
        Assign
    };

    BinaryOperatorExpr() : Expr(Category::BinaryOperator) {}
    BinaryOperatorExpr(Op t, std::unique_ptr<Expr> &&l, std::unique_ptr<Expr> &&r)
        : Expr(Category::BinaryOperator), op(t), left(move(l)), right(move(r))
    {}
    static std::string typeString(Op op);
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "BinaryOperatorExpr(%s)\n", typeString(op).c_str());
        left->print(indent + 1);
        right->print(indent + 1);
    }

    Op op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct UnaryOperatorExpr : public Expr
{
    enum Op
    {
        Positive,
        Negative,
        Not
    };

    UnaryOperatorExpr() : Expr(Category::UnaryOperator) {}
    static std::string typeString(Op op);
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "UnaryOperatorExpr(%s)\n", typeString(op).c_str());
        expr->print(indent + 1);
    }

    Op op;
    std::unique_ptr<Expr> expr;
};

struct CallExpr : public Expr
{
    CallExpr() : Expr(Category::Call) {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "CallExpr\n");
        funcExpr->print(indent + 1);
        for (auto &e : paramList)
        {
            e->print(indent + 1);
        }
    }

    std::unique_ptr<Expr> funcExpr;
    std::vector<std::unique_ptr<Expr>> paramList;
};

struct ListSubscriptExpr : public Expr
{
    ListSubscriptExpr() : Expr(Category::ListSubscript) {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "ListSubscriptExpr\n");
        listExpr->print(indent + 1);
        indexExpr->print(indent + 1);
    }
    std::unique_ptr<Expr> listExpr;
    std::unique_ptr<Expr> indexExpr;
};

struct MemberExpr : public Expr
{
    MemberExpr() : Expr(Category::Member) {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "MemberExpr(%s)\n", name.c_str());
        instanceExpr->print(indent + 1);
    }

    std::unique_ptr<Expr> instanceExpr;
    std::string name;
};

struct RefExpr : public Expr
{
    RefExpr() : Expr(Category::Ref) {}
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "RefExpr(%s)\n", name.c_str());
    }

    std::string name;
};

struct VarDecl : public ASTNode
{
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "VarDecl(%s %s)\n", type->toString().c_str(), name.c_str());
        if (expr)
        {
            expr->print(indent);
        }
    }

    std::shared_ptr<TypeInfo> type;
    std::string name;
    std::unique_ptr<Expr> expr;
};

struct PropertyDecl : public ASTNode
{
    ~PropertyDecl() override;
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "PropertyDecl(%s %s)\n", type->toString().c_str(), name.c_str());
        if (expr)
        {
            expr->print(indent + 1);
        }
    }

    std::string name;
    std::shared_ptr<TypeInfo> type;
    std::unique_ptr<Expr> expr;

    int index = -1;
    int initSequence = -1;
    std::vector<int> out;
    std::vector<int> in;
};

struct GroupedPropertyDecl : public PropertyDecl
{
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "GroupedPropertyDecl(%s %s.%s)\n", type->toString().c_str(), groupName.c_str(), name.c_str());
        expr->print(indent + 1);
    }
    std::string groupName;
};

struct ParamDecl : public ASTNode
{
    ParamDecl(const std::string &n, const std::shared_ptr<TypeInfo> &t) : name(n), type(t) {}
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "ParamDecl(%s %s)\n", type->toString().c_str(), name.c_str());
    }

    std::string name;
    std::shared_ptr<TypeInfo> type;
};

struct Stmt : public ASTNode
{
public:
    enum class Category
    {
        Invalid,
        Compound,
        Decl,
        If,
        While,
        Break,
        Continue,
        Return,
        Expr
    };

    Stmt(Category cat) : category(cat) {}
    virtual ~Stmt();

    Category category;
};

struct CompoundStmt : public Stmt
{
    explicit CompoundStmt(std::vector<std::unique_ptr<Stmt>> &&sl) : Stmt(Category::Compound), stmtList(move(sl)) {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "CompoundStmt\n");
        for (auto &s : stmtList)
        {
            s->print(indent + 1);
        }
    }

    std::vector<std::unique_ptr<Stmt>> stmtList;
};

struct DeclStmt : public Stmt
{
    explicit DeclStmt(std::unique_ptr<VarDecl> &&vd) : Stmt(Category::Decl), decl(std::move(vd)) {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "DeclStmt\n");
        decl->print(indent + 1);
    }

    std::unique_ptr<VarDecl> decl;
};

struct IfStmt : public Stmt
{
    IfStmt(std::unique_ptr<Expr> &&cond,
           std::unique_ptr<Stmt> &&ts,
           std::unique_ptr<Stmt> &&es)
        : Stmt(Category::If), condition(move(cond)), thenStmt(move(ts)), elseStmt(move(es))
    {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "IfStmt\n");
        condition->print(indent + 1);
        thenStmt->print(indent + 1);
        if (elseStmt)
        {
            elseStmt->print(indent + 1);
        }
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenStmt;
    std::unique_ptr<Stmt> elseStmt;
};

struct WhileStmt : public Stmt
{
    WhileStmt(std::unique_ptr<Expr> &&cond, std::unique_ptr<Stmt> &&bs)
        : Stmt(Category::While), condition(move(cond)), bodyStmt(move(bs))
    {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "WhileStmt\n");
        condition->print(indent + 1);
        bodyStmt->print(indent + 1);
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> bodyStmt;
};

struct BreakStmt : public Stmt
{
    BreakStmt() : Stmt(Category::Break) {}
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "BreakStmt\n");
    }
};

struct ContinueStmt : public Stmt
{
    ContinueStmt() : Stmt(Category::Continue) {}
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "ContinueStmt\n");
    }
};

struct ReturnStmt : public Stmt
{
    explicit ReturnStmt(std::unique_ptr<Expr> &&re) : Stmt(Category::Return), returnExpr(move(re)) {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "ReturnStmt\n");
        if (returnExpr)
        {
            returnExpr->print(indent + 1);
        }
    }

    std::unique_ptr<Expr> returnExpr;
};

struct ExprStmt : public Stmt
{
    explicit ExprStmt(std::unique_ptr<Expr> &&e) : Stmt(Category::Expr), expr(move(e)) {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "ExprStmt\n");
        expr->print(indent + 1);
    }

    std::unique_ptr<Expr> expr;
};

struct FunctionDecl : public ASTNode
{
    FunctionDecl(const std::string &n,
                 const std::shared_ptr<TypeInfo> &rt,
                 std::vector<std::unique_ptr<ParamDecl>> &&pl,
                 std::unique_ptr<CompoundStmt> &&b)
        : name(n), returnType(move(rt)), paramList(move(pl)), body(move(b))
    {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "FunctionDecl(%s %s)\n", returnType->toString().c_str(), name.c_str());
        for (auto &p : paramList)
        {
            p->print(indent + 1);
        }
        body->print(indent + 1);
    }

    std::string name;
    std::shared_ptr<TypeInfo> returnType;
    std::vector<std::unique_ptr<ParamDecl>> paramList;
    std::unique_ptr<CompoundStmt> body;
};

struct EnumConstantDecl : public ASTNode
{
    void doPrint(int) const override
    {
        util::condOutput(option::showAst, "EnumConstantDecl(%s)\n", name.c_str());
    }

    std::string name;
    int value = -1;
};

struct EnumDecl : public ASTNode
{
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "EnumConstantDecl(%s)\n", name.c_str());
        for (auto &c : constantList)
        {
            c->print(indent + 1);
        }
    }

    std::string name;
    std::vector<std::unique_ptr<EnumConstantDecl>> constantList;
};

struct ComponentDefinationDecl : public ASTNode
{
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "ComponentDefinationDecl(%s)\n", name.c_str());
        for (auto &p : propertyList)
        {
            p->print(indent + 1);
        }
        for (auto &p : methodList)
        {
            p->print(indent + 1);
        }
        for (auto &p : enumList)
        {
            p->print(indent + 1);
        }
    }

    std::string name;
    std::vector<std::unique_ptr<PropertyDecl>> propertyList;
    std::vector<std::unique_ptr<FunctionDecl>> methodList;
    std::vector<std::unique_ptr<EnumDecl>> enumList;
};

struct BindingDecl : public ASTNode
{
    BindingDecl(const std::string &n, std::unique_ptr<Expr> &&e)
        : name(n), expr(move(e))
    {}
    ~BindingDecl();
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "BindingDecl(%s)\n", name.c_str());
        expr->print(indent + 1);
    }

    std::string name;
    std::unique_ptr<Expr> expr;
};

struct GroupedBindingDecl : public BindingDecl
{
    GroupedBindingDecl(const std::string &sn, const std::string &n, std::unique_ptr<Expr> &&e)
        : BindingDecl(n, move(e)), groupName(sn)
    {}
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "GroupedBindingDecl(%s.%s)\n",groupName.c_str(), name.c_str());
        expr->print(indent + 1);
    }

    std::string groupName;
};

struct ComponentInstanceDecl : public ASTNode
{
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "ComponentInstanceDecl(%s)\n",componentName.c_str());
        for (auto &p : bindingList)
        {
            p->print(indent + 1);
        }
        for (auto &p : instanceList)
        {
            p->print(indent + 1);
        }
    }

    std::string componentName;
    std::vector<std::unique_ptr<BindingDecl>> bindingList;
    std::vector<std::unique_ptr<ComponentInstanceDecl>> instanceList;
};

struct DocumentDecl : public ASTNode
{
    void doPrint(int indent) const override
    {
        util::condOutput(option::showAst, "DocumentDecl\n");
        if (type == Type::Defination)
        {
            defination->print(indent + 1);
        }
        else
        {
            instance->print(indent + 1);
        }
    }

    enum class Type
    {
        Defination,
        Instance
    };

    Type type;
    std::unique_ptr<ComponentDefinationDecl> defination;
    std::unique_ptr<ComponentInstanceDecl> instance;
};
