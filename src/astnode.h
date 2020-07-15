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
#include <map>
#include <set>

class Scope;

struct ASTNode
{
    virtual ~ASTNode();

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

    int value;
};

struct FloatLiteral : public Expr
{
    explicit FloatLiteral(float f)
        : Expr(Category::Float)
        , value(f)
    {

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

    std::string value;
};

struct InitListExpr : public Expr
{
    explicit InitListExpr(std::vector<std::unique_ptr<Expr>> &&l)
        : Expr(Category::InitList)
        , exprList(move(l))
    {}

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

    Op op = Op::Invalid;
    std::unique_ptr<Expr> expr;
};

struct CallExpr : public Expr
{
    CallExpr() : Expr(Category::Call) {}

    std::unique_ptr<Expr> funcExpr;
    std::vector<std::unique_ptr<Expr>> paramList;
};

struct ListSubscriptExpr : public Expr
{
    ListSubscriptExpr() : Expr(Category::ListSubscript) {}

    std::unique_ptr<Expr> listExpr;
    std::unique_ptr<Expr> indexExpr;
};

struct MemberExpr : public Expr
{
    MemberExpr() : Expr(Category::Member) {}

    std::unique_ptr<Expr> instanceExpr;
    std::string name;
};

struct RefExpr : public Expr
{
    RefExpr() : Expr(Category::Ref) {}

    std::string name;
};

struct VarDecl : public ASTNode
{
    std::shared_ptr<TypeInfo> type;
    std::string name;
    std::unique_ptr<Expr> expr;

    int localIndex = -1;
};

struct ComponentDefinationDecl;

struct PropertyDecl : public ASTNode
{
    ~PropertyDecl() override;

    std::string name;
    std::shared_ptr<TypeInfo> type;
    std::unique_ptr<Expr> expr;

    int fieldIndex = -1;
    ComponentDefinationDecl *componentDefination = nullptr;
};

struct ParamDecl : public ASTNode
{
    ParamDecl(const std::string &n, const std::shared_ptr<TypeInfo> &t) : name(n), type(t) {}

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

    std::vector<std::unique_ptr<Stmt>> stmtList;
};

struct DeclStmt : public Stmt
{
    explicit DeclStmt(std::unique_ptr<VarDecl> &&vd) : Stmt(Category::Decl), decl(std::move(vd)) {}

    std::unique_ptr<VarDecl> decl;
};

struct IfStmt : public Stmt
{
    IfStmt(std::unique_ptr<Expr> &&cond,
           std::unique_ptr<Stmt> &&ts,
           std::unique_ptr<Stmt> &&es)
        : Stmt(Category::If), condition(move(cond)), thenStmt(move(ts)), elseStmt(move(es))
    {}

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenStmt;
    std::unique_ptr<Stmt> elseStmt;
};

struct WhileStmt : public Stmt
{
    WhileStmt(std::unique_ptr<Expr> &&cond, std::unique_ptr<Stmt> &&bs)
        : Stmt(Category::While), condition(move(cond)), bodyStmt(move(bs))
    {}

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> bodyStmt;
};

struct BreakStmt : public Stmt
{
    BreakStmt() : Stmt(Category::Break) {}
};

struct ContinueStmt : public Stmt
{
    ContinueStmt() : Stmt(Category::Continue) {}
};

struct ReturnStmt : public Stmt
{
    explicit ReturnStmt(std::unique_ptr<Expr> &&re) : Stmt(Category::Return), returnExpr(move(re)) {}

    std::unique_ptr<Expr> returnExpr;
};

struct ExprStmt : public Stmt
{
    explicit ExprStmt(std::unique_ptr<Expr> &&e) : Stmt(Category::Expr), expr(move(e)) {}

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

    std::string name;
    std::shared_ptr<TypeInfo> returnType;
    std::vector<std::unique_ptr<ParamDecl>> paramList;
    std::unique_ptr<CompoundStmt> body;
    ComponentDefinationDecl *component = nullptr;
    int locals = -1;
};

struct EnumConstantDecl : public ASTNode
{
    std::string name;
    int value = -1;
};

struct EnumDecl : public ASTNode
{
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

    Type type;
    std::string filename;
};

struct ComponentDefinationDecl : public DocumentDecl
{
    ComponentDefinationDecl() : DocumentDecl(DocumentDecl::Type::Defination) {}

    std::string name;
    std::vector<std::unique_ptr<PropertyDecl>> propertyList;
    std::vector<std::unique_ptr<FunctionDecl>> methodList;
    std::vector<std::unique_ptr<EnumDecl>> enumList;
    std::vector<int> propertyInitOrder;
    std::map<int, std::set<int>> propertyDeps;
};

struct FieldDecl : public ASTNode
{
    FieldDecl(const std::string &_name, const std::shared_ptr<TypeInfo> &_type)
        : type(_type), name(_name)
    {}

    std::shared_ptr<TypeInfo> type;
    std::string name;

    int fieldIndex = -1;
};

struct StructDecl : public DocumentDecl
{
    StructDecl() : DocumentDecl(DocumentDecl::Type::Struct) {}

    std::string name;
    std::vector<std::unique_ptr<FieldDecl>> fieldList;
};

struct ComponentInstanceDecl;

struct BindingDecl : public ASTNode
{
    BindingDecl(const std::string &n, std::unique_ptr<Expr> &&e)
        : name(n), expr(move(e))
    {}
    ~BindingDecl() override;

    bool isId() const { return name == "id"; }

    std::string bindingId() const;
    int fieldIndex() const;
    int instanceIndex() const;

    std::string name;
    std::unique_ptr<Expr> expr;

    PropertyDecl *propertyDecl = nullptr;
    ComponentInstanceDecl *componentInstance = nullptr;
};

struct ComponentInstanceDecl : public DocumentDecl
{
    ComponentInstanceDecl() : DocumentDecl(DocumentDecl::Type::Instance) {}

    std::vector<ComponentInstanceDecl *> instanceList();
    std::vector<int> unboundProperty() const;

    std::string componentName;
    std::vector<std::unique_ptr<BindingDecl>> bindingList;
    std::vector<std::unique_ptr<ComponentInstanceDecl>> childrenList;
    ComponentInstanceDecl *parent = nullptr;

    ComponentDefinationDecl *componentDefination = nullptr;
    int instanceIndex = -1;
    int instanceTreeSize = -1;
    std::string instanceId;

    std::vector<std::pair<ComponentInstanceDecl *, ASTNode *>> orderedMemberInitList;
};
