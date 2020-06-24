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

SymbolTable::SymbolTable()
{

}

SymbolTable::~SymbolTable()
{
    for (auto p : m_scopes)
    {
        delete p;
    }
    m_scopes.clear();
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
}

void SymbolTable::define(Symbol *symbol)
{
    if (m_curScope == nullptr)
    {
        throw SymbolException("def", "cur scope is nullptr");
    }
    m_curScope->define(symbol);
}
