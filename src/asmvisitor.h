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
#include "asmtext.h"

namespace rectangle
{
namespace backend
{

class AsmVisitor : public Visitor
{
public:
    AsmVisitor();

    AsmText visit(AST *ast);

protected:
    void visit(Expr *e) override { Visitor::visit(e); }
    void visit(Stmt *s) override { Visitor::visit(s); }
    void visit(DocumentDecl *dd) override { Visitor::visit(dd); }
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
    void visit(BreakStmt *bs) override;
    void visit(ContinueStmt *cs) override;
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

    void genAsmForInitInstance(ComponentInstanceDecl *cid);
    void genAsmForAllMember(ComponentInstanceDecl *cid);
    void genAsmForPropertyDecl(ComponentInstanceDecl *cid, PropertyDecl *pd);
    void genAsmForBindingDecl(ComponentInstanceDecl *cid, BindingDecl *bd);

private:
    void pushVisitingLvalue(bool lvalue);
    void popVisitingLvalue();
    bool visitingLvalue() const;

    void setVisitingInstance(bool visiting, ComponentDefinationDecl *cdd = nullptr, int index = -1);
    void setVisitingMethod(bool visiting);

    bool visitingInstance() const;
    int instanceIndexVisiting() const;
    ComponentDefinationDecl *componentVisiting() const;
    bool visitingMethod() const;

    void clear();

private:
    AsmText m_asm;
    AST *m_ast = nullptr;

    int m_labelCounter = 0;

    std::vector<std::string> m_breakLabels;
    std::vector<std::string> m_continueLabels;

    enum class LvalueCategory
    {
        Invalid,
        Global,
        Local,
        Field,
        List
    };
    LvalueCategory m_lvalueCategory = LvalueCategory::Invalid;
    int m_lvalueIndex = -1;

    std::vector<bool> m_visitingLvalueStack;

    bool m_visitingInstance = false;
    int m_instanceIndexVisiting = -1;
    ComponentDefinationDecl *m_componentVisiting = nullptr;
    bool m_visitingMethod = false;
    std::string m_curFilePath;
};

}
}
