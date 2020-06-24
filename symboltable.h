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

#include "symbol.h"

#include <set>

class SymbolTable
{
public:
    SymbolTable();
    ~SymbolTable();

    Scope *curScope() const;
    void pushScope(Scope *scope);
    void popScope();

    void define(Symbol *symbol);

private:
    std::set<Scope *> m_scopes;

    Scope *m_curScope = nullptr;
};

