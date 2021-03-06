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

#include <assert.h>

#include "builtinstruct.h"
#include "option.h"
#include "symbol.h"
#include "typeinfo.h"
#include "util.h"

using namespace std;

namespace rectangle {
namespace backend {

SymbolTable::SymbolTable() { initGlobalScope(); }

SymbolTable::~SymbolTable() {
  delete m_globalScope;
  m_globalScope = nullptr;

  clear();
}

void SymbolTable::clear() {
  for (auto p : m_scopes) {
    delete p;
  }
  m_scopes.clear();
  m_curScope = m_globalScope;
}

Scope *SymbolTable::curScope() const { return m_curScope; }

void SymbolTable::pushScope(Scope *scope) {
  if (dynamic_cast<Symbol *>(scope) == nullptr) {
    m_scopes.insert(scope);
  }

  m_curScope = scope;
  util::condPrint(option::printScopeStack, "pushScope: %p(%s)\n",
                  static_cast<void *>(m_curScope),
                  m_curScope->scopeString().c_str());
}

void SymbolTable::popScope() {
  util::condPrint(option::printScopeStack, "popScope: %p(%s)\n",
                  static_cast<void *>(m_curScope),
                  m_curScope->scopeString().c_str());
  m_curScope = m_curScope->parent();
  assert(m_curScope != nullptr);
}

void SymbolTable::define(Symbol *symbol) {
  assert(m_curScope != nullptr);
  m_curScope->define(symbol);
}

void SymbolTable::initGlobalScope() {
  m_globalScope = new Scope(Scope::Category::Global, nullptr);
  m_curScope = m_globalScope;

  shared_ptr<TypeInfo> voidType =
      make_shared<TypeInfo>(TypeInfo::Category::Void);

  {
    shared_ptr<TypeInfo> sceneType(
        new CustomTypeInfo(builtin::sceneInfo.name()));
    vector<shared_ptr<TypeInfo>> paramTypes(1, sceneType);
    Symbol *defineScene =
        new FunctionSymbol("defineScene", voidType, paramTypes);
    define(defineScene);
  }

  {
    shared_ptr<TypeInfo> rectType(new CustomTypeInfo(builtin::rectInfo.name()));
    vector<shared_ptr<TypeInfo>> paramTypes(1, rectType);
    Symbol *drawRect = new FunctionSymbol("drawRect", voidType, paramTypes);
    define(drawRect);
  }

  {
    shared_ptr<TypeInfo> ellipseType(
        new CustomTypeInfo(builtin::ellipseInfo.name()));
    vector<shared_ptr<TypeInfo>> paramTypes(1, ellipseType);
    Symbol *drawEllipse =
        new FunctionSymbol("drawEllipse", voidType, paramTypes);
    define(drawEllipse);
  }

  {
    shared_ptr<TypeInfo> textType(new CustomTypeInfo(builtin::textInfo.name()));
    vector<shared_ptr<TypeInfo>> paramTypes(1, textType);
    Symbol *drawText = new FunctionSymbol("drawText", voidType, paramTypes);
    define(drawText);
  }

  {
    shared_ptr<TypeInfo> polygonType(
        new CustomTypeInfo(builtin::polygonInfo.name()));
    vector<shared_ptr<TypeInfo>> paramTypes(1, polygonType);
    Symbol *drawPolygon =
        new FunctionSymbol("drawPolygon", voidType, paramTypes);
    define(drawPolygon);
  }

  {
    shared_ptr<TypeInfo> lineType(new CustomTypeInfo(builtin::lineInfo.name()));
    vector<shared_ptr<TypeInfo>> paramTypes(1, lineType);
    Symbol *drawLine = new FunctionSymbol("drawLine", voidType, paramTypes);
    define(drawLine);
  }

  {
    shared_ptr<TypeInfo> polylineType(
        new CustomTypeInfo(builtin::polylineInfo.name()));
    vector<shared_ptr<TypeInfo>> paramTypes(1, polylineType);
    Symbol *drawPolyline =
        new FunctionSymbol("drawPolyline", voidType, paramTypes);
    define(drawPolyline);
  }

  {
    vector<shared_ptr<TypeInfo>> paramTypes(1, voidType);
    Symbol *len = new FunctionSymbol(
        "len", make_shared<TypeInfo>(TypeInfo::Category::Int), paramTypes);
    define(len);
    Symbol *print = new FunctionSymbol("print", voidType, paramTypes);
    define(print);
  }
}

}  // namespace backend
}  // namespace rectangle
