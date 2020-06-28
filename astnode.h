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
#include "typeinfo.h"

#include <string>
#include <memory>
#include <vector>

class Scope;

struct ASTNode
{
    virtual ~ASTNode();
    virtual void print(int indent = 0) const
    {
        std::string space(static_cast<size_t>(indent), ' ');
        util::condPrint(option::showAst, "%s", space.c_str());
        doPrint(indent);
    }
    virtual void doPrint(int) const
    {

    }

    Scope *scope = nullptr;
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
        util::condPrint(option::showAst, "IntegerLiteral(%d)\n", value);
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
        util::condPrint(option::showAst, "FloatLiteral(%f)\n", value);
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
        util::condPrint(option::showAst, "StringLiteral(%s)\n", value.c_str());
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
        util::condPrint(option::showAst, "InitListExpr\n");
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
        Invalid,
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
        util::condPrint(option::showAst, "BinaryOperatorExpr(%s)\n", typeString(op).c_str());
        left->print(indent + 1);
        right->print(indent + 1);
    }

    Op op = Op::Invalid;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct UnaryOperatorExpr : public Expr
{
    enum Op
    {
        Invalid,
        Positive,
        Negative,
        Not
    };

    UnaryOperatorExpr() : Expr(Category::UnaryOperator) {}
    static std::string typeString(Op op);
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "UnaryOperatorExpr(%s)\n", typeString(op).c_str());
        expr->print(indent + 1);
    }

    Op op = Op::Invalid;
    std::unique_ptr<Expr> expr;
};

struct CallExpr : public Expr
{
    CallExpr() : Expr(Category::Call) {}
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "CallExpr\n");
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
        util::condPrint(option::showAst, "ListSubscriptExpr\n");
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
        util::condPrint(option::showAst, "MemberExpr(%s)\n", name.c_str());
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
        util::condPrint(option::showAst, "RefExpr(%s)\n", name.c_str());
    }

    std::string name;
};

struct VarDecl : public ASTNode
{
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "VarDecl(%s %s)\n", type->toString().c_str(), name.c_str());
        if (expr)
        {
            expr->print(indent);
        }
    }

    std::shared_ptr<TypeInfo> type;
    std::string name;
    std::unique_ptr<Expr> expr;

    int localIndex = -1;
};

struct PropertyDecl : public ASTNode
{
    ~PropertyDecl() override;
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "PropertyDecl(%s %s)\n", type->toString().c_str(), name.c_str());
        if (expr)
        {
            expr->print(indent + 1);
        }
    }

    std::string name;
    std::shared_ptr<TypeInfo> type;
    std::unique_ptr<Expr> expr;

    int fieldIndex = -1;
};

struct GroupedPropertyDecl : public PropertyDecl
{
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "GroupedPropertyDecl(%s %s.%s)\n", type->toString().c_str(), groupName.c_str(), name.c_str());
        expr->print(indent + 1);
    }
    std::string groupName;
};

struct ParamDecl : public ASTNode
{
    ParamDecl(const std::string &n, const std::shared_ptr<TypeInfo> &t) : name(n), type(t) {}
    void doPrint(int) const override
    {
        util::condPrint(option::showAst, "ParamDecl(%s %s)\n", type->toString().c_str(), name.c_str());
    }

    std::string name;
    std::shared_ptr<TypeInfo> type;

    int localIndex = -1;
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

    explicit Stmt(Category cat) : category(cat) {}
    virtual ~Stmt();

    Category category;
};

struct CompoundStmt : public Stmt
{
    explicit CompoundStmt(std::vector<std::unique_ptr<Stmt>> &&sl) : Stmt(Category::Compound), stmtList(move(sl)) {}
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "CompoundStmt\n");
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
        util::condPrint(option::showAst, "DeclStmt\n");
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
        util::condPrint(option::showAst, "IfStmt\n");
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
        util::condPrint(option::showAst, "WhileStmt\n");
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
        util::condPrint(option::showAst, "BreakStmt\n");
    }
};

struct ContinueStmt : public Stmt
{
    ContinueStmt() : Stmt(Category::Continue) {}
    void doPrint(int) const override
    {
        util::condPrint(option::showAst, "ContinueStmt\n");
    }
};

struct ReturnStmt : public Stmt
{
    explicit ReturnStmt(std::unique_ptr<Expr> &&re) : Stmt(Category::Return), returnExpr(move(re)) {}
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "ReturnStmt\n");
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
        util::condPrint(option::showAst, "ExprStmt\n");
        expr->print(indent + 1);
    }

    std::unique_ptr<Expr> expr;
};

struct ComponentDefinationDecl;

struct FunctionDecl : public ASTNode
{
    FunctionDecl(const std::string &n,
                 const std::shared_ptr<TypeInfo> &rt,
                 std::vector<std::unique_ptr<ParamDecl>> &&pl,
                 std::unique_ptr<CompoundStmt> &&b)
        : name(n), returnType(move(rt)), paramList(move(pl)), body(move(b))
    {}
    void doPrint(int indent) const override;

    std::string name;
    std::shared_ptr<TypeInfo> returnType;
    std::vector<std::unique_ptr<ParamDecl>> paramList;
    std::unique_ptr<CompoundStmt> body;
    ComponentDefinationDecl *component = nullptr;
    int locals = -1;
};

struct EnumConstantDecl : public ASTNode
{
    void doPrint(int) const override
    {
        util::condPrint(option::showAst, "EnumConstantDecl(%s)\n", name.c_str());
    }

    std::string name;
    int value = -1;
};

struct EnumDecl : public ASTNode
{
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "EnumConstantDecl(%s)\n", name.c_str());
        for (auto &c : constantList)
        {
            c->print(indent + 1);
        }
    }

    std::string name;
    std::vector<std::unique_ptr<EnumConstantDecl>> constantList;
};

struct DocumentDecl : public ASTNode
{
    enum class Type
    {
        Defination,
        Instance,
        Struct
    };

    explicit DocumentDecl(Type type_) : type(type_) {}
    void dump() const
    {
        util::condPrint(option::showAst, "---------- DocumentDecl::dump begin ----------\n");
        print(0);
        util::condPrint(option::showAst, "----------- DocumentDecl::dump end -----------\n");
    }

    Type type;
};

struct ComponentDefinationDecl : public DocumentDecl
{
    ComponentDefinationDecl() : DocumentDecl(DocumentDecl::Type::Defination) {}

    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "ComponentDefinationDecl(%s)\n", name.c_str());
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
    std::vector<int> propertyInitOrder;
};

struct FieldDecl : public ASTNode
{
    FieldDecl(const std::string &_name, const std::shared_ptr<TypeInfo> &_type)
        : type(_type), name(_name)
    {}
    void doPrint(int) const override
    {
        util::condPrint(option::showAst, "FieldDecl(%s %s)\n", type->toString().c_str(), name.c_str());
    }

    std::shared_ptr<TypeInfo> type;
    std::string name;

    int fieldIndex = -1;
};

struct StructDecl : public DocumentDecl
{
    StructDecl() : DocumentDecl(DocumentDecl::Type::Struct) {}
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "StructDecl(%s)\n", name.c_str());
        for (auto &p : fieldList)
        {
            p->print(indent + 1);
        }
    }

    std::string name;
    std::vector<std::unique_ptr<FieldDecl>> fieldList;
};

struct BindingDecl : public ASTNode
{
    BindingDecl(const std::string &n, std::unique_ptr<Expr> &&e)
        : name(n), expr(move(e))
    {}
    ~BindingDecl() override;
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "BindingDecl(%s)\n", name.c_str());
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
        util::condPrint(option::showAst, "GroupedBindingDecl(%s.%s)\n",groupName.c_str(), name.c_str());
        expr->print(indent + 1);
    }

    std::string groupName;
};

struct ComponentInstanceDecl : public DocumentDecl
{
    ComponentInstanceDecl() : DocumentDecl(DocumentDecl::Type::Instance) {}
    void doPrint(int indent) const override
    {
        util::condPrint(option::showAst, "ComponentInstanceDecl(%s)\n",componentName.c_str());
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
