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

#include <map>
#include <string>
#include <vector>

#include "typeinfo.h"

namespace rectangle {

struct ASTNode;

namespace backend {

class TypeInfo;

class Symbol {
 public:
  enum class Category {
    Invalid,
    Variable,
    Parameter,
    Struct,
    Field,
    Component,
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

  Symbol(Category cat, const std::string &n,
         std::shared_ptr<TypeInfo> ti = std::shared_ptr<TypeInfo>(),
         ASTNode *ast = nullptr);
  virtual ~Symbol();

  std::string symbolString() const;

  Category category() const;
  std::string name() const;
  std::shared_ptr<TypeInfo> typeInfo() const;
  void setTypeInfo(const std::shared_ptr<TypeInfo> &typeInfo);
  ASTNode *astNode() const;
  void setAstNode(ASTNode *astNode);

 private:
  Category m_category = Category::Invalid;
  std::string m_name;
  std::shared_ptr<TypeInfo> m_typeInfo;
  ASTNode *m_astNode = nullptr;
};

class MethodSymbol : public Symbol {
 public:
  MethodSymbol(const std::string &name, const std::shared_ptr<TypeInfo> &ti,
               Symbol *componentSymbol,
               const std::vector<std::shared_ptr<TypeInfo>> &paramTypes)
      : Symbol(Symbol::Category::Method, name, ti),
        m_componentSymbol(componentSymbol),
        m_paramTypes(paramTypes) {}

  Symbol *componentSymbol() const { return m_componentSymbol; }
  std::vector<std::shared_ptr<TypeInfo>> paramTypes() const {
    return m_paramTypes;
  }

 private:
  Symbol *m_componentSymbol;
  std::vector<std::shared_ptr<TypeInfo>> m_paramTypes;
};

class FunctionSymbol : public Symbol {
 public:
  FunctionSymbol(const std::string &name, const std::shared_ptr<TypeInfo> &ti,
                 const std::vector<std::shared_ptr<TypeInfo>> &paramTypes =
                     std::vector<std::shared_ptr<TypeInfo>>())
      : Symbol(Symbol::Category::Function, name, ti),
        m_paramTypes(paramTypes) {}

  std::vector<std::shared_ptr<TypeInfo>> paramTypes() const {
    return m_paramTypes;
  }

 private:
  std::vector<std::shared_ptr<TypeInfo>> m_paramTypes;
};

class Scope {
 public:
  enum class Category {
    Invalid,
    Global,
    Local,
    Function,
    Component,
    Method,
    Struct,
    Instance
  };

 public:
  static std::string scopeCategoryString(Category category);

  Scope(Category cat, Scope *p);
  virtual ~Scope();

  virtual std::string scopeString() const {
    char buf[512];
    snprintf(buf, sizeof(buf), "%s (%s)",
             scopeCategoryString(m_category).c_str(), m_scopeName.c_str());
    return std::string(buf);
  }

  Scope *parent() const { return m_parent; }
  Category category() const { return m_category; }

  void define(Symbol *sym);
  Symbol *resolve(const std::string &name);

  std::string scopeName() const;
  void setScopeName(const std::string &scopeName);

  Scope *componentScope() const;
  void setComponentScope(Scope *componentScope);

 private:
  std::map<std::string, Symbol *> m_symbols;
  Scope *m_parent = nullptr;
  Scope *m_componentScope = nullptr;
  Category m_category = Category::Invalid;
  std::string m_scopeName = "anonymous";
};

class ScopeSymbol : public Symbol, public Scope {
 public:
  ScopeSymbol(
      Symbol::Category symCat, const std::string &name,
      Scope::Category scopeCat, Scope *parent,
      const std::shared_ptr<TypeInfo> &ti = std::shared_ptr<TypeInfo>());
};

}  // namespace backend
}  // namespace rectangle
