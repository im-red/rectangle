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

#include <stdexcept>

#include "ast.h"
#include "astnode.h"

namespace rectangle {

class VisitException : public std::runtime_error {
 public:
  explicit VisitException(const std::string &s) : std::runtime_error(s) {}
  VisitException(const std::string &ast, const std::string &s)
      : VisitException("visit " + ast + " exception: " + s) {}
};

class Visitor {
 public:
  virtual ~Visitor();

 protected:
  virtual void visit(Expr *e);
  virtual void visit(Stmt *s);
  virtual void visit(DocumentDecl *dd);

  virtual void visit(IntegerLiteral *) = 0;
  virtual void visit(FloatLiteral *) = 0;
  virtual void visit(StringLiteral *) = 0;
  virtual void visit(InitListExpr *) = 0;
  virtual void visit(BinaryOperatorExpr *) = 0;
  virtual void visit(UnaryOperatorExpr *) = 0;
  virtual void visit(CallExpr *) = 0;
  virtual void visit(ListSubscriptExpr *) = 0;
  virtual void visit(MemberExpr *) = 0;
  virtual void visit(RefExpr *) = 0;
  virtual void visit(VarDecl *) = 0;
  virtual void visit(PropertyDecl *) = 0;
  virtual void visit(ParamDecl *) = 0;
  virtual void visit(CompoundStmt *) = 0;
  virtual void visit(DeclStmt *) = 0;
  virtual void visit(IfStmt *) = 0;
  virtual void visit(WhileStmt *) = 0;
  virtual void visit(BreakStmt *) = 0;
  virtual void visit(ContinueStmt *) = 0;
  virtual void visit(ReturnStmt *) = 0;
  virtual void visit(ExprStmt *) = 0;
  virtual void visit(FunctionDecl *) = 0;
  virtual void visit(EnumConstantDecl *) = 0;
  virtual void visit(EnumDecl *) = 0;
  virtual void visit(ComponentDefinationDecl *) = 0;
  virtual void visit(FieldDecl *) = 0;
  virtual void visit(StructDecl *) = 0;
  virtual void visit(BindingDecl *) = 0;
  virtual void visit(ComponentInstanceDecl *) = 0;
};

}  // namespace rectangle
