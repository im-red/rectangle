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

#include "ast.h"

#include "builtinstruct.h"

using namespace std;
using namespace rectangle::backend;

namespace rectangle {

AST::AST() {
  m_symbolTable.reset(new SymbolTable);
  initBuiltinDocuments();
}

AST::~AST() {}

void AST::clear() {
  m_documents.clear();
  m_symbolTable->clear();
}

void AST::addDocument(std::unique_ptr<DocumentDecl> &&document) {
  m_documents.push_back(move(document));
}

std::vector<DocumentDecl *> AST::documents() const {
  std::vector<DocumentDecl *> result;
  for (auto &doc : m_documents) {
    result.push_back(doc.get());
  }
  for (auto &doc : m_builtinDocuments) {
    result.push_back(doc.get());
  }
  return result;
}

SymbolTable *AST::symbolTable() { return m_symbolTable.get(); }

void AST::initBuiltinDocuments() {
  for (auto pInfo : builtin::infoList) {
    unique_ptr<StructDecl> sd(new StructDecl);
    sd->name = pInfo->name();
    sd->filepath = "(builtin)";
    int fieldCount = pInfo->fieldCount();
    for (int i = 0; i < fieldCount; i++) {
      builtin::FieldInfo field = pInfo->fieldAt(i);
      sd->fieldList.emplace_back(new FieldDecl(field.name, field.type));
    }
    m_builtinDocuments.push_back(move(sd));
  }
}

}  // namespace rectangle
