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

#include "symbol.h"

class SymbolVisitor
{
public:
    SymbolVisitor();

    void setDocuments(std::vector<DocumentDecl *> documents);
    void visit();

private:
    std::shared_ptr<Scope> curScope();
    void pushScope(const std::shared_ptr<Scope> &scope);
    void popScope();

    void initGlobalScope();
    void initBuiltInStructs();

private:
    void visit(DocumentDecl *dd);
    void visit(StructDecl *sd);
    void visit(ComponentDefinationDecl *cdd);
    void visit(ComponentInstanceDecl *cid);
    void visit(Expr *e);
    void visit(InitListExpr *ile);
    void visit(BinaryOperatorExpr *b);
    void visit(UnaryOperatorExpr *u);
    void visit(CallExpr *c);
    void visit(ListSubscriptExpr *lse);
    void visit(MemberExpr *me);
    void visit(RefExpr *re);
    void visit(VarDecl *vd);
    void visit(FieldDecl *md);
    void visitPropertyDefination(PropertyDecl *pd);
    void visitPropertyDefination(GroupedPropertyDecl *gpd);
    void visitPropertyInitialization(PropertyDecl *pd);
    void visitPropertyInitialization(GroupedPropertyDecl *gpd);
    void visit(ParamDecl *pd);
    void visit(Stmt *s);
    void visit(CompoundStmt *cs);
    void visit(DeclStmt *ds);
    void visit(IfStmt *is);
    void visit(WhileStmt *ws);
    void visit(BreakStmt *bs);
    void visit(ContinueStmt *cs);
    void visit(ReturnStmt *rs);
    void visit(ExprStmt *es);
    void visitMethodHeader(FunctionDecl *fd);
    void visitMethodBody(FunctionDecl *fd);
    void visit(EnumConstantDecl *ecd);
    void visit(EnumDecl *ed);
    void visit(BindingDecl *bd);
    void visit(GroupedBindingDecl *gbd);

private:
    std::vector<DocumentDecl *> m_documents;
    std::vector<std::unique_ptr<StructDecl>> m_builtInStructs;
    std::vector<std::shared_ptr<Scope>> m_scopes;
    std::vector<std::shared_ptr<Symbol>> m_symbols;
    std::shared_ptr<Scope> m_curScope = nullptr;

    bool m_analyzingPropertyDep = false;
    PropertyDecl *m_curAnalyzingProperty = nullptr;

    int m_functionLocals = -1;
    int m_labelCounter = 0;

    std::vector<int> m_breakLabels;
    std::vector<int> m_continueLabels;

    bool m_visitingLvalue = false;
    enum class LvalueCategory
    {
        Invalid,
        Global,
        Local,
        Field,
        List
    };
    LvalueCategory m_lvalueCategory = LvalueCategory::Invalid;
    int m_lvalueIndex;

    std::string m_curComponentName;
};

