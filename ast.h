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

#include <string>
#include <memory>
#include <vector>

struct Printable
{
    virtual ~Printable();
    virtual void print(int indent) const
    {
        std::string space(indent, ' ');
        printf("%s", space.c_str());
        doPrint(indent);
    }
    virtual void doPrint(int) const
    {

    }
};

struct Expr : public Printable
{
    virtual ~Expr();
};

struct IntegerLiteral : public Expr
{
    explicit IntegerLiteral(int i) : value(i) {}
    void doPrint(int) const override
    {
        printf("IntegerLiteral(%d)\n", value);
    }

    int value;
};

struct FloatLiteral : public Expr
{
    explicit FloatLiteral(float f) : value(f) {}
    void doPrint(int) const override
    {
        printf("FloatLiteral(%f)\n", value);
    }

    float value;
};

struct StringLiteral : public Expr
{
    explicit StringLiteral(const std::string &s) : value(s) {}
    void doPrint(int) const override
    {
        printf("StringLiteral(%s)\n", value.c_str());
    }

    std::string value;
};

struct InitListExpr : public Expr
{
    explicit InitListExpr(std::vector<std::unique_ptr<Expr>> &&l) : exprList(move(l)) {}
    void doPrint(int indent) const override
    {
        printf("InitListExpr\n");
        for (auto &e : exprList)
        {
            e->print(indent + 1);
        }
    }

    std::vector<std::unique_ptr<Expr>> exprList;
};

struct BinaryOperatorExpr : public Expr
{
    enum class Type
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

    BinaryOperatorExpr() {}
    BinaryOperatorExpr(Type t, std::unique_ptr<Expr> &&l, std::unique_ptr<Expr> &&r)
        : type(t), left(move(l)), right(move(r))
    {}
    static std::string typeString(Type type);
    void doPrint(int indent) const override
    {
        printf("BinaryOperatorExpr(%s)\n", typeString(type).c_str());
        left->print(indent + 1);
        right->print(indent + 1);
    }

    Type type;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct UnaryOperatorExpr : public Expr
{
    enum Type
    {
        Positive,
        Negative,
        Not
    };

    static std::string typeString(Type type);
    void doPrint(int indent) const override
    {
        printf("UnaryOperatorExpr(%s)\n", typeString(type).c_str());
        expr->print(indent + 1);
    }

    Type type;
    std::unique_ptr<Expr> expr;
};

struct CallExpr : public Expr
{
    void doPrint(int indent) const override
    {
        printf("CallExpr\n");
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
    void doPrint(int indent) const override
    {
        printf("ListSubscriptExpr\n");
        listExpr->print(indent + 1);
        indexExpr->print(indent + 1);
    }
    std::unique_ptr<Expr> listExpr;
    std::unique_ptr<Expr> indexExpr;
};

struct MemberExpr : public Expr
{
    void doPrint(int indent) const override
    {
        printf("MemberExpr(%s)\n", name.c_str());
        instanceExpr->print(indent + 1);
    }

    std::unique_ptr<Expr> instanceExpr;
    std::string name;
};

struct RefExpr : public Expr
{
    void doPrint(int) const override
    {
        printf("RefExpr(%s)\n", name.c_str());
    }

    std::string name;
};

struct TypeInfo
{
    std::string name;
};

struct VarDecl : public Printable
{
    void doPrint(int indent) const override
    {
        printf("VarDecl(%s %s)\n", type.name.c_str(), name.c_str());
        if (expr)
        {
            expr->print(indent);
        }
    }

    std::string name;
    TypeInfo type;
    std::unique_ptr<Expr> expr;
};

struct PropertyDecl : public Printable
{
    ~PropertyDecl() override;
    void doPrint(int indent) const override
    {
        printf("PropertyDecl(%s %s)\n", type.name.c_str(), name.c_str());
        if (expr)
        {
            expr->print(indent + 1);
        }
    }

    std::string name;
    TypeInfo type;
    std::unique_ptr<Expr> expr;
};

struct ScopedPropertyDecl : public PropertyDecl
{
    void doPrint(int indent) const override
    {
        printf("ScopedPropertyDecl(%s %s.%s)\n", type.name.c_str(), scopeName.c_str(), name.c_str());
        expr->print(indent + 1);
    }
    std::string scopeName;
};

struct ParamDecl : public Printable
{
    ParamDecl(const std::string &n, const TypeInfo &t) : name(n), type(t) {}
    void doPrint(int) const override
    {
        printf("ParamDecl(%s %s)\n", type.name.c_str(), name.c_str());
    }

    std::string name;
    TypeInfo type;
};

struct Stmt : public Printable
{
    virtual ~Stmt();
};

struct CompoundStmt : public Stmt
{
    explicit CompoundStmt(std::vector<std::unique_ptr<Stmt>> &&sl) : stmtList(move(sl)) {}
    void doPrint(int indent) const override
    {
        printf("CompoundStmt\n");
        for (auto &s : stmtList)
        {
            s->print(indent + 1);
        }
    }

    std::vector<std::unique_ptr<Stmt>> stmtList;
};

struct DeclStmt : public Stmt
{
    explicit DeclStmt(std::unique_ptr<VarDecl> &&vd) : decl(std::move(vd)) {}
    void doPrint(int indent) const override
    {
        printf("DeclStmt\n");
        decl->print(indent + 1);
    }

    std::unique_ptr<VarDecl> decl;
};

struct IfStmt : public Stmt
{
    IfStmt(std::unique_ptr<Expr> &&cond,
           std::unique_ptr<Stmt> &&ts,
           std::unique_ptr<Stmt> &&es)
        : condition(move(cond)), thenStmt(move(ts)), elseStmt(move(es))
    {}
    void doPrint(int indent) const override
    {
        printf("IfStmt\n");
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
        : condition(move(cond)), bodyStmt(move(bs))
    {}
    void doPrint(int indent) const override
    {
        printf("WhileStmt\n");
        condition->print(indent + 1);
        bodyStmt->print(indent + 1);
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> bodyStmt;
};

struct BreakStmt : public Stmt
{
    void doPrint(int) const override
    {
        printf("BreakStmt\n");
    }
};

struct ContinueStmt : public Stmt
{
    void doPrint(int) const override
    {
        printf("ContinueStmt\n");
    }
};

struct ReturnStmt : public Stmt
{
    explicit ReturnStmt(std::unique_ptr<Expr> &&re) : returnExpr(move(re)) {}
    void doPrint(int indent) const override
    {
        printf("ReturnStmt\n");
        if (returnExpr)
        {
            returnExpr->print(indent + 1);
        }
    }

    std::unique_ptr<Expr> returnExpr;
};

struct ExprStmt : public Stmt
{
    explicit ExprStmt(std::unique_ptr<Expr> &&e) : expr(move(e)) {}
    void doPrint(int indent) const override
    {
        printf("ExprStmt\n");
        expr->print(indent + 1);
    }

    std::unique_ptr<Expr> expr;
};

struct FunctionDecl : public Printable
{
    FunctionDecl(const std::string &n,
                 const TypeInfo &rt,
                 std::vector<std::unique_ptr<ParamDecl>> &&pl,
                 std::unique_ptr<CompoundStmt> &&b)
        : name(n), returnType(rt), paramList(move(pl)), body(move(b))
    {}
    void doPrint(int indent) const override
    {
        printf("FunctionDecl(%s %s)\n", returnType.name.c_str(), name.c_str());
        for (auto &p : paramList)
        {
            p->print(indent + 1);
        }
        body->print(indent + 1);
    }

    std::string name;
    TypeInfo returnType;
    std::vector<std::unique_ptr<ParamDecl>> paramList;
    std::unique_ptr<CompoundStmt> body;
};

struct EnumConstantDecl : public Printable
{
    void doPrint(int) const override
    {
        printf("EnumConstantDecl(%s)\n", name.c_str());
    }

    std::string name;
};

struct EnumDecl : public Printable
{
    void doPrint(int indent) const override
    {
        printf("EnumConstantDecl(%s)\n", name.c_str());
        for (auto &c : constantList)
        {
            c->print(indent + 1);
        }
    }

    std::string name;
    std::vector<std::unique_ptr<EnumConstantDecl>> constantList;
};

struct ComponentDefinationDecl : public Printable
{
    void doPrint(int indent) const override
    {
        printf("ComponentDefinationDecl(%s)\n", name.c_str());
        for (auto &p : propertyList)
        {
            p->print(indent + 1);
        }
        for (auto &p : functionList)
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
    std::vector<std::unique_ptr<FunctionDecl>> functionList;
    std::vector<std::unique_ptr<EnumDecl>> enumList;
};

struct BindingDecl : public Printable
{
    BindingDecl(const std::string &n, std::unique_ptr<Expr> &&e)
        : name(n), expr(move(e))
    {}
    ~BindingDecl();
    void doPrint(int indent) const override
    {
        printf("BindingDecl(%s)\n", name.c_str());
        expr->print(indent + 1);
    }

    std::string name;
    std::unique_ptr<Expr> expr;
};

struct ScopedBindingDecl : public BindingDecl
{
    ScopedBindingDecl(const std::string &sn, const std::string &n, std::unique_ptr<Expr> &&e)
        : BindingDecl(n, move(e)), scopeName(sn)
    {}
    void doPrint(int indent) const override
    {
        printf("ScopedBindingDecl(%s.%s)\n",scopeName.c_str(), name.c_str());
        expr->print(indent + 1);
    }

    std::string scopeName;
};

struct ComponentInstanceDecl : public Printable
{
    void doPrint(int indent) const override
    {
        printf("ComponentInstanceDecl(%s)\n",typeName.c_str());
        for (auto &p : bindingList)
        {
            p->print(indent + 1);
        }
        for (auto &p : instanceList)
        {
            p->print(indent + 1);
        }
    }

    std::string typeName;
    std::vector<std::unique_ptr<BindingDecl>> bindingList;
    std::vector<std::unique_ptr<ComponentInstanceDecl>> instanceList;
};

struct DocumentDecl : public Printable
{
    void doPrint(int indent) const override
    {
        printf("DocumentDecl\n");
        if (type == DocumentType::ComponentDefination)
        {
            defination->print(indent + 1);
        }
        else
        {
            instance->print(indent + 1);
        }
    }

    enum class DocumentType
    {
        ComponentDefination,
        ComponentInstance
    };

    DocumentType type;
    std::unique_ptr<ComponentDefinationDecl> defination;
    std::unique_ptr<ComponentInstanceDecl> instance;
};
