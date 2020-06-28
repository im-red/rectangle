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

#include "symboltable.h"
#include "option.h"
#include "util.h"
#include "symbol.h"
#include "typeinfo.h"

#include <assert.h>

using namespace std;

SymbolTable::SymbolTable()
{
    initGlobalScope();
}

SymbolTable::~SymbolTable()
{
    delete m_globalScope;
    m_globalScope = nullptr;

    clear();
}

void SymbolTable::clear()
{
    for (auto p : m_scopes)
    {
        delete p;
    }
    m_scopes.clear();
    m_curScope = m_globalScope;
}

Scope *SymbolTable::curScope() const
{
    return m_curScope;
}

void SymbolTable::pushScope(Scope *scope)
{
    if (dynamic_cast<Symbol *>(scope) == nullptr)
    {
        m_scopes.insert(scope);
    }

    m_curScope = scope;
    util::condPrint(option::showScopeStack, "pushScope: %p(%s)\n",
                     static_cast<void *>(m_curScope),
                     m_curScope->scopeString().c_str());
}

void SymbolTable::popScope()
{
    util::condPrint(option::showScopeStack, "popScope: %p(%s)\n",
                     static_cast<void *>(m_curScope),
                     m_curScope->scopeString().c_str());
    m_curScope = m_curScope->parent();
    assert(m_curScope != nullptr);
}

void SymbolTable::define(Symbol *symbol)
{
    if (m_curScope == nullptr)
    {
        throw SymbolException("def", "cur scope is nullptr");
    }
    m_curScope->define(symbol);
}

void SymbolTable::initGlobalScope()
{
    m_globalScope = new Scope(Scope::Category::Global, nullptr);
    m_curScope = m_globalScope;

    shared_ptr<TypeInfo> voidType = make_shared<TypeInfo>(TypeInfo::Category::Void);

    {
        shared_ptr<TypeInfo> rectType(new CustomTypeInfo("rect"));
        vector<shared_ptr<TypeInfo>> paramTypes(1, rectType);
        Symbol *drawRect = new FunctionSymbol("drawRect", voidType, paramTypes);
        define(drawRect);
    }

    {
        shared_ptr<TypeInfo> ptType(new CustomTypeInfo("pt"));
        vector<shared_ptr<TypeInfo>> paramTypes(1, ptType);
        Symbol *drawPt = new FunctionSymbol("drawPt", voidType, paramTypes);
        define(drawPt);
    }

    {
        shared_ptr<TypeInfo> textType(new CustomTypeInfo("text"));
        vector<shared_ptr<TypeInfo>> paramTypes(1, textType);
        Symbol *drawText = new FunctionSymbol("drawText", voidType, paramTypes);
        define(drawText);
    }

    {
        vector<shared_ptr<TypeInfo>> paramTypes(1, voidType);
        Symbol *len = new FunctionSymbol("len", make_shared<TypeInfo>(TypeInfo::Category::Int), paramTypes);
        define(len);
        Symbol *print = new FunctionSymbol("print", voidType, paramTypes);
        define(print);
    }
}
