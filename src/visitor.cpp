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

#include "visitor.h"
#include "symboltable.h"
#include "typeinfo.h"

#include <memory>

#include <assert.h>

namespace rectangle
{

Visitor::~Visitor()
{

}

void Visitor::visit(Expr *e)
{
    assert(e != nullptr);

    switch (e->category)
    {
    case Expr::Category::InitList:
    {
        InitListExpr *l = dynamic_cast<InitListExpr *>(e);
        visit(l);
        break;
    }
    case Expr::Category::BinaryOperator:
    {
        BinaryOperatorExpr *b = dynamic_cast<BinaryOperatorExpr *>(e);
        visit(b);
        break;
    }
    case Expr::Category::UnaryOperator:
    {
        UnaryOperatorExpr *u = dynamic_cast<UnaryOperatorExpr *>(e);
        visit(u);
        break;
    }
    case Expr::Category::Call:
    {
        CallExpr *c = dynamic_cast<CallExpr *>(e);
        visit(c);
        break;
    }
    case Expr::Category::ListSubscript:
    {
        ListSubscriptExpr *l = dynamic_cast<ListSubscriptExpr *>(e);
        visit(l);
        break;
    }
    case Expr::Category::Member:
    {
        MemberExpr *m = dynamic_cast<MemberExpr *>(e);
        visit(m);
        break;
    }
    case Expr::Category::Ref:
    {
        RefExpr *r = dynamic_cast<RefExpr *>(e);
        visit(r);
        break;
    }
    case Expr::Category::Integer:
    {
        IntegerLiteral *il = dynamic_cast<IntegerLiteral *>(e);
        visit(il);
        break;
    }
    case Expr::Category::Float:
    {
        FloatLiteral *fl = dynamic_cast<FloatLiteral *>(e);
        visit(fl);
        break;
    }
    case Expr::Category::String:
    {
        StringLiteral *sl = dynamic_cast<StringLiteral *>(e);
        visit(sl);
        break;
    }
    case Expr::Category::Invalid:
    {
        assert(false);
    }
    }
}

void Visitor::visit(Stmt *s)
{
    switch (s->category)
    {
    case Stmt::Category::If:
    {
        IfStmt *st = dynamic_cast<IfStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Decl:
    {
        DeclStmt *st = dynamic_cast<DeclStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Expr:
    {
        ExprStmt *st = dynamic_cast<ExprStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Break:
    {
        BreakStmt *st = dynamic_cast<BreakStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::While:
    {
        WhileStmt *st = dynamic_cast<WhileStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Return:
    {
        ReturnStmt *st = dynamic_cast<ReturnStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Compound:
    {
        CompoundStmt *st = dynamic_cast<CompoundStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Continue:
    {
        ContinueStmt *st = dynamic_cast<ContinueStmt *>(s);
        visit(st);
        break;
    }
    default:
    {
        assert(false);
    }
    }
}

void Visitor::visit(DocumentDecl *dd)
{
    assert(dd != nullptr);

    if (dd->type == DocumentDecl::Type::Defination)
    {
        ComponentDefinationDecl *cdd = dynamic_cast<ComponentDefinationDecl *>(dd);
        visit(cdd);
    }
    else if (dd->type == DocumentDecl::Type::Instance)
    {
        ComponentInstanceDecl *cid = dynamic_cast<ComponentInstanceDecl *>(dd);
        visit(cid);
    }
    else
    {
        StructDecl *sd = dynamic_cast<StructDecl *>(dd);
        visit(sd);
    }
}

}
