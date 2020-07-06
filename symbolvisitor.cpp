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

#include "asmbin.h"
#include "symbolvisitor.h"
#include "structinfo.h"
#include "symboltable.h"
#include "typeinfo.h"

#include <list>

#include <assert.h>

using namespace std;

SymbolVisitor::SymbolVisitor()
{

}

void SymbolVisitor::visit(AST *ast)
{
    m_ast = ast;
    clear();

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
    m_topLevelInstance = cid;

    Scope *mainScope = new Scope(Scope::Category::Function, m_ast->symbolTable()->curScope());
    mainScope->setScopeName("main");
    m_ast->symbolTable()->pushScope(mainScope);

    visitInstanceIndex(cid);
    visitInstanceId(cid);
    visit(cid);

    calculateOrderedMemberInitList();

    m_ast->symbolTable()->popScope();
}

void SymbolVisitor::visit(StructDecl *sd)
{
    assert(sd != nullptr);

    string name = sd->name;
    Symbol *sym(new ScopeSymbol(Symbol::Category::Struct, name,
                                Scope::Category::Struct, m_ast->symbolTable()->curScope()));
    sym->setAstNode(sd);
    m_ast->symbolTable()->define(sym);

    m_ast->symbolTable()->pushScope(dynamic_cast<Scope *>(sym));
    for (size_t i = 0; i < sd->fieldList.size(); i++)
    {
        visit(sd->fieldList[i].get());
        sd->fieldList[i]->fieldIndex = static_cast<int>(i);
    }
    m_ast->symbolTable()->popScope();
}

void SymbolVisitor::visit(ComponentDefinationDecl *cdd)
{
    assert(cdd != nullptr);

    string name = cdd->name;
    Symbol *sym(new ScopeSymbol(Symbol::Category::Component, name,
                                Scope::Category::Component, m_ast->symbolTable()->curScope()));
    sym->setAstNode(cdd);
    sym->setTypeInfo(shared_ptr<TypeInfo>(new CustomTypeInfo(name)));

    m_ast->symbolTable()->define(sym);

    m_ast->symbolTable()->pushScope(dynamic_cast<Scope *>(sym));

    for (auto &e : cdd->enumList)
    {
        visit(e.get());
    }

    for (size_t i = 0; i < cdd->propertyList.size(); i++)
    {
        cdd->propertyList[i]->fieldIndex = static_cast<int>(i);
    }
    for (auto &p : cdd->propertyList)
    {
        visitPropertyDefination(p.get());
    }

    for (auto &p : cdd->propertyList)
    {
        visitPropertyInitialization(p.get());
    }

    for (auto &f : cdd->methodList)
    {
        visitMethodHeader(f.get());
    }
    for (auto &f : cdd->methodList)
    {
        visitMethodBody(f.get());
    }

    m_ast->symbolTable()->popScope();
}

void SymbolVisitor::visit(ComponentInstanceDecl *cid)
{
    assert(cid != nullptr);

    Scope *instanceScope = new Scope(Scope::Category::Instance, m_ast->symbolTable()->curScope());
    instanceScope->setScopeName(cid->instanceId);
    Symbol *componentSymbol = m_ast->symbolTable()->curScope()->resolve(cid->componentName);
    if (!componentSymbol)
    {
        throw SymbolException("ComponentInstanceDecl",
                              "No component named \"" + cid->componentName + "\"");
    }
    Scope *componentScope = dynamic_cast<Scope *>(componentSymbol);
    assert(componentScope != nullptr);
    instanceScope->setComponentScope(componentScope);

    ASTNode *componentDef = componentSymbol->astNode();
    assert(componentDef != nullptr);
    ComponentDefinationDecl *cdd = dynamic_cast<ComponentDefinationDecl *>(componentDef);
    assert(cdd != nullptr);
    cid->componentDefination = cdd;

    m_ast->symbolTable()->pushScope(instanceScope);
    if (cid->parent)
    {
        Symbol *parentSymbol = new Symbol(Symbol::Category::InstanceId, "parent",
                                          shared_ptr<TypeInfo>(new CustomTypeInfo(cid->parent->componentName)),
                                          cid->parent);
        m_ast->symbolTable()->define(parentSymbol);
    }

    pushInstanceStack(cid);
    for (auto &b : cid->bindingList)
    {
        visit(b.get());
    }
    for (auto &c : cid->childrenList)
    {
        visit(c.get());
    }
    popInstanceStack();

    m_ast->symbolTable()->popScope();
}

static string bindingId(const string &instanceId, int fieldIndex)
{
    return instanceId + "[" + to_string(fieldIndex) + "]";
}

void SymbolVisitor::calculateOrderedMemberInitList()
{
    assert(m_topLevelInstance != nullptr);

    map<string, int> id2seq;
    map<int, ASTNode *> seq2astNode;
    map<int, string> seq2id;
    map<int, ComponentInstanceDecl *> seq2instance;

    int nextSeq = 0;
    for (auto &p : m_bindingId2bindingDecl)
    {
        int seq = nextSeq++;
        string id = p.first;
        BindingDecl *astNode = p.second;
        ComponentInstanceDecl *cid = astNode->componentInstance;
        assert(cid != nullptr);

        id2seq[id] = seq;
        seq2astNode[seq] = astNode;
        seq2id[seq] = id;
        seq2instance[seq] = cid;

        util::condPrint(option::showBindingDep, "binding: seq [%d] %s(%p)\n",
                        seq,
                        id.c_str(),
                        astNode);
    }

    vector<ComponentInstanceDecl *> instances = m_topLevelInstance->instanceList();
    for (auto instance : instances)
    {
        string instanceId = instance->instanceId;
        ComponentDefinationDecl *cdd = instance->componentDefination;
        vector<int> unbound = instance->unboundProperty();
        for (int propertyIndex : unbound)
        {
            int seq = nextSeq++;
            string id = bindingId(instanceId, propertyIndex);
            ASTNode *astNode = cdd->propertyList[static_cast<size_t>(propertyIndex)].get();

            id2seq[id] = seq;
            seq2astNode[seq] = astNode;
            seq2id[seq] = id;
            seq2instance[seq] = instance;

            util::condPrint(option::showBindingDep, "binding: seq [%d] %s(%p)\n",
                            seq,
                            id.c_str(),
                            astNode);
        }
    }

    TopologicalSorter sorter(static_cast<int>(id2seq.size()));

    for (auto &p : m_bindingIdDeps)
    {
        string fromId = p.first;
        string toId = p.second;

        auto fromIter = id2seq.find(fromId);
        auto toIter = id2seq.find(toId);
        assert(fromIter != id2seq.end() && toIter != id2seq.end());

        int fromSeq = fromIter->second;
        int toSeq = toIter->second;

        sorter.addEdge(fromSeq, toSeq);
        util::condPrint(option::showBindingDep, "binding: edge %d -> %d(%s -> %s)\n",
                        fromSeq,
                        toSeq,
                        fromId.c_str(),
                        toId.c_str());
    }

    for (auto instance : instances)
    {
        string instanceId = instance->instanceId;
        ComponentDefinationDecl *cdd = instance->componentDefination;
        vector<int> unbound = instance->unboundProperty();
        for (int fromIndex : unbound)
        {
            string fromId = bindingId(instanceId, fromIndex);
            for (int toIndex : cdd->propertyDeps[fromIndex])
            {
                string toId = bindingId(instanceId, toIndex);

                auto fromIter = id2seq.find(fromId);
                auto toIter = id2seq.find(toId);
                assert(fromIter != id2seq.end() && toIter != id2seq.end());

                int fromSeq = fromIter->second;
                int toSeq = toIter->second;

                sorter.addEdge(fromSeq, toSeq);
                util::condPrint(option::showBindingDep, "binding: edge %d -> %d(%s -> %s)\n",
                                fromSeq,
                                toSeq,
                                fromId.c_str(),
                                toId.c_str());
            }
        }
    }

    vector<int> order;
    TopologicalSorter::SortResult result = sorter.sort(order);
    if (result == TopologicalSorter::SortResult::LoopDetected)
    {
        throw VisitException("BindingDecl", "Loop detected in property dependency");
    }

    for (int i = 0; i < static_cast<int>(order.size()); i++)
    {
        int seq = order[static_cast<size_t>(i)];
        string id = seq2id[seq];
        ASTNode *astNode = seq2astNode[seq];
        ComponentInstanceDecl *cid = seq2instance[seq];
        m_topLevelInstance->orderedMemberInitList.push_back(make_pair(cid, astNode));
        util::condPrint(option::showBindingDep, "binding: order [%d] [%d] %s(%p)\n",
                        i,
                        seq,
                        id.c_str(),
                        astNode);
    }
}

void SymbolVisitor::visit(BindingDecl *bd)
{
    assert(bd != nullptr);

    GroupedBindingDecl *gbd = dynamic_cast<GroupedBindingDecl *>(bd);
    if (gbd)
    {
        visit(gbd);
    }
    else
    {
        if (bd->isId())
        {
            return;
        }

        Scope *componentScope = m_ast->symbolTable()->curScope()->componentScope();
        assert(componentScope != nullptr);
        Symbol *componentSymbol = dynamic_cast<Symbol *>(componentScope);
        assert(componentSymbol != nullptr);
        Symbol *propertySymbol = componentScope->resolve(bd->name);
        if (!propertySymbol)
        {
            throw SymbolException("BindingDecl",
                                  "Component \"" + componentSymbol->name() + "\"" +
                                  " has no property named \"" + bd->name + "\"");
        }
        ASTNode *astNode = propertySymbol->astNode();
        assert(astNode != nullptr);
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(astNode);
        assert(pd != nullptr);

        bd->propertyDecl = pd;

        setAnalyzingBindingDep(true, bd);
        m_bindingId2bindingDecl[bindingId(curInstanceId(), bindingIndexAnalyzing())] = bd;
        visit(bd->expr.get());
        setAnalyzingBindingDep(false);
    }
}

void SymbolVisitor::visit(GroupedBindingDecl *gbd)
{
    assert(gbd != nullptr);
}

int SymbolVisitor::visitInstanceIndex(ComponentInstanceDecl *cid)
{
    assert(cid != nullptr);

    cid->scope = m_ast->symbolTable()->curScope();
    assert(cid->scope != nullptr);

    cid->instanceIndex = m_nextInstanceIndex++;
    cid->instanceTreeSize = 1;

    for (auto &d : cid->childrenList)
    {
        cid->instanceTreeSize += visitInstanceIndex(d.get());
    }
    return cid->instanceTreeSize;
}

static std::string getId(ComponentInstanceDecl *cid)
{
    std::string id;
    for (auto &b : cid->bindingList)
    {
        if (b->name == "id")
        {
            RefExpr *re = dynamic_cast<RefExpr *>(b->expr.get());
            if (!re)
            {
                throw SymbolException("ComponentInstanceDecl", "Instance id should be a valid identifier");
            }
            id = re->name;
            break;
        }
    }
    return id;
}

void SymbolVisitor::visitInstanceId(ComponentInstanceDecl *cid)
{
    assert(cid != nullptr);

    // instance id is global.
    // we define instance id in the virtual "main" function scope.
    // then we will define "parent" in instance scope.

    Scope *mainScope = cid->scope;

    string id = getId(cid);
    if (id == "")
    {
        id = "#" + to_string(cid->instanceIndex);
    }

    cid->instanceId = id;
    Symbol *symbol = new Symbol(Symbol::Category::InstanceId, id, shared_ptr<TypeInfo>(new CustomTypeInfo(cid->componentName)), cid);
    mainScope->define(symbol);

    for (auto &c : cid->childrenList)
    {
        visitInstanceId(c.get());
    }
}

void SymbolVisitor::visit(IntegerLiteral *e)
{
    assert(e != nullptr);

    e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Int));
}

void SymbolVisitor::visit(FloatLiteral *e)
{
    assert(e != nullptr);

    e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Float));
}

void SymbolVisitor::visit(StringLiteral *e)
{
    assert(e != nullptr);

    e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::String));
}

void SymbolVisitor::visit(InitListExpr *ile)
{
    assert(ile != nullptr);

    shared_ptr<TypeInfo> eType;
    if (ile->exprList.size() == 0)
    {
        eType.reset(new TypeInfo(TypeInfo::Category::Void));
    }
    else
    {
        for (size_t i = 0; i < ile->exprList.size(); i++)
        {
            visit(ile->exprList[i].get());
        }

        eType = ile->exprList[0]->typeInfo;
        for (size_t i = 1; i < ile->exprList.size(); i++)
        {
            if (*eType != *(ile->exprList[i]->typeInfo))
            {
                throw VisitException("InitListExpr",
                                     "Elements of InitListExpr must have same type");
            }
        }
    }

    ile->typeInfo = shared_ptr<ListTypeInfo>(new ListTypeInfo(eType));
}

void SymbolVisitor::visit(BinaryOperatorExpr *b)
{
    assert(b != nullptr);

    visit(b->left.get());
    visit(b->right.get());

    shared_ptr<TypeInfo> leftType = b->left->typeInfo;
    shared_ptr<TypeInfo> rightType = b->right->typeInfo;

    switch (b->op)
    {
    case BinaryOperatorExpr::Op::LogicalAnd:
    case BinaryOperatorExpr::Op::LogicalOr:
    {
        if (!(leftType->category() == TypeInfo::Category::Int && rightType->category() == TypeInfo::Category::Int))
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'&&'/'||' operator require int operand");
        }
        b->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Int));
        break;
    }
    case BinaryOperatorExpr::Op::LessThan:
    case BinaryOperatorExpr::Op::GreaterThan:
    case BinaryOperatorExpr::Op::LessEqual:
    case BinaryOperatorExpr::Op::GreaterEqual:
    {
        if (*leftType != *rightType)
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'<'/'>'/'<='/'>=' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float)
        {
            // good
        }
        else
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'<'/'>'/'<='/'>=' operator requires type of operands is int/float");
        }
        b->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Int));
        break;
    }
    case BinaryOperatorExpr::Op::Equal:
    case BinaryOperatorExpr::Op::NotEqual:
    {
        if (*leftType != *rightType)
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'=='/'!=' operator requires type of operands is same("
                                 + leftType->toString() + ", "
                                 + rightType->toString() + ")");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float || cat == TypeInfo::Category::String)
        {
            // good
        }
        else
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'=='/'!=' operator requires type of operands is int/float/string");
        }
        b->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Int));
        break;
    }
    case BinaryOperatorExpr::Op::Plus:
    {
        if (*leftType != *rightType)
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'+' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float || cat == TypeInfo::Category::String)
        {
            // good
        }
        else
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'+' operator requires type of operands is int/float/string");
        }
        b->typeInfo = leftType;
        break;
    }
    case BinaryOperatorExpr::Op::Minus:
    case BinaryOperatorExpr::Op::Multiply:
    case BinaryOperatorExpr::Op::Divide:
    {
        if (*leftType != *rightType)
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'-'/'*'/'/' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float)
        {
            // good
        }
        else
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'-'/'*'/'/' operator requires type of operands is int/float");
        }
        b->typeInfo = leftType;
        break;
    }
    case BinaryOperatorExpr::Op::Remainder:
    {
        if (*leftType != *rightType)
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'%' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int)
        {
            // good
        }
        else
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'%' operator requires type of operands is int");
        }
        b->typeInfo = leftType;
        break;
    }
    case BinaryOperatorExpr::Op::Assign:
    {
        if (*leftType != *rightType)
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'=' operator requires type of operands is same("
                                 + leftType->toString() + ", "
                                 + rightType->toString() + ")");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float || cat == TypeInfo::Category::String)
        {
            // good
        }
        else
        {
            throw VisitException("BinaryOperatorExpr",
                                 "'=' operator requires type of operands is int/float/string");
        }
        b->typeInfo = leftType;
        break;
    }
    case BinaryOperatorExpr::Op::Invalid:
    {
        assert(false);
        break;
    }
    }
}

void SymbolVisitor::visit(UnaryOperatorExpr *u)
{
    assert(u != nullptr);
    visit(u->expr.get());

    shared_ptr<TypeInfo> eType = u->expr->typeInfo;
    switch (u->op)
    {
    case UnaryOperatorExpr::Op::Negative:
    case UnaryOperatorExpr::Op::Positive:
    {
        if (eType->category() == TypeInfo::Category::Int
                || eType->category() == TypeInfo::Category::Float)
        {
            u->typeInfo = eType;
        }
        else
        {
            throw VisitException("UnaryOperatorExpr",
                                 "Unary operator '-'/'+' can only used for int/float expr");
        }
        break;
    }
    case UnaryOperatorExpr::Op::Not:
    {
        if (eType->category() == TypeInfo::Category::Int)
        {
            u->typeInfo = eType;
        }
        else
        {
            throw VisitException("UnaryOperatorExpr",
                                 "Unary operator '!' can only used for int expr");
        }
        break;
    }
    case UnaryOperatorExpr::Op::Invalid:
    {
        assert(false);
        break;
    }
    }
}

void SymbolVisitor::visit(CallExpr *e)
{
    assert(e != nullptr);

    e->scope = m_ast->symbolTable()->curScope();

    if (m_ast->symbolTable()->curScope()->category() != Scope::Category::Local)
    {
        throw VisitException("CallExpr",
                             "CallExpr can only be used in LocalScope");
    }

    string functionName;
    vector<shared_ptr<TypeInfo>> paramTypes;

    if (e->funcExpr->category == Expr::Category::Ref)
    {
        RefExpr *r = dynamic_cast<RefExpr *>(e->funcExpr.get());
        assert(r != nullptr);
        visit(r);

        Symbol *func = m_ast->symbolTable()->curScope()->resolve(r->name);
        if (!func)
        {
            throw VisitException("CallExpr", "No symbol: " + r->name);
        }

        if (func->category() == Symbol::Category::Function)
        {
            FunctionSymbol *funcSymbol = dynamic_cast<FunctionSymbol *>(func);
            assert(funcSymbol);
            functionName = funcSymbol->name();
            paramTypes = funcSymbol->paramTypes();
        }
        else if (func->category() == Symbol::Category::Method)
        {
            MethodSymbol *methodSymbol = dynamic_cast<MethodSymbol *>(func);
            assert(methodSymbol);
            functionName = methodSymbol->name();
            paramTypes = methodSymbol->paramTypes();
        }
        else
        {
            throw VisitException("CallExpr",
                                 r->name
                                 + " is a "
                                 + Symbol::symbolCategoryString(func->category()));
        }

        util::condPrint(option::showSymbolRef, "ref: %s\n", func->symbolString().c_str());
        e->funcExpr->typeInfo = func->typeInfo();
    }
    else if (e->funcExpr->category == Expr::Category::Member)
    {
        MemberExpr *m = dynamic_cast<MemberExpr *>(e->funcExpr.get());
        assert(m != nullptr);

        visit(m->instanceExpr.get());
        shared_ptr<TypeInfo> instanceType = m->instanceExpr->typeInfo;
        string typeName = instanceType->toString();

        Symbol *instanceTypeSymbol = m_ast->symbolTable()->curScope()->resolve(typeName);
        if (!instanceTypeSymbol)
        {
            throw VisitException("CallExpr", "No symbol for typeName: " + typeName);
        }
        ScopeSymbol *scopeSym = dynamic_cast<ScopeSymbol *>(instanceTypeSymbol);
        if (!scopeSym)
        {
            throw VisitException("CallExpr", typeName + " is a "
                                 + Symbol::symbolCategoryString(instanceTypeSymbol->category())
                                 + ", doesn't contains method");
        }
        Symbol *method = scopeSym->resolve(m->name);
        if (!method)
        {
            throw VisitException("CallExpr", typeName + " doesn't contains method named " + m->name);
        }

        if (method->category() == Symbol::Category::Method)
        {
            MethodSymbol *methodSymbol = dynamic_cast<MethodSymbol *>(method);
            assert(methodSymbol);
            functionName = methodSymbol->name();
            paramTypes = methodSymbol->paramTypes();
        }
        else
        {
            throw VisitException("CallExpr",
                                 m->name
                                 + " is a "
                                 + Symbol::symbolCategoryString(method->category()));
        }

        util::condPrint(option::showSymbolRef, "ref: %s\n", method->symbolString().c_str());
        e->funcExpr->typeInfo = method->typeInfo();
    }
    else
    {
        throw VisitException("CallExpr", "Only f(...) and obj.f(...) is valid");
    }

    for (auto &p : e->paramList)
    {
        visit(p.get());
    }

    if (paramTypes.size() != e->paramList.size())
    {
        char buf[512];
        snprintf(buf, sizeof(buf), "%s requires %d parameters but is passed %d parameters",
                 functionName.c_str(),
                 static_cast<int>(paramTypes.size()),
                 static_cast<int>(e->paramList.size()));
        throw VisitException("CallExpr", string(buf));
    }

    if (functionName == "len"
            || functionName == "print"
            || functionName == "drawRect"
            || functionName == "drawText"
            || functionName == "drawPt")
    {
    }
    else
    {
        for (size_t i = 0; i < paramTypes.size(); i++)
        {
            if (*paramTypes[i] != *(e->paramList[i]->typeInfo))
            {
                char buf[512];
                snprintf(buf, sizeof(buf), "%s requires %s in %dth parameters but is passed %s",
                         functionName.c_str(),
                         paramTypes[i]->toString().c_str(),
                         static_cast<int>(i),
                         e->paramList[i]->typeInfo->toString().c_str());
                throw VisitException("CallExpr", string(buf));
            }
        }
    }

    e->typeInfo = e->funcExpr->typeInfo;
}

void SymbolVisitor::visit(ListSubscriptExpr *e)
{
    assert(e != nullptr);

    visit(e->listExpr.get());
    if (e->listExpr->typeInfo->category() != TypeInfo::Category::List)
    {
        throw VisitException("ListSubscriptExpr", "Type of listExpr is " + e->listExpr->typeInfo->toString());
    }

    visit(e->indexExpr.get());
    if (e->indexExpr->typeInfo->category() != TypeInfo::Category::Int)
    {
        throw VisitException("ListSubscriptExpr", "Type of indexExpr is " + e->indexExpr->typeInfo->toString());
    }

    shared_ptr<TypeInfo> ti = e->listExpr->typeInfo;
    shared_ptr<ListTypeInfo> lti = dynamic_pointer_cast<ListTypeInfo>(ti);
    assert(lti != nullptr);

    e->typeInfo = lti->elementType();
}

void SymbolVisitor::visit(MemberExpr *e)
{
    assert(e != nullptr);

    e->scope = m_ast->symbolTable()->curScope();

    visit(e->instanceExpr.get());

    shared_ptr<TypeInfo> instanceTypeInfo = e->instanceExpr->typeInfo;
    string typeString = instanceTypeInfo->toString();

    Symbol *instanceTypeSymbol;
    if (instanceTypeInfo->category() == TypeInfo::Category::Group)
    {
        shared_ptr<GroupTypeInfo> groupTypeInfo = dynamic_pointer_cast<GroupTypeInfo>(instanceTypeInfo);
        assert(groupTypeInfo != nullptr);

        string componentName = groupTypeInfo->componentType()->toString();
        string groupName = groupTypeInfo->name();

        Symbol *componentSym = m_ast->symbolTable()->curScope()->resolve(componentName);
        if (!componentSym)
        {
            throw VisitException("MemberExpr",
                                 "No symbol for componentName " + componentName);
        }
        if (componentSym->category() != Symbol::Category::Component)
        {
            throw VisitException("MemberExpr",
                                 componentName + " is not a component symbol");
        }
        ScopeSymbol *componentScope = dynamic_cast<ScopeSymbol *>(componentSym);
        assert(componentScope != nullptr);

        Symbol *groupSym = componentScope->resolve(groupName);
        if (!groupSym)
        {
            throw VisitException("MemberExpr", componentName + " doesn't contains group named " + groupName);
        }

        instanceTypeSymbol = groupSym;
    }
    else if (instanceTypeInfo->category() == TypeInfo::Category::Custom)
    {
        instanceTypeSymbol = m_ast->symbolTable()->curScope()->resolve(typeString);
        if (!instanceTypeSymbol)
        {
            throw VisitException("MemberExpr", "No symbol for type " + typeString);
        }
    }
    else
    {
        throw VisitException("MemberExpr", instanceTypeInfo->toString() + " doesn't contains member");
    }

    ScopeSymbol *scopeSym = dynamic_cast<ScopeSymbol *>(instanceTypeSymbol);
    if (!scopeSym)
    {
        throw VisitException("MemberExpr", typeString + " is a "
                             + Symbol::symbolCategoryString(instanceTypeSymbol->category())
                             + ", doesn't contains member");
    }

    Symbol *memberSymbol = scopeSym->resolve(e->name);
    if (!memberSymbol)
    {
        throw VisitException("MemberExpr", typeString + " doesn't contains member named " + e->name);
    }

    util::condPrint(option::showSymbolRef, "ref: %s\n", memberSymbol->symbolString().c_str());
    e->typeInfo = memberSymbol->typeInfo();

    if (memberSymbol->category() == Symbol::Category::Property && analyzingPropertyDep())
    {
        ASTNode *ast = memberSymbol->astNode();
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
        assert(pd != nullptr);

        componentDefinationAnalyzing()->propertyDeps[propertyIndexAnalyzing()].insert(pd->fieldIndex);
        util::condPrint(option::showPropertyDep, "property: [%d] -> [%d]\n", propertyIndexAnalyzing(), pd->fieldIndex);
    }

    if (analyzingBindingDep())
    {
        if (memberSymbol->category() == Symbol::Category::Property)
        {
            ASTNode *ast = memberSymbol->astNode();
            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
            assert(pd != nullptr);

            util::condPrint(option::showBindingDep, "binding: %s[%d](%p) -> %s[%d]\n",
                            curInstanceId().c_str(),
                            bindingIndexAnalyzing(),
                            bindingAnalyzing(),
                            m_curAnalyzingBindingToId.c_str(),
                            pd->fieldIndex);
            m_bindingIdDeps.push_back(pair<string, string>(bindingId(curInstanceId(), bindingIndexAnalyzing()),
                                                           bindingId(m_curAnalyzingBindingToId, pd->fieldIndex)));
        }
    }
}

void SymbolVisitor::visit(RefExpr *e)
{
    assert(e != nullptr);

    e->scope = m_ast->symbolTable()->curScope();

    Symbol *sym = m_ast->symbolTable()->curScope()->resolve(e->name);
    if (!sym)
    {
        throw VisitException("RefExpr", "No symbol named " + e->name);
    }

    util::condPrint(option::showSymbolRef, "ref: %s\n", sym->symbolString().c_str());
    e->typeInfo = sym->typeInfo();

    if (sym->category() == Symbol::Category::Property && analyzingPropertyDep())
    {
        ASTNode *ast = sym->astNode();
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
        assert(pd != nullptr);

        componentDefinationAnalyzing()->propertyDeps[propertyIndexAnalyzing()].insert(pd->fieldIndex);
        util::condPrint(option::showPropertyDep, "property: [%d] -> [%d]\n", propertyIndexAnalyzing(), pd->fieldIndex);
    }

    if (analyzingBindingDep())
    {
        if (sym->category() == Symbol::Category::Property)
        {
            ASTNode *ast = sym->astNode();
            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
            assert(pd != nullptr);

            util::condPrint(option::showBindingDep, "binding: %s[%d](%p) -> %s[%d]\n",
                            curInstanceId().c_str(),
                            bindingIndexAnalyzing(),
                            bindingAnalyzing(),
                            curInstanceId().c_str(),
                            pd->fieldIndex);
            m_bindingIdDeps.push_back(pair<string, string>(bindingId(curInstanceId(), bindingIndexAnalyzing()),
                                                           bindingId(curInstanceId(), pd->fieldIndex)));
        }
        else if (sym->category() == Symbol::Category::InstanceId)
        {
            ASTNode *ast = sym->astNode();
            ComponentInstanceDecl *cid = dynamic_cast<ComponentInstanceDecl *>(ast);
            assert(cid != nullptr);

            m_curAnalyzingBindingToId = cid->instanceId;
        }
        else if (sym->category() == Symbol::Category::PropertyGroup)
        {
            m_curAnalyzingBindingToId = curInstanceId();
        }
    }
}

void SymbolVisitor::visit(VarDecl *vd)
{
    assert(vd != nullptr);

    vd->scope = m_ast->symbolTable()->curScope();

    Symbol *variableSym = new Symbol(Symbol::Category::Variable, vd->name, vd->type, vd);
    m_ast->symbolTable()->define(variableSym);

    if (vd->type->category() == TypeInfo::Category::Custom)
    {
        Symbol *typeSymbol = m_ast->symbolTable()->curScope()->resolve(vd->type->toString());
        if (!typeSymbol)
        {
            throw SymbolException("VarDecl", "No such type \"" + vd->type->toString() + "\"");
        }
        switch (typeSymbol->category())
        {
        case Symbol::Category::Struct:
            break;
        case Symbol::Category::Enum:
            throw SymbolException("VarDecl", "Please use int type instead of enum type");
        case Symbol::Category::Component:
            throw SymbolException("VarDecl", "Define variable with component type is illegal");
        default:
            throw SymbolException("VarDecl", "\"" + typeSymbol->name() + "\" is a " + Symbol::symbolCategoryString(typeSymbol->category()));
        }
    }

    vd->localIndex = m_stackFrameLocals;
    m_stackFrameLocals++;

    util::condPrint(option::showGenAsm, "genAsm: localIndex [%d] %s\n",
                    vd->localIndex,
                    vd->name.c_str());

    if (vd->expr)
    {
        visit(vd->expr.get());
        if (!(*vd->type == *vd->expr->typeInfo) && !vd->type->assignCompatible(vd->expr->typeInfo))
        {
            throw VisitException("VarDecl", "Type doesn't match("
                                 + vd->type->toString()
                                 + ", "
                                 + vd->expr->typeInfo->toString()
                                 + ")");
        }
    }
}

void SymbolVisitor::visit(FieldDecl *md)
{
    assert(md != nullptr);

    string name = md->name;
    Symbol *sym = new Symbol(Symbol::Category::Field, name, md->type, md);
    m_ast->symbolTable()->define(sym);
}

void SymbolVisitor::visitPropertyDefination(PropertyDecl *pd)
{
    assert(pd != nullptr);
    GroupedPropertyDecl *gpd = dynamic_cast<GroupedPropertyDecl *>(pd);
    if (gpd)
    {
        visitPropertyDefination(gpd);
    }
    else
    {
        string propertyName = pd->name;
        Symbol *propertySym = new Symbol(Symbol::Category::Property, propertyName, pd->type, pd);
        m_ast->symbolTable()->define(propertySym);

        util::condPrint(option::showPropertyDep, "property: [%d] %s\n", pd->fieldIndex, propertySym->symbolString().c_str());
    }
}

void SymbolVisitor::visitPropertyDefination(GroupedPropertyDecl *gpd)
{
    assert(gpd != nullptr);

    string groupName = gpd->groupName;
    string propertyName = gpd->name;

    Symbol *group = m_ast->symbolTable()->curScope()->resolve(groupName);
    if (group)
    {
        // the group is already defined
        if (group->category() == Symbol::Category::PropertyGroup)
        {
            // the group is defined as a PropertyGroup, that's good
            ScopeSymbol *scopeSym = dynamic_cast<ScopeSymbol *>(group);
            assert(scopeSym != nullptr);
            util::condPrint(option::showSymbolRef, "ref: %s\n", scopeSym->symbolString().c_str());

            Symbol *propertySym = new Symbol(Symbol::Category::Property, propertyName, gpd->type, gpd);
            scopeSym->define(propertySym);

            util::condPrint(option::showPropertyDep, "property: [%d] %s\n", gpd->fieldIndex, propertySym->symbolString().c_str());
        }
        else
        {
            // the group is defined as a non-PropertyGroup, that's bad
            throw VisitException("GroupedPropertyDecl",
                                 group->name() + " is already defined as "
                                 + group->symbolString());
        }
    }
    else
    {
        // the group is not defined, define it
        ScopeSymbol *componentSym = dynamic_cast<ScopeSymbol *>(m_ast->symbolTable()->curScope());
        shared_ptr<TypeInfo> customType(new CustomTypeInfo(componentSym->name()));

        Symbol *groupSym(new ScopeSymbol(Symbol::Category::PropertyGroup,
                                         groupName,
                                         Scope::Category::Group,
                                         m_ast->symbolTable()->curScope(),
                                         shared_ptr<TypeInfo>(new GroupTypeInfo(groupName, customType))));
        m_ast->symbolTable()->define(groupSym);

        Symbol *propertySym = new Symbol(Symbol::Category::Property, propertyName, gpd->type, gpd);
        dynamic_cast<ScopeSymbol *>(groupSym)->define(propertySym);

        util::condPrint(option::showPropertyDep, "property [%d] %s\n", gpd->fieldIndex, propertySym->symbolString().c_str());
    }
}

void SymbolVisitor::visitPropertyInitialization(PropertyDecl *pd)
{
    assert(pd != nullptr);

    setAnalyzingPropertyDep(true, pd);

    GroupedPropertyDecl *gpd = dynamic_cast<GroupedPropertyDecl *>(pd);
    if (gpd)
    {
        visitPropertyInitialization(gpd);
    }
    else
    {
        visit(pd->expr.get());
    }

    setAnalyzingPropertyDep(false);
}

void SymbolVisitor::visitPropertyInitialization(GroupedPropertyDecl *gpd)
{
    assert(gpd != nullptr);

    visit(gpd->expr.get());
}

void SymbolVisitor::visit(ParamDecl *pd)
{
    assert(pd != nullptr);

    Symbol *paramSym = new Symbol(Symbol::Category::Parameter, pd->name, pd->type, pd);
    m_ast->symbolTable()->define(paramSym);
}

void SymbolVisitor::visit(CompoundStmt *cs)
{
    assert(cs != nullptr);

    Scope *localScope(new Scope(Scope::Category::Local, m_ast->symbolTable()->curScope()));
    m_ast->symbolTable()->pushScope(localScope);

    for (auto &s : cs->stmtList)
    {
        visit(s.get());
    }

    m_ast->symbolTable()->popScope();
}

void SymbolVisitor::visit(DeclStmt *ds)
{
    assert(ds != nullptr);

    visit(ds->decl.get());
}

void SymbolVisitor::visit(IfStmt *is)
{
    assert(is != nullptr);

    visit(is->condition.get());
    if (is->condition->typeInfo->category() != TypeInfo::Category::Int)
    {
        throw VisitException("IfStmt", "if statement requires type of condition expression is int, but actually "
                             + is->condition->typeInfo->toString());
    }
    visit(is->thenStmt.get());
    if (is->elseStmt)
    {
        visit(is->elseStmt.get());
    }
}

void SymbolVisitor::visit(WhileStmt *ws)
{
    assert(ws != nullptr);

    visit(ws->condition.get());
    visit(ws->bodyStmt.get());
}

void SymbolVisitor::visit(BreakStmt *bs)
{
    assert(bs != nullptr);
}

void SymbolVisitor::visit(ContinueStmt *cs)
{
    assert(cs != nullptr);
}

void SymbolVisitor::visit(ReturnStmt *rs)
{
    assert(rs != nullptr);

    if (rs->returnExpr)
    {
        visit(rs->returnExpr.get());
    }
}

void SymbolVisitor::visit(ExprStmt *es)
{
    assert(es != nullptr);

    visit(es->expr.get());
}

void SymbolVisitor::visitMethodHeader(FunctionDecl *fd)
{
    assert(fd != nullptr);

    fd->scope = m_ast->symbolTable()->curScope();

    Scope *componentScope = m_ast->symbolTable()->curScope();
    Symbol *componentSymbol = dynamic_cast<Symbol *>(componentScope);
    assert(componentSymbol);

    vector<shared_ptr<TypeInfo>> paramTypes;
    for (auto &p : fd->paramList)
    {
        paramTypes.push_back(p->type);
    }

    Symbol *methodSym(new MethodSymbol(fd->name, fd->returnType, componentSymbol, paramTypes));
    m_ast->symbolTable()->define(methodSym);
}

void SymbolVisitor::visitMethodBody(FunctionDecl *fd)
{
    assert(fd != nullptr);

    Scope *methodScope(new Scope(Scope::Category::Method, m_ast->symbolTable()->curScope()));
    methodScope->setScopeName(fd->name);
    m_ast->symbolTable()->pushScope(methodScope);

    for (size_t i = 0; i < fd->paramList.size(); i++)
    {
        visit(fd->paramList[i].get());
        fd->paramList[i]->localIndex = static_cast<int>(i);
    }

    int args = static_cast<int>(fd->paramList.size());
    if (fd->component)
    {
        args += 1;
        for (size_t i = 0; i < fd->paramList.size(); i++)
        {
            fd->paramList[i]->localIndex += 1;
        }
    }

    for (size_t i = 0; i < fd->paramList.size(); i++)
    {
        util::condPrint(option::showGenAsm, "genAsm: localIndex [%d] %s\n",
                        fd->paramList[i]->localIndex,
                        fd->paramList[i]->name.c_str());
    }

    m_stackFrameLocals = args;
    visit(fd->body.get());

    int locals = m_stackFrameLocals - args;
    fd->locals = locals;

    m_ast->symbolTable()->popScope();
}

void SymbolVisitor::visit(EnumConstantDecl *ecd)
{
    assert(ecd != nullptr);

    Symbol *enumConstantSym = new Symbol(Symbol::Category::EnumConstants, ecd->name, make_shared<TypeInfo>(TypeInfo::Category::Int), ecd);
    m_ast->symbolTable()->define(enumConstantSym);
}

void SymbolVisitor::visit(EnumDecl *ed)
{
    assert(ed != nullptr);

    Symbol *enumSym = new Symbol(Symbol::Category::Enum, ed->name, std::shared_ptr<TypeInfo>(), ed);
    m_ast->symbolTable()->define(enumSym);

    for (size_t i = 0; i < ed->constantList.size(); i++)
    {
        ed->constantList[i]->value = static_cast<int>(i);
        visit(ed->constantList[i].get());
    }
}

void SymbolVisitor::visit(FunctionDecl *)
{
    throw VisitException("FunctionDecl", "Not implement");
}

void SymbolVisitor::visit(PropertyDecl *)
{
    throw VisitException("PropertyDecl", "Not implement");
}

void SymbolVisitor::visit(GroupedPropertyDecl *)
{
    throw VisitException("GroupedPropertyDecl", "Not implement");
}

void SymbolVisitor::clear()
{
    Scope::resetNextScopeId();
    m_stackFrameLocals = -1;
    m_nextInstanceIndex = 0;
    m_topLevelInstance = nullptr;
    m_instanceStack.clear();
    m_propertyIndexAnalyzing = -1;
    m_propertyAnalyzing = nullptr;
    m_bindingAnalyzing = nullptr;
}

void SymbolVisitor::setAnalyzingPropertyDep(bool analyzing, PropertyDecl *pd)
{
    assert(analyzing != m_analyzingPropertyDep);
    if (analyzing)
    {
        assert(pd->fieldIndex >= 0);

        m_propertyIndexAnalyzing = pd->fieldIndex;
        m_propertyAnalyzing = pd;
    }
    else
    {
        m_propertyIndexAnalyzing = -1;
        m_propertyAnalyzing = nullptr;
    }

    m_analyzingPropertyDep = analyzing;
}

bool SymbolVisitor::analyzingPropertyDep() const
{
    return m_analyzingPropertyDep;
}

int SymbolVisitor::propertyIndexAnalyzing() const
{
    assert(m_propertyIndexAnalyzing != -1);

    return m_propertyIndexAnalyzing;
}

PropertyDecl *SymbolVisitor::propertyAnalyzing() const
{
    assert(m_propertyAnalyzing != nullptr);

    return m_propertyAnalyzing;
}

ComponentDefinationDecl *SymbolVisitor::componentDefinationAnalyzing() const
{
    assert(m_propertyAnalyzing != nullptr);
    assert(m_propertyAnalyzing->componentDefination != nullptr);

    return m_propertyAnalyzing->componentDefination;
}

void SymbolVisitor::setAnalyzingBindingDep(bool analyzing, BindingDecl *bd)
{
    assert(analyzing != m_analyzingBindingDep);
    if (analyzing)
    {
        assert(bd != nullptr);
        m_bindingAnalyzing = bd;
    }
    else
    {
        m_bindingAnalyzing = nullptr;
    }

    m_analyzingBindingDep = analyzing;
}

bool SymbolVisitor::analyzingBindingDep() const
{
    return m_analyzingBindingDep;
}

int SymbolVisitor::bindingIndexAnalyzing() const
{
    assert(m_bindingAnalyzing != nullptr);
    assert(m_bindingAnalyzing->fieldIndex() != -1);

    return m_bindingAnalyzing->fieldIndex();
}

BindingDecl *SymbolVisitor::bindingAnalyzing() const
{
    assert(m_bindingAnalyzing != nullptr);

    return m_bindingAnalyzing;
}

void SymbolVisitor::pushInstanceStack(ComponentInstanceDecl *cid)
{
    assert(cid != nullptr);

    m_instanceStack.push_back(cid);
}

void SymbolVisitor::popInstanceStack()
{
    assert(m_instanceStack.size() != 0);

    m_instanceStack.pop_back();
}

ComponentInstanceDecl *SymbolVisitor::curInstance() const
{
    assert(m_instanceStack.size() != 0);

    return m_instanceStack.back();
}

string SymbolVisitor::curInstanceId() const
{
    assert(m_instanceStack.size() != 0);

    return m_instanceStack.back()->instanceId;
}
