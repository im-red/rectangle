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

#include "asmvisitor.h"
#include "symboltable.h"
#include "typeinfo.h"

#include <assert.h>

using namespace std;

AsmVisitor::AsmVisitor()
{
    m_visitingLvalueStack.push_back(false);
}

AsmText AsmVisitor::visit(AST *ast)
{
    m_ast = ast;
    m_asm.clear();

    auto documents = m_ast->documents();

    vector<DocumentDecl *> structs;
    vector<DocumentDecl *> definations;
    vector<DocumentDecl *> instances;

    for (auto doc : documents)
    {
        if (doc)
        {
            switch (doc->type)
            {
            case DocumentDecl::Type::Struct:        structs.push_back(doc);     break;
            case DocumentDecl::Type::Defination:    definations.push_back(doc); break;
            case DocumentDecl::Type::Instance:      instances.push_back(doc);   break;
            }
        }
    }

    for (auto doc : structs)
    {
        visit(doc);
    }
    for (auto doc : definations)
    {
        visit(doc);
    }

    assert(instances.size() == 1);
    ComponentInstanceDecl *cid = dynamic_cast<ComponentInstanceDecl *>(instances[0]);
    assert(cid != nullptr);

    m_asm.appendLine({".def", "main", "0", to_string(cid->instanceTreeSize)});
    genAsmForInitInstance(cid);
    genAsmForAllMember(cid);
    visit(cid);
    m_asm.appendLine({"ret"});

    return m_asm;
}

void AsmVisitor::visit(IntegerLiteral *il)
{
    assert(il != nullptr);
    if (visitingLvalue())
    {
        throw VisitException("IntegerLiteral", "Illegal lvalue");
    }

    m_asm.appendLine({"iconst", to_string(il->value)});
}

void AsmVisitor::visit(FloatLiteral *fl)
{
    assert(fl != nullptr);
    if (visitingLvalue())
    {
        throw VisitException("FloatLiteral", "Illegal lvalue");
    }

    m_asm.appendLine({"fconst", to_string(fl->value)});
}

void AsmVisitor::visit(StringLiteral *sl)
{
    assert(sl != nullptr);
    if (visitingLvalue())
    {
        throw VisitException("StringLiteral", "Illegal lvalue");
    }

    m_asm.appendLine({"sconst", sl->value});
}

void AsmVisitor::visit(InitListExpr *ile)
{
    assert(ile != nullptr);
    if (visitingLvalue())
    {
        throw VisitException("InitListExpr", "Illegal lvalue");
    }

    m_asm.appendLine({"vector"});
    if (ile->exprList.size() != 0)
    {
        for (size_t i = 0; i < ile->exprList.size(); i++)
        {
            visit(ile->exprList[i].get());
            m_asm.appendLine({"vappend"});
        }
    }
}

void AsmVisitor::visit(BinaryOperatorExpr *boe)
{
    assert(boe != nullptr);
    if (visitingLvalue())
    {
        throw VisitException("BinaryOperatorExpr", "Illegal lvalue");
    }

    if (boe->op == BinaryOperatorExpr::Op::Assign)
    {
        pushVisitingLvalue(true);
        m_lvalueCategory = LvalueCategory::Invalid;
        visit(boe->left.get());
        popVisitingLvalue();
    }
    else
    {
        visit(boe->left.get());
    }
    visit(boe->right.get());

    if (boe->op == BinaryOperatorExpr::Op::Assign)
    {
        switch (m_lvalueCategory)
        {
        case LvalueCategory::List:      m_asm.appendLine({"vstore"}); break;
        case LvalueCategory::Local:     m_asm.appendLine({"lstore", to_string(m_lvalueIndex)}); break;
        case LvalueCategory::Global:    m_asm.appendLine({"gstore", to_string(m_lvalueIndex)}); break;
        case LvalueCategory::Field:     m_asm.appendLine({"fstore", to_string(m_lvalueIndex)}); break;
        case LvalueCategory::Invalid:   assert(false); break;
        }
    }
    else
    {
        string prefix;
        switch (boe->left->typeInfo->category())
        {
        case TypeInfo::Category::Int:       prefix = "i"; break;
        case TypeInfo::Category::Float:     prefix = "f"; break;
        case TypeInfo::Category::String:    prefix = "s"; break;
        default:                            throw VisitException("BinaryOperatorExpr", "Binary operator is only valid for int/float/string");
        }

        string op;
        switch (boe->op)
        {
        case BinaryOperatorExpr::Op::LogicalAnd:    op = "and"; break;
        case BinaryOperatorExpr::Op::LogicalOr:     op = "or"; break;
        case BinaryOperatorExpr::Op::LessThan:      op = "lt"; break;
        case BinaryOperatorExpr::Op::GreaterThan:   op = "gt"; break;
        case BinaryOperatorExpr::Op::LessEqual:     op = "le"; break;
        case BinaryOperatorExpr::Op::GreaterEqual:  op = "ge"; break;
        case BinaryOperatorExpr::Op::Equal:         op = "eq"; break;
        case BinaryOperatorExpr::Op::NotEqual:      op = "ne"; break;
        case BinaryOperatorExpr::Op::Plus:          op = "add"; break;
        case BinaryOperatorExpr::Op::Minus:         op = "sub"; break;
        case BinaryOperatorExpr::Op::Multiply:      op = "mul"; break;
        case BinaryOperatorExpr::Op::Divide:        op = "div"; break;
        case BinaryOperatorExpr::Op::Remainder:     op = "rem"; break;
        case BinaryOperatorExpr::Op::Assign:        assert(false); break;
        case BinaryOperatorExpr::Op::Invalid:       assert(false); break;
        }

        m_asm.appendLine({prefix + op});
    }
}

void AsmVisitor::visit(UnaryOperatorExpr *uoe)
{
    assert(uoe != nullptr);
    if (visitingLvalue())
    {
        throw VisitException("UnaryOperatorExpr", "Illegal lvalue");
    }

    visit(uoe->expr.get());

    if (uoe->op != UnaryOperatorExpr::Op::Positive)
    {
        string prefix;
        switch (uoe->typeInfo->category())
        {
        case TypeInfo::Category::Int:       prefix = "i"; break;
        case TypeInfo::Category::Float:     prefix = "f"; break;
        case TypeInfo::Category::String:
        default:                            throw VisitException("UnaryOperatorExpr", "Unary operator is only valid for int/float");
        }

        string op;
        switch (uoe->op)
        {
        case UnaryOperatorExpr::Op::Negative:   op = "neg"; break;
        case UnaryOperatorExpr::Op::Not:        op = "not"; break;
        case UnaryOperatorExpr::Op::Positive:
        case UnaryOperatorExpr::Op::Invalid:    assert(false); break;
        }

        m_asm.appendLine({prefix + op});
    }
}

void AsmVisitor::visit(CallExpr *ce)
{
    assert(ce != nullptr);
    if (visitingLvalue())
    {
        throw VisitException("CallExpr", "Illegal lvalue");
    }

    Scope *scope = ce->scope;
    assert(scope != nullptr);

    string functionName;

    if (ce->funcExpr->category == Expr::Category::Ref)
    {
        RefExpr *r = dynamic_cast<RefExpr *>(ce->funcExpr.get());
        assert(r != nullptr);

        visit(r);

        Symbol *func = scope->resolve(r->name);
        assert(func != nullptr);

        if (func->category() == Symbol::Category::Method)
        {
            MethodSymbol *methodSymbol = dynamic_cast<MethodSymbol *>(func);
            assert(methodSymbol != nullptr);

            Symbol *componentSymbol = methodSymbol->componentSymbol();
            assert(componentSymbol != nullptr);

            functionName = componentSymbol->name() + "::" + methodSymbol->name();
        }
        else
        {
            functionName = r->name;
        }
    }
    else if (ce->funcExpr->category == Expr::Category::Member)
    {
        MemberExpr *m = dynamic_cast<MemberExpr *>(ce->funcExpr.get());
        assert(m != nullptr);

        visit(m->instanceExpr.get());

        string instanceTypeName = m->instanceExpr->typeInfo->toString();
        Symbol *instanceTypeSymbol = scope->resolve(instanceTypeName);
        assert(instanceTypeSymbol != nullptr);

        ScopeSymbol *scopeSym = dynamic_cast<ScopeSymbol *>(instanceTypeSymbol);
        assert(scopeSym != nullptr);

        Symbol *methodSymbol = scopeSym->resolve(m->name);
        assert(methodSymbol != nullptr);

        assert(methodSymbol->category() == Symbol::Category::Method);
        functionName = instanceTypeName + "::" + methodSymbol->name();
    }
    else
    {
        assert(false);
    }

    for (auto &p : ce->paramList)
    {
        visit(p.get());
    }

    if (functionName == "len"
            || functionName == "print"
            || functionName == "drawRect"
            || functionName == "drawText"
            || functionName == "drawPt")
    {
        m_asm.appendLine({functionName});
    }
    else
    {
        m_asm.appendLine({"call", functionName});
    }
}

void AsmVisitor::visit(ListSubscriptExpr *lse)
{
    assert(lse != nullptr);

    pushVisitingLvalue(false);
    visit(lse->listExpr.get());
    visit(lse->indexExpr.get());
    popVisitingLvalue();

    if (visitingLvalue())
    {
        m_lvalueCategory = LvalueCategory::List;
    }
    else
    {
        m_asm.appendLine({"vload"});
    }
}

void AsmVisitor::visit(MemberExpr *me)
{
    assert(me != nullptr);

    Scope *scope = me->scope;
    assert(scope != nullptr);

    pushVisitingLvalue(false);
    visit(me->instanceExpr.get());
    popVisitingLvalue();

    shared_ptr<TypeInfo> instanceTypeInfo = me->instanceExpr->typeInfo;
    string instanceTypeName = instanceTypeInfo->toString();

    Symbol *instanceTypeSymbol = nullptr;
    if (instanceTypeInfo->category() == TypeInfo::Category::Group)
    {
        shared_ptr<GroupTypeInfo> groupTypeInfo = dynamic_pointer_cast<GroupTypeInfo>(instanceTypeInfo);
        assert(groupTypeInfo != nullptr);

        string componentName = groupTypeInfo->componentType()->toString();
        string groupName = groupTypeInfo->name();

        Symbol *componentSymbol = scope->resolve(componentName);
        assert(componentSymbol != nullptr);
        assert(componentSymbol->category() == Symbol::Category::Component);

        ScopeSymbol *componentScope = dynamic_cast<ScopeSymbol *>(componentSymbol);
        assert(componentScope != nullptr);

        Symbol *groupSymbol = componentScope->resolve(groupName);
        assert(groupSymbol != nullptr);

        instanceTypeSymbol = groupSymbol;
    }
    else if (instanceTypeInfo->category() == TypeInfo::Category::Custom)
    {
        instanceTypeSymbol = scope->resolve(instanceTypeName);
        assert(instanceTypeSymbol != nullptr);
    }
    else
    {
        assert(false);
    }

    ScopeSymbol *scopeSymbol = dynamic_cast<ScopeSymbol *>(instanceTypeSymbol);
    assert(scopeSymbol != nullptr);

    Symbol *member = scopeSymbol->resolve(me->name);
    assert(member != nullptr);

    ASTNode *astNode = member->astNode();

    int fieldIndex = -1;
    if (member->category() == Symbol::Category::Field)
    {
        assert(astNode != nullptr);
        FieldDecl *fd = dynamic_cast<FieldDecl *>(astNode);
        assert(fd != nullptr);
        fieldIndex = fd->fieldIndex;
    }
    else if (member->category() == Symbol::Category::Property)
    {
        assert(astNode != nullptr);
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(astNode);
        assert(pd != nullptr);
        fieldIndex = pd->fieldIndex;
    }
    else if (member->category() == Symbol::Category::PropertyGroup) // PropertyGroup
    {
        fieldIndex = -1;
    }
    else if (member->category() == Symbol::Category::EnumConstants)
    {
        assert(astNode != nullptr);
        EnumConstantDecl *ecd = dynamic_cast<EnumConstantDecl *>(astNode);
        assert(ecd != nullptr);
        m_asm.appendLine({"iconst", to_string(ecd->value)});
    }
    else
    {
        assert(false);
    }

    if (fieldIndex != -1)
    {
        if (visitingLvalue())
        {
            m_lvalueCategory = LvalueCategory::Field;
            m_lvalueIndex = fieldIndex;
        }
        else
        {
            m_asm.appendLine({"fload", to_string(fieldIndex)});
        }
    }
}

void AsmVisitor::visit(RefExpr *re)
{
    assert(re != nullptr);

    Scope *scope = re->scope;
    assert(scope != nullptr);

    Symbol *symbol = scope->resolve(re->name);
    assert(symbol != nullptr);

    ASTNode *astNode = symbol->astNode();
    if (visitingLvalue())
    {
        switch (symbol->category())
        {
        case Symbol::Category::Variable:
        {
            VarDecl *vd = dynamic_cast<VarDecl *>(astNode);
            assert(vd != nullptr);

            m_lvalueCategory = LvalueCategory::Local;
            m_lvalueIndex = vd->localIndex;

            break;
        }
        case Symbol::Category::Parameter:
        {
            ParamDecl *pd = dynamic_cast<ParamDecl *>(astNode);
            assert(pd != nullptr);

            m_lvalueCategory = LvalueCategory::Local;
            m_lvalueIndex = pd->localIndex;

            break;
        }
        case Symbol::Category::Property:
        {
            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(astNode);
            assert(pd != nullptr);

            m_lvalueCategory = LvalueCategory::Field;
            m_lvalueIndex = pd->fieldIndex;

            m_asm.appendLine({"lload", "0"});

            break;
        }
        case Symbol::Category::PropertyGroup:
        {
            m_asm.appendLine({"lload", "0"});

            break;
        }
        default:
        {
            assert(false);
        }
        }
    }
    else
    {
        switch (symbol->category())
        {
        case Symbol::Category::Variable:
        {
            VarDecl *vd = dynamic_cast<VarDecl *>(astNode);
            assert(vd != nullptr);

            m_asm.appendLine({"lload", to_string(vd->localIndex)});

            break;
        }
        case Symbol::Category::Parameter:
        {
            ParamDecl *pd = dynamic_cast<ParamDecl *>(astNode);
            assert(pd != nullptr);

            m_asm.appendLine({"lload", to_string(pd->localIndex)});

            break;
        }
        case Symbol::Category::Property:
        {
            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(astNode);
            assert(pd != nullptr);

            int localIndex = -1;
            if (visitingMethod())
            {
                localIndex = 0;
            }
            else if (visitingInstance())
            {
                localIndex = instanceIndexVisiting();
            }
            else
            {
                assert(false);
            }

            m_asm.appendLine({"lload", to_string(localIndex)});
            m_asm.appendLine({"fload", to_string(pd->fieldIndex)});

            break;
        }
        case Symbol::Category::PropertyGroup:
        {
            m_asm.appendLine({"lload", "0"});

            break;
        }
        case Symbol::Category::EnumConstants:
        {
            EnumConstantDecl *ecd = dynamic_cast<EnumConstantDecl *>(astNode);
            assert(ecd != nullptr);

            m_asm.appendLine({"iconst", to_string(ecd->value)});

            break;
        }
        case Symbol::Category::Method:
        {
            m_asm.appendLine({"lload", "0"});

            break;
        }
        case Symbol::Category::InstanceId:
        {
            ComponentInstanceDecl *cid = dynamic_cast<ComponentInstanceDecl *>(astNode);
            assert(cid != nullptr);

            m_asm.appendLine({"lload", to_string(cid->instanceIndex)});
            break;
        }
        default:
        {
        }
        }
    }
}

void AsmVisitor::visit(VarDecl *vd)
{
    assert(vd != nullptr);

    Scope *scope = vd->scope;
    assert(scope != nullptr);

    if (vd->expr)
    {
        visit(vd->expr.get());
        m_asm.appendLine({"lstore", to_string(vd->localIndex)});
    }
    else
    {
        if (vd->type->category() == TypeInfo::Category::Custom)
        {
            std::string typeName = vd->type->toString();
            Symbol *typeSymbol = scope->resolve(typeName);
            assert(typeSymbol != nullptr);

            ASTNode *astNode = typeSymbol->astNode();
            assert(astNode != nullptr);
            assert(typeSymbol->category() == Symbol::Category::Struct);

            StructDecl *sd = dynamic_cast<StructDecl *>(astNode);
            assert(sd != nullptr);

            int memberCount = static_cast<int>(sd->fieldList.size());

            m_asm.appendLine({"struct", to_string(memberCount)});
            m_asm.appendLine({"lstore", to_string(vd->localIndex)});
        }
    }
}

void AsmVisitor::visit(PropertyDecl *pd)
{
    assert(pd != nullptr);

    GroupedPropertyDecl *gpd = dynamic_cast<GroupedPropertyDecl *>(pd);
    if (gpd)
    {
        visit(gpd);
    }
    else
    {
        visit(pd->expr.get());
    }
}

void AsmVisitor::visit(GroupedPropertyDecl *gpd)
{
    assert(gpd != nullptr);

    visit(gpd->expr.get());
}

void AsmVisitor::visit(ParamDecl *pd)
{
    assert(pd != nullptr);
}

void AsmVisitor::visit(CompoundStmt *cs)
{
    assert(cs != nullptr);

    for (auto &s : cs->stmtList)
    {
        visit(s.get());
    }
}

void AsmVisitor::visit(DeclStmt *ds)
{
    assert(ds != nullptr);

    visit(ds->decl.get());
}

void AsmVisitor::visit(IfStmt *is)
{
    assert(is != nullptr);

    const string falseLabel = string(".L") + to_string(m_labelCounter++);

    visit(is->condition.get());
    m_asm.appendLine({"brf", falseLabel});
    visit(is->thenStmt.get());
    m_asm.appendLine({falseLabel});

    if (is->elseStmt)
    {
        visit(is->elseStmt.get());
    }
}

void AsmVisitor::visit(WhileStmt *ws)
{
    assert(ws != nullptr);

    const string conditionLabel = string(".L") + to_string(m_labelCounter++);
    const string endLabel = string(".L") + to_string(m_labelCounter++);

    m_breakLabels.push_back(endLabel);
    m_continueLabels.push_back(conditionLabel);

    m_asm.appendLine({conditionLabel});
    visit(ws->condition.get());
    m_asm.appendLine({"brf", endLabel});
    visit(ws->bodyStmt.get());
    m_asm.appendLine({"br", conditionLabel});
    m_asm.appendLine({endLabel});

    m_breakLabels.pop_back();
    m_continueLabels.pop_back();
}

void AsmVisitor::visit(BreakStmt *bs)
{
    assert(bs != nullptr);
    assert(m_breakLabels.size() != 0);

    m_asm.appendLine({"br", m_breakLabels.back()});
}

void AsmVisitor::visit(ContinueStmt *cs)
{
    assert(cs != nullptr);
    assert(m_continueLabels.size() != 0);

    m_asm.appendLine({"br", m_continueLabels.back()});
}

void AsmVisitor::visit(ReturnStmt *rs)
{
    assert(rs != nullptr);

    if (rs->returnExpr)
    {
        visit(rs->returnExpr.get());
    }

    m_asm.appendLine({"ret"});
}

void AsmVisitor::visit(ExprStmt *es)
{
    assert(es != nullptr);

    visit(es->expr.get());
}

void AsmVisitor::visit(FunctionDecl *fd)
{
    assert(fd != nullptr);

    setVisitingMethod(true);

    string name = fd->name;
    int args = static_cast<int>(fd->paramList.size());
    if (fd->component)
    {
        name = fd->component->name + "::" + name;
        args += 1;
    }
    int locals = fd->locals;

    m_asm.appendLine({".def", name, to_string(args), to_string(locals)});

    visit(fd->body.get());

    setVisitingMethod(false);
}

void AsmVisitor::visit(EnumConstantDecl *ecd)
{
    assert(ecd != nullptr);
}

void AsmVisitor::visit(EnumDecl *ed)
{
    assert(ed != nullptr);

    for (auto &ecd : ed->constantList)
    {
        visit(ecd.get());
    }
}

void AsmVisitor::visit(ComponentDefinationDecl *cdd)
{
    assert(cdd != nullptr);

    for (auto &e : cdd->enumList)
    {
        visit(e.get());
    }

    for (auto &m : cdd->methodList)
    {
        visit(m.get());
    }
}

void AsmVisitor::visit(FieldDecl *fd)
{
    assert(fd != nullptr);
}

void AsmVisitor::visit(StructDecl *sd)
{
    assert(sd != nullptr);
}

void AsmVisitor::visit(BindingDecl *bd)
{
    assert(bd != nullptr);

    assert(bd->instanceIndex() != -1);
    m_asm.appendLine({"lload", to_string(bd->instanceIndex())});
    visit(bd->expr.get());
    assert(bd->fieldIndex() != -1);
    m_asm.appendLine({"fstore", to_string(bd->fieldIndex())});
}

void AsmVisitor::visit(GroupedBindingDecl *gbd)
{
    assert(gbd != nullptr);
}

void AsmVisitor::visit(ComponentInstanceDecl *cid)
{
    assert(cid != nullptr);

    int instanceIndex = cid->instanceIndex;
    string componentName = cid->componentName;

    m_asm.appendLine({"lload", to_string(instanceIndex)});
    m_asm.appendLine({"call", componentName + "::draw"});
    for (auto &child : cid->childrenList)
    {
        visit(child.get());
    }
}

void AsmVisitor::genAsmForInitInstance(ComponentInstanceDecl *cid)
{
    assert(cid != nullptr);

    vector<ComponentInstanceDecl *> instances = cid->instanceList();
    for (auto instance : instances)
    {
        int instanceIndex = instance->instanceIndex;
        int fieldCount = static_cast<int>(instance->componentDefination->propertyList.size());

        m_asm.appendLine({"struct", to_string(fieldCount)});
        m_asm.appendLine({"lstore", to_string(instanceIndex)});
    }
}

void AsmVisitor::genAsmForAllMember(ComponentInstanceDecl *cid)
{
    for (auto pair : cid->orderedMemberInitList)
    {
        ComponentInstanceDecl *instance = pair.first;
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(pair.second);
        BindingDecl *bd = dynamic_cast<BindingDecl *>(pair.second);

        if ((pd == nullptr && bd == nullptr) || (pd != nullptr && bd != nullptr))
        {
            assert(false);
        }
        if (pd)
        {
            genAsmForPropertyDecl(instance, pd);
        }
        else
        {
            genAsmForBindingDecl(instance, bd);
        }
    }
}

void AsmVisitor::genAsmForPropertyDecl(ComponentInstanceDecl *cid, PropertyDecl *pd)
{
    int instanceIndex = cid->instanceIndex;
    assert(instanceIndex != -1);

    int fieldIndex = pd->fieldIndex;
    assert(fieldIndex != -1);

    m_asm.appendLine({"lload", to_string(instanceIndex)});
    setVisitingInstance(true, cid->componentDefination, cid->instanceIndex);
    visit(pd->expr.get());
    setVisitingInstance(false);
    m_asm.appendLine({"fstore", to_string(fieldIndex)});
}

void AsmVisitor::genAsmForBindingDecl(ComponentInstanceDecl *cid, BindingDecl *bd)
{
    int instanceIndex = cid->instanceIndex;
    assert(instanceIndex != -1);

    int fieldIndex = bd->fieldIndex();
    assert(fieldIndex != -1);

    m_asm.appendLine({"lload", to_string(instanceIndex)});
    setVisitingInstance(true, cid->componentDefination, cid->instanceIndex);
    visit(bd->expr.get());
    setVisitingInstance(false);
    m_asm.appendLine({"fstore", to_string(fieldIndex)});
}

void AsmVisitor::pushVisitingLvalue(bool lvalue)
{
    m_visitingLvalueStack.push_back(lvalue);
}

void AsmVisitor::popVisitingLvalue()
{
    m_visitingLvalueStack.pop_back();
    assert(m_visitingLvalueStack.size() != 0);
}

bool AsmVisitor::visitingLvalue() const
{
    return m_visitingLvalueStack.back();
}

void AsmVisitor::setVisitingInstance(bool visiting, ComponentDefinationDecl *cdd, int index)
{
    assert(visiting != m_visitingInstance);
    assert(!m_visitingMethod);
    if (visiting)
    {
        assert(cdd != nullptr);
        assert(index >= 0);
    }

    m_visitingInstance = visiting;
    m_componentVisiting = cdd;
    m_instanceIndexVisiting = index;
}

void AsmVisitor::setVisitingMethod(bool visiting)
{
    assert(visiting != m_visitingMethod);
    assert(!m_visitingInstance);

    m_visitingMethod = visiting;
}

bool AsmVisitor::visitingInstance() const
{
    return m_visitingInstance;
}

int AsmVisitor::instanceIndexVisiting() const
{
    return m_instanceIndexVisiting;
}

bool AsmVisitor::visitingMethod() const
{
    return m_visitingMethod;
}

void AsmVisitor::clear()
{
    setVisitingMethod(false);
    setVisitingInstance(false);
}
