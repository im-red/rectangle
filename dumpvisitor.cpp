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

#include "dumpvisitor.h"

#include <assert.h>

using namespace std;

DumpVisitor::DumpVisitor()
{

}

void DumpVisitor::visit(AST *ast)
{
    m_indent = 0;
    auto documents = ast->documents();

    for (auto doc : documents)
    {
        assert(doc != nullptr);
        printf("---------- dump DocumentDecl begin ----------\n");
        visit(doc);
        printf("----------- dump DocumentDecl end -----------\n");
    }
}

void DumpVisitor::visit(IntegerLiteral *il)
{
    IndentPrinter ip(this);
    printf("IntegerLiteral(%d)\n", il->value);
}

void DumpVisitor::visit(FloatLiteral *fl)
{
    IndentPrinter ip(this);
    printf("FloatLiteral(%lf)\n", static_cast<double>(fl->value));
}

void DumpVisitor::visit(StringLiteral *sl)
{
    IndentPrinter ip(this);
    printf("StringLiteral(\"%s\")\n", sl->value.c_str());
}

void DumpVisitor::visit(InitListExpr *ile)
{
    IndentPrinter ip(this);
    printf("InitListExpr\n");
    for (auto &e : ile->exprList)
    {
        visit(e.get());
    }
}

void DumpVisitor::visit(BinaryOperatorExpr *boe)
{
    IndentPrinter ip(this);
    printf("BinaryOperatorExpr(%s)\n", BinaryOperatorExpr::typeString(boe->op).c_str());
    visit(boe->left.get());
    visit(boe->right.get());
}

void DumpVisitor::visit(UnaryOperatorExpr *uoe)
{
    IndentPrinter ip(this);
    printf("UnaryOperatorExpr(%s)\n", UnaryOperatorExpr::typeString(uoe->op).c_str());
    visit(uoe->expr.get());
}

void DumpVisitor::visit(CallExpr *ce)
{
    IndentPrinter ip(this);
    printf("CallExpr\n");
    visit(ce->funcExpr.get());
    for (auto &e : ce->paramList)
    {
        visit(e.get());
    }
}

void DumpVisitor::visit(ListSubscriptExpr *lse)
{
    IndentPrinter ip(this);
    printf("ListSubscriptExpr\n");
    visit(lse->listExpr.get());
    visit(lse->indexExpr.get());
}

void DumpVisitor::visit(MemberExpr *me)
{
    IndentPrinter ip(this);
    printf("MemberExpr(%s)\n", me->name.c_str());
    visit(me->instanceExpr.get());
}

void DumpVisitor::visit(RefExpr *re)
{
    IndentPrinter ip(this);
    printf("RefExpr(%s)\n", re->name.c_str());
}

void DumpVisitor::visit(VarDecl *vd)
{
    IndentPrinter ip(this);
    printf("VarDecl(%s %s)\n", vd->type->toString().c_str(), vd->name.c_str());
    if (vd->expr)
    {
        visit(vd->expr.get());
    }
}

void DumpVisitor::visit(PropertyDecl *pd)
{
    IndentPrinter ip(this);
    printf("PropertyDecl(%s %s)\n", pd->type->toString().c_str(), pd->name.c_str());
    if (pd->expr)
    {
        visit(pd->expr.get());
    }
}

void DumpVisitor::visit(ParamDecl *pd)
{
    IndentPrinter ip(this);
    printf("ParamDecl(%s %s)\n", pd->type->toString().c_str(), pd->name.c_str());
}

void DumpVisitor::visit(CompoundStmt *cs)
{
    IndentPrinter ip(this);
    printf("CompoundStmt\n");
    for (auto &s : cs->stmtList)
    {
        visit(s.get());
    }
}

void DumpVisitor::visit(DeclStmt *ds)
{
    IndentPrinter ip(this);
    printf("DeclStmt\n");
    visit(ds->decl.get());
}

void DumpVisitor::visit(IfStmt *is)
{
    IndentPrinter ip(this);
    printf("IfStmt\n");
    visit(is->condition.get());
    visit(is->thenStmt.get());
    if (is->elseStmt)
    {
        visit(is->elseStmt.get());
    }
}

void DumpVisitor::visit(WhileStmt *ws)
{
    IndentPrinter ip(this);
    printf("WhileStmt\n");
    visit(ws->condition.get());
    visit(ws->bodyStmt.get());
}

void DumpVisitor::visit(BreakStmt *)
{
    IndentPrinter ip(this);
    printf("BreakStmt\n");
}

void DumpVisitor::visit(ContinueStmt *)
{
    IndentPrinter ip(this);
    printf("ContinueStmt\n");
}

void DumpVisitor::visit(ReturnStmt *rs)
{
    IndentPrinter ip(this);
    printf("ReturnStmt\n");
    if (rs->returnExpr)
    {
        visit(rs->returnExpr.get());
    }
}

void DumpVisitor::visit(ExprStmt *es)
{
    IndentPrinter ip(this);
    printf("ExprStmt\n");
    visit(es->expr.get());
}

void DumpVisitor::visit(FunctionDecl *fd)
{
    IndentPrinter ip(this);
    if (fd->component)
    {
        printf("FunctionDecl(%s %s::%s)\n", fd->returnType->toString().c_str(), fd->component->name.c_str(), fd->name.c_str());
    }
    else
    {
        printf("FunctionDecl(%s %s)\n", fd->returnType->toString().c_str(), fd->name.c_str());
    }

    for (auto &p : fd->paramList)
    {
        visit(p.get());
    }
    visit(fd->body.get());
}

void DumpVisitor::visit(EnumConstantDecl *ecd)
{
    IndentPrinter ip(this);
    printf("EnumConstantDecl(%s)\n", ecd->name.c_str());
}

void DumpVisitor::visit(EnumDecl *ed)
{
    IndentPrinter ip(this);
    printf("EnumDecl(%s)\n", ed->name.c_str());
    for (auto &c : ed->constantList)
    {
        visit(c.get());
    }
}

void DumpVisitor::visit(ComponentDefinationDecl *cdd)
{
    IndentPrinter ip(this);
    printf("ComponentDefinationDecl(%s)\n", cdd->name.c_str());
    for (auto &p : cdd->enumList)
    {
        visit(p.get());
    }
    for (auto &p : cdd->propertyList)
    {
        visit(p.get());
    }
    for (auto &p : cdd->methodList)
    {
        visit(p.get());
    }
}

void DumpVisitor::visit(FieldDecl *fd)
{
    IndentPrinter ip(this);
    printf("FieldDecl(%s %s)\n", fd->type->toString().c_str(), fd->name.c_str());
}

void DumpVisitor::visit(StructDecl *sd)
{
    IndentPrinter ip(this);
    printf("StructDecl(%s)\n", sd->name.c_str());
    for (auto &p : sd->fieldList)
    {
        visit(p.get());
    }
}

void DumpVisitor::visit(BindingDecl *bd)
{
    IndentPrinter ip(this);
    printf("BindingDecl(%s)\n", bd->name.c_str());
    visit(bd->expr.get());
}

void DumpVisitor::visit(ComponentInstanceDecl *cid)
{
    IndentPrinter ip(this);
    printf("ComponentInstanceDecl(%s)\n", cid->componentName.c_str());
    for (auto &p : cid->bindingList)
    {
        visit(p.get());
    }
    for (auto &p : cid->childrenList)
    {
        visit(p.get());
    }
}

void DumpVisitor::incIndent()
{
    m_indent++;
}

void DumpVisitor::decIndent()
{
    m_indent--;
}

void DumpVisitor::printIndent()
{
    std::string space(static_cast<size_t>(m_indent), ' ');
    printf("%s", space.c_str());
}
