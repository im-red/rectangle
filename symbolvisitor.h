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
#include "symbol.h"
#include "topologicalsorter.h"

class SymbolVisitor : public Visitor
{
public:
    SymbolVisitor();

    void visit(AST *ast);

protected:
    void visit(Expr *e) override { Visitor::visit(e); }
    void visit(Stmt *s) override { Visitor::visit(s); }
    void visit(DocumentDecl *dd) override { Visitor::visit(dd); }
    void visit(StructDecl *sd) override;
    void visit(ComponentDefinationDecl *cdd) override;
    void visit(ComponentInstanceDecl *cid) override;
    void calculateOrderedMemberInitList();
    void visit(BindingDecl *bd) override;
    void visit(GroupedBindingDecl *gbd) override;
    int visitInstanceIndex(ComponentInstanceDecl *cid);
    void visitInstanceId(ComponentInstanceDecl *cid);
    void visit(IntegerLiteral *e) override;
    void visit(FloatLiteral *e) override;
    void visit(StringLiteral *e) override;
    void visit(InitListExpr *ile) override;
    void visit(BinaryOperatorExpr *b) override;
    void visit(UnaryOperatorExpr *u) override;
    void visit(CallExpr *c) override;
    void visit(ListSubscriptExpr *lse) override;
    void visit(MemberExpr *me) override;
    void visit(RefExpr *re) override;
    void visit(VarDecl *vd) override;
    void visit(FieldDecl *md) override;
    void visitPropertyDefination(PropertyDecl *pd);
    void visitPropertyDefination(GroupedPropertyDecl *gpd);
    void visitPropertyInitialization(PropertyDecl *pd);
    void visitPropertyInitialization(GroupedPropertyDecl *gpd);
    void visit(ParamDecl *pd) override;
    void visit(CompoundStmt *cs) override;
    void visit(DeclStmt *ds) override;
    void visit(IfStmt *is) override;
    void visit(WhileStmt *ws) override;
    void visit(BreakStmt *bs) override;
    void visit(ContinueStmt *cs) override;
    void visit(ReturnStmt *rs) override;
    void visit(ExprStmt *es) override;
    void visitMethodHeader(FunctionDecl *fd);
    void visitMethodBody(FunctionDecl *fd);
    void visit(EnumConstantDecl *ecd) override;
    void visit(EnumDecl *ed) override;
    void visit(FunctionDecl *) override;
    void visit(PropertyDecl *) override;
    void visit(GroupedPropertyDecl *) override;

private:
    void clear();

    void setAnalyzingPropertyDep(bool analyzing, PropertyDecl *pd = nullptr);

    bool analyzingPropertyDep() const;
    int propertyIndexAnalyzing() const;
    PropertyDecl *propertyAnalyzing() const;
    ComponentDefinationDecl *componentDefinationAnalyzing() const;

    void setAnalyzingBindingDep(bool analyzing, BindingDecl *bd = nullptr);

    bool analyzingBindingDep() const;
    int bindingIndexAnalyzing() const;
    BindingDecl *bindingAnalyzing() const;

    void pushInstanceStack(ComponentInstanceDecl *cid);
    void popInstanceStack();
    ComponentInstanceDecl *curInstance() const;
    std::string curInstanceId() const;

private:
    bool m_analyzingPropertyDep = false;
    PropertyDecl *m_propertyAnalyzing = nullptr;
    int m_propertyIndexAnalyzing = -1;

    bool m_analyzingBindingDep = false;
    std::string m_curAnalyzingBindingToId;

    std::map<std::string, BindingDecl *> m_bindingId2bindingDecl;
    std::vector<std::pair<std::string, std::string>> m_bindingIdDeps;

    BindingDecl *m_bindingAnalyzing = nullptr;
    std::vector<ComponentInstanceDecl *> m_instanceStack;

    AST *m_ast = nullptr;
    int m_stackFrameLocals = -1;

    int m_nextInstanceIndex = -1;

    ComponentInstanceDecl *m_topLevelInstance = nullptr;
};

