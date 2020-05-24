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

#include <string>
#include <map>

class Scope;

class Symbol
{
public:
    enum class Category
    {
        Invalid,
        Variable,
        Parameter,
        Struct,
        Member,
        Component,
        PropertyGroup,
        Property,
        Method,
        InstanceId,
        Enum,
        EnumConstants,
        Function,
        BuiltInType
    };

public:
    static std::string symbolCategoryString(Category m_category);

    Symbol(Category cat, const std::string &n, std::shared_ptr<TypeInfo> ti = std::shared_ptr<TypeInfo>());
    virtual ~Symbol();

    std::string symbolString() const
    {
        constexpr int BUF_LEN = 200;
        char buf[BUF_LEN];
        if (m_typeInfo)
        {
            snprintf(buf, sizeof(buf), "%s <%s:%s>",
                     symbolCategoryString(m_category).c_str(),
                     m_name.c_str(),
                     m_typeInfo->toString().c_str());
        }
        else
        {
            snprintf(buf, sizeof(buf), "%s <%s>",
                     symbolCategoryString(m_category).c_str(),
                     m_name.c_str());
        }
        return std::string(buf);
    }

    Category category() const;
    std::string name() const;
    std::shared_ptr<TypeInfo> typeInfo() const;

private:
    Category m_category = Category::Invalid;
    std::string m_name;
    std::shared_ptr<TypeInfo> m_typeInfo;
};

class MethodSymbol : public Symbol
{
public:
    MethodSymbol(const std::string &name,
                 const std::shared_ptr<TypeInfo> &ti,
                 const std::shared_ptr<Symbol> &componentSymbol,
                 const std::vector<std::shared_ptr<TypeInfo>> &paramTypes)
        : Symbol(Symbol::Category::Method, name, ti)
        , m_componentSymbol(componentSymbol)
        , m_paramTypes(paramTypes)
    {}

    std::shared_ptr<Symbol> componentSymbol() const { return m_componentSymbol; }
    std::vector<std::shared_ptr<TypeInfo>> paramTypes() const { return m_paramTypes; }

private:
    std::shared_ptr<Symbol> m_componentSymbol;
    std::vector<std::shared_ptr<TypeInfo>> m_paramTypes;
};

class FunctionSymbol : public Symbol
{
public:
    FunctionSymbol(const std::string &name,
                   const std::shared_ptr<TypeInfo> &ti,
                   const std::vector<std::shared_ptr<TypeInfo>> &paramTypes)
        : Symbol(Symbol::Category::Function, name, ti)
        , m_paramTypes(paramTypes)
    {}

    std::vector<std::shared_ptr<TypeInfo>> paramTypes() const { return m_paramTypes; }

private:
    std::vector<std::shared_ptr<TypeInfo>> m_paramTypes;
};

class Scope
{
public:
    enum class Category
    {
        Invalid,
        Global,
        Local,
        Function,
        Component,
        Method,
        Group,
        Struct,
        Instance
    };

public:
    static std::string scopeCategoryString(Category category);

    Scope(Category cat, std::shared_ptr<Scope> p);
    virtual ~Scope();

    std::shared_ptr<Scope> parent() const { return m_parent; }
    Category category() const { return m_category; }

    std::shared_ptr<Symbol> resolve(const std::string &name);

    void define(const std::shared_ptr<Symbol> &sym);

private:
    std::map<std::string, std::shared_ptr<Symbol>> m_symbols;
    std::shared_ptr<Scope> m_parent = nullptr;
    Category m_category = Category::Invalid;
};

class ScopeSymbol : public Symbol, public Scope
{
public:
    ScopeSymbol(Symbol::Category symCat, const std::string &name, Scope::Category scopeCat, std::shared_ptr<Scope> parent, const std::shared_ptr<TypeInfo> &ti = std::shared_ptr<TypeInfo>());
};

class SymbolVisitor
{
public:
    SymbolVisitor();

    void setDocuments(std::vector<DocumentDecl *> documents);
    void visit();

private:
    std::shared_ptr<Scope> curScope();
    void pushScope(std::shared_ptr<Scope> scope);
    void popScope();

    void save(std::shared_ptr<Symbol> &symbol);
    void save(std::shared_ptr<Scope> &scope);

    void initGlobalScope();

private:
    void visit(DocumentDecl *dd);
    void visit(ComponentDefinationDecl *cdd);
    void visit(ComponentInstanceDecl *cid);
    void visit(Expr *e);
    void visit(InitListExpr *ile);
    void visit(BinaryOperatorExpr *b);
    void visit(UnaryOperatorExpr *u);
    void visit(CallExpr *c);
    void visit(ListSubscriptExpr *lse);
    void visit(MemberExpr *me);
    void visit(RefExpr *re);
    void visit(VarDecl *vd);
    void visit(PropertyDecl *pd);
    void visit(GroupedPropertyDecl *gpd);
    void visit(ParamDecl *pd);
    void visit(Stmt *s);
    void visit(CompoundStmt *cs);
    void visit(DeclStmt *ds);
    void visit(IfStmt *is);
    void visit(WhileStmt *ws);
    void visit(ReturnStmt *rs);
    void visit(ExprStmt *es);
    void visitHeader(FunctionDecl *fd);
    void visitBody(FunctionDecl *fd);
    void visit(EnumConstantDecl *ecd);
    void visit(EnumDecl *ed);
    void visit(BindingDecl *bd);
    void visit(GroupedBindingDecl *gbd);

private:
    std::vector<DocumentDecl *> m_documents;
    std::vector<std::shared_ptr<Scope>> m_scopes;
    std::vector<std::shared_ptr<Symbol>> m_symbols;
    std::shared_ptr<Scope> m_curScope = nullptr;
};
