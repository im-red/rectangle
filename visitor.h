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

class VisitException : public std::runtime_error
{
public:
    explicit VisitException(const std::string &s)
        : std::runtime_error(s)
    {}
    VisitException(const std::string &ast, const std::string &s)
        : VisitException("visit " + ast + " exception: " + s)
    {}
};

class Visitor
{
public:
    virtual ~Visitor();

protected:
    virtual void visit(Expr *e);
    virtual void visit(IntegerLiteral *) {}
    virtual void visit(FloatLiteral *) {}
    virtual void visit(StringLiteral *) {}
    virtual void visit(InitListExpr *) {}
    virtual void visit(BinaryOperatorExpr *) {}
    virtual void visit(UnaryOperatorExpr *) {}
    virtual void visit(CallExpr *) {}
    virtual void visit(ListSubscriptExpr *) {}
    virtual void visit(MemberExpr *) {}
    virtual void visit(RefExpr *) {}
    virtual void visit(VarDecl *) {}
    virtual void visit(PropertyDecl *) {}
    virtual void visit(GroupedPropertyDecl *) {}
    virtual void visit(ParamDecl *) {}
    virtual void visit(Stmt *);
    virtual void visit(CompoundStmt *) {}
    virtual void visit(DeclStmt *) {}
    virtual void visit(IfStmt *) {}
    virtual void visit(WhileStmt *) {}
    virtual void visit(BreakStmt *) {}
    virtual void visit(ContinueStmt *) {}
    virtual void visit(ReturnStmt *) {}
    virtual void visit(ExprStmt *) {}
    virtual void visit(FunctionDecl *) {}
    virtual void visit(EnumConstantDecl *) {}
    virtual void visit(EnumDecl *) {}
    virtual void visit(ComponentDefinationDecl *) {}
    virtual void visit(FieldDecl *) {}
    virtual void visit(StructDecl *) {}
    virtual void visit(BindingDecl *) {}
    virtual void visit(GroupedBindingDecl *) {}
    virtual void visit(ComponentInstanceDecl *) {}
    virtual void visit(DocumentDecl *) {}
};

