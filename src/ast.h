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

#include "astnode.h"
#include "symboltable.h"

#include <vector>
#include <memory>

namespace rectangle
{

class AST
{
public:
    AST();
    ~AST();

    void clear();
    void addDocument(std::unique_ptr<DocumentDecl> &&document);

    std::vector<DocumentDecl *> documents() const;
    backend::SymbolTable *symbolTable();

private:
    void initBuiltinDocuments();

private:
    std::vector<std::unique_ptr<DocumentDecl>> m_documents;
    std::vector<std::unique_ptr<DocumentDecl>> m_builtinDocuments;
    std::unique_ptr<backend::SymbolTable> m_symbolTable = nullptr;
};

}
