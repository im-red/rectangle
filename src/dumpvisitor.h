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

#include "visitor.h"

namespace rectangle
{
namespace frontend
{

class DumpVisitor : public Visitor
{
public:
    DumpVisitor();

    void visit(AST *ast);

protected:
    void visit(Expr *e) override { Visitor::visit(e); }
    void visit(Stmt *s) override { Visitor::visit(s); }
    void visit(DocumentDecl *dd) override;
    void visit(IntegerLiteral *il) override;
    void visit(FloatLiteral *fl) override;
    void visit(StringLiteral *sl) override;
    void visit(InitListExpr *ile) override;
    void visit(BinaryOperatorExpr *boe) override;
    void visit(UnaryOperatorExpr *uoe) override;
    void visit(CallExpr *ce) override;
    void visit(ListSubscriptExpr *lse) override;
    void visit(MemberExpr *me) override;
    void visit(RefExpr *re) override;
    void visit(VarDecl *vd) override;
    void visit(PropertyDecl *pd) override;
    void visit(ParamDecl *pd) override;
    void visit(CompoundStmt *cs) override;
    void visit(DeclStmt *ds) override;
    void visit(IfStmt *is) override;
    void visit(WhileStmt *ws) override;
    void visit(BreakStmt *) override;
    void visit(ContinueStmt *) override;
    void visit(ReturnStmt *rs) override;
    void visit(ExprStmt *es) override;
    void visit(FunctionDecl *fd) override;
    void visit(EnumConstantDecl *ecd) override;
    void visit(EnumDecl *ed) override;
    void visit(ComponentDefinationDecl *cdd) override;
    void visit(FieldDecl *fd) override;
    void visit(StructDecl *sd) override;
    void visit(BindingDecl *bd) override;
    void visit(ComponentInstanceDecl *cid) override;

private:
    void incIndent();
    void decIndent();
    void printIndent();

    struct IndentPrinter
    {
        IndentPrinter(DumpVisitor *dv) : m_dv(dv)
        {
            m_dv->incIndent();
            m_dv->printIndent();
        }
        ~IndentPrinter()
        {
            m_dv->decIndent();
        }
        DumpVisitor *m_dv;
    };

private:
    int m_indent = 0;
};

}
}
