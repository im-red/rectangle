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

#include "symbol.h"
#include "option.h"
#include "util.h"
#include "astnode.h"
#include "typeinfo.h"

#include <utility>

#include <assert.h>

using namespace std;

namespace rectangle
{
namespace backend
{

Symbol::Symbol(Category cat, const string &n, std::shared_ptr<TypeInfo> ti, ASTNode *ast)
    : m_category(cat)
    , m_name(n)
    , m_typeInfo(ti)
    , m_astNode(ast)
{
    util::condPrint(option::showSymbolDef, "def: %s\n", symbolString().c_str());
}

Symbol::~Symbol()
{

}

string Symbol::symbolString() const
{
    char buf[512];
    if (m_category == Category::Property)
    {
        assert(m_astNode != nullptr);
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(m_astNode);
        assert(pd != nullptr);
    }
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

Symbol::Category Symbol::category() const
{
    return m_category;
}

std::string Symbol::name() const
{
    return m_name;
}

std::shared_ptr<TypeInfo> Symbol::typeInfo() const
{
    return m_typeInfo;
}

void Symbol::setTypeInfo(const std::shared_ptr<TypeInfo> &typeInfo)
{
    m_typeInfo = typeInfo;
}

ASTNode *Symbol::astNode() const
{
    return m_astNode;
}

void Symbol::setAstNode(ASTNode *astNode)
{
    m_astNode = astNode;
}

int Scope::m_nextScopeId = 0;

Scope::Scope(Category cat, Scope *p)
    : m_parent(p)
    , m_category(cat)
    , m_scopeId(m_nextScopeId++)
{

}

Scope::~Scope()
{
    for (auto &pair : m_symbols)
    {
        delete pair.second;
    }
}

Symbol *Scope::resolve(const string &name)
{
    Symbol *result = nullptr;
    auto iter = m_symbols.find(name);
    if (iter == m_symbols.end())
    {
        if (m_parent)
        {
            result = m_parent->resolve(name);
        }
    }
    else
    {
        result = iter->second;
    }
    if (result == nullptr && m_componentScope != nullptr)
    {
        result = m_componentScope->resolve(name);
    }
    return result;
}

void Scope::define(Symbol *sym)
{
    m_symbols[sym->name()] = sym;
}

std::string Scope::scopeName() const
{
    return m_scopeName;
}

void Scope::setScopeName(const std::string &scopeName)
{
    m_scopeName = scopeName;
}

int Scope::scopeId() const
{
    return m_scopeId;
}

void Scope::setScopeId(int scopeId)
{
    m_scopeId = scopeId;
}

Scope *Scope::componentScope() const
{
    return m_componentScope;
}

void Scope::setComponentScope(Scope *componentScope)
{
    m_componentScope = componentScope;
}

ScopeSymbol::ScopeSymbol(Symbol::Category symCat, const string &name, Scope::Category scopeCat, Scope *parent, const std::shared_ptr<TypeInfo> &ti)
    : Symbol(symCat, name, ti)
    , Scope(scopeCat, parent)
{
    setScopeName(name);
}

}
}
