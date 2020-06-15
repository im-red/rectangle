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

class SymbolException : public std::runtime_error
{
public:
    explicit SymbolException(const std::string &s)
        : std::runtime_error(s)
    {}
    SymbolException(const std::string &ast, const std::string &s)
        : SymbolException("visit " + ast + " exception: " + s)
    {}
};

class Symbol
{
public:
    enum class Category
    {
        Invalid,
        Variable,
        Parameter,
        Struct,
        Field,
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

    Symbol(Category cat, const std::string &n, std::shared_ptr<TypeInfo> ti = std::shared_ptr<TypeInfo>(), ASTNode *ast = nullptr);
    virtual ~Symbol();

    std::string symbolString() const;

    Category category() const;
    std::string name() const;
    std::shared_ptr<TypeInfo> typeInfo() const;
    ASTNode *astNode() const;
    void setAstNode(ASTNode *astNode);

private:
    Category m_category = Category::Invalid;
    std::string m_name;
    std::shared_ptr<TypeInfo> m_typeInfo;
    ASTNode *m_astNode = nullptr;
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
                   const std::vector<std::shared_ptr<TypeInfo>> &paramTypes = std::vector<std::shared_ptr<TypeInfo>>())
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

    virtual std::string scopeString() const
    {
        constexpr int BUF_LEN = 200;
        char buf[BUF_LEN];
        snprintf(buf, sizeof(buf), "[%d] %s (%s)",
                 m_scopeId,
                 scopeCategoryString(m_category).c_str(),
                 m_scopeName.c_str());
        return std::string(buf);
    }

    std::shared_ptr<Scope> parent() const { return m_parent; }
    Category category() const { return m_category; }

    std::shared_ptr<Symbol> resolve(const std::string &name);

    void define(const std::shared_ptr<Symbol> &sym);

    std::string scopeName() const;
    void setScopeName(const std::string &scopeName);

    int scopeId() const;
    void setScopeId(int scopeId);

    static void resetNextScopeId() { m_nextScopeId = 0; }

private:
    std::map<std::string, std::shared_ptr<Symbol>> m_symbols;
    std::shared_ptr<Scope> m_parent = nullptr;
    Category m_category = Category::Invalid;
    std::string m_scopeName = "anonymous";
    int m_scopeId = -1;
    static int m_nextScopeId;
};

class ScopeSymbol : public Symbol, public Scope
{
public:
    ScopeSymbol(Symbol::Category symCat, const std::string &name,
                Scope::Category scopeCat, std::shared_ptr<Scope> parent,
                const std::shared_ptr<TypeInfo> &ti = std::shared_ptr<TypeInfo>());
};
