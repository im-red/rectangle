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

#include "symbolvisitor.h"

#include <assert.h>

using namespace std;

constexpr int BUF_LEN = 512;

SymbolVisitor::SymbolVisitor()
{

}

void SymbolVisitor::setDocuments(std::vector<DocumentDecl *> documents)
{
    m_documents = documents;

    m_scopes.clear();
    m_symbols.clear();
    m_curScope.reset();
    Scope::resetNextScopeId();

    initGlobalScope();
}

std::shared_ptr<Scope> SymbolVisitor::curScope()
{
    return m_curScope;
}

void SymbolVisitor::pushScope(const std::shared_ptr<Scope> &scope)
{
    m_curScope = scope;
    util::condPrint(option::showScopeStack, "pushScope: %p(%s)\n",
                     static_cast<void *>(m_curScope.get()),
                     m_curScope->scopeString().c_str());
}

void SymbolVisitor::popScope()
{
    util::condPrint(option::showScopeStack, "popScope: %p(%s)\n",
                     static_cast<void *>(m_curScope.get()),
                     m_curScope->scopeString().c_str());
    m_curScope = m_curScope->parent();
}

void SymbolVisitor::initGlobalScope()
{
    shared_ptr<Scope> globalScope(new Scope(Scope::Category::Global, nullptr));
    pushScope(globalScope);

    {
        shared_ptr<Symbol> rect(new ScopeSymbol(Symbol::Category::Struct, "rect", Scope::Category::Struct, curScope()));
        pushScope(dynamic_pointer_cast<Scope>(rect));

        shared_ptr<Symbol> x(new Symbol(Symbol::Category::Member, "x", make_shared<TypeInfo>(TypeInfo::Category::Int)));
        shared_ptr<Symbol> y(new Symbol(Symbol::Category::Member, "y", make_shared<TypeInfo>(TypeInfo::Category::Int)));
        shared_ptr<Symbol> width(new Symbol(Symbol::Category::Member, "width", make_shared<TypeInfo>(TypeInfo::Category::Int)));
        shared_ptr<Symbol> height(new Symbol(Symbol::Category::Member, "height", make_shared<TypeInfo>(TypeInfo::Category::Int)));
        shared_ptr<Symbol> fill_color(new Symbol(Symbol::Category::Member, "fill_color", make_shared<TypeInfo>(TypeInfo::Category::String)));
        shared_ptr<Symbol> stroke_color(new Symbol(Symbol::Category::Member, "stroke_color", make_shared<TypeInfo>(TypeInfo::Category::String)));
        shared_ptr<Symbol> stroke_width(new Symbol(Symbol::Category::Member, "stroke_width", make_shared<TypeInfo>(TypeInfo::Category::Int)));
        shared_ptr<Symbol> stroke_dasharray(new Symbol(Symbol::Category::Member, "stroke_dasharray", make_shared<TypeInfo>(TypeInfo::Category::String)));

        curScope()->define(x);
        curScope()->define(y);
        curScope()->define(width);
        curScope()->define(height);
        curScope()->define(fill_color);
        curScope()->define(stroke_color);
        curScope()->define(stroke_width);
        curScope()->define(stroke_dasharray);

        popScope();
        curScope()->define(rect);
    }

    {
        shared_ptr<TypeInfo> stringType(new TypeInfo(TypeInfo::Category::String));
        vector<shared_ptr<TypeInfo>> paramTypes(1, stringType);
        shared_ptr<Symbol> printString(new FunctionSymbol("printString", make_shared<TypeInfo>(TypeInfo::Category::Void), paramTypes));
        curScope()->define(printString);
    }

    {
        shared_ptr<TypeInfo> rectType(new CustomTypeInfo("rect"));
        vector<shared_ptr<TypeInfo>> paramTypes(1, rectType);
        shared_ptr<Symbol> drawRect(new FunctionSymbol("drawRect", make_shared<TypeInfo>(TypeInfo::Category::Void), paramTypes));
        curScope()->define(drawRect);
    }
}

void SymbolVisitor::visit()
{
    for (auto doc : m_documents)
    {
        if (doc && doc->type == DocumentDecl::Type::Defination)
        {
            visit(doc);
        }
    }
    for (auto doc : m_documents)
    {
        if (doc && doc->type == DocumentDecl::Type::Instance)
        {
            visit(doc);
        }
    }
}

void SymbolVisitor::visit(DocumentDecl *dd)
{
    if (dd->type == DocumentDecl::Type::Defination)
    {
        visit(dd->defination.get());
    }
    else
    {
        visit(dd->instance.get());
    }
}

void SymbolVisitor::visit(ComponentDefinationDecl *cdd)
{
    {
        string name = cdd->name;
        shared_ptr<Symbol> sym(new ScopeSymbol(Symbol::Category::Component, name,
                                               Scope::Category::Component, curScope()));
        curScope()->define(sym);

        pushScope(dynamic_pointer_cast<Scope>(sym));

        for (auto &e : cdd->enumList)
        {
            visit(e.get());
        }

        for (size_t i = 0; i < cdd->propertyList.size(); i++)
        {
            cdd->propertyList[i]->index = static_cast<int>(i);
            visitPropertyDefination(cdd->propertyList[i].get());
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

        popScope();
    }
}

void SymbolVisitor::visit(ComponentInstanceDecl *cid)
{
    (void) cid;
    throw SymbolException("ComponentInstanceDecl", "Not implement");
}

void SymbolVisitor::visit(Expr *e)
{
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
        e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Int));
        break;
    }
    case Expr::Category::Float:
    {
        e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Float));
        break;
    }
    case Expr::Category::String:
    {
        e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::String));
        break;
    }
    case Expr::Category::Invalid:
    {
        throw SymbolException("Expr", "Invalid expr");
    }
    }
}

void SymbolVisitor::visit(InitListExpr *ile)
{
    assert(ile != nullptr);
    for (auto &i : ile->exprList)
    {
        visit(i.get());
    }

    shared_ptr<TypeInfo> eType;
    if (ile->exprList.size() == 0)
    {
        eType.reset(new TypeInfo(TypeInfo::Category::Void));
    }
    else
    {
        eType = ile->exprList[0]->typeInfo;
        for (size_t i = 1; i < ile->exprList.size(); i++)
        {
            if (*eType != *(ile->exprList[i]->typeInfo))
            {
                throw SymbolException("InitListExpr",
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
            throw SymbolException("BinaryOperatorExpr",
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
            throw SymbolException("BinaryOperatorExpr",
                                  "'<'/'>'/'<='/'>=' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float)
        {
            // good
        }
        else
        {
            throw SymbolException("BinaryOperatorExpr",
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
            throw SymbolException("BinaryOperatorExpr",
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
            throw SymbolException("BinaryOperatorExpr",
                                  "'=='/'!=' operator requires type of operands is int/float/string");
        }
        b->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Int));
        break;
    }
    case BinaryOperatorExpr::Op::Plus:
    {
        if (*leftType != *rightType)
        {
            throw SymbolException("BinaryOperatorExpr",
                                  "'+' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float || cat == TypeInfo::Category::String)
        {
            // good
        }
        else
        {
            throw SymbolException("BinaryOperatorExpr",
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
            throw SymbolException("BinaryOperatorExpr",
                                  "'-'/'*'/'/' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int || cat == TypeInfo::Category::Float)
        {
            // good
        }
        else
        {
            throw SymbolException("BinaryOperatorExpr",
                                  "'-'/'*'/'/' operator requires type of operands is int/float");
        }
        b->typeInfo = leftType;
        break;
    }
    case BinaryOperatorExpr::Op::Remainder:
    {
        if (*leftType != *rightType)
        {
            throw SymbolException("BinaryOperatorExpr",
                                  "'%' operator requires type of operands is same");
        }
        TypeInfo::Category cat = leftType->category();
        if (cat == TypeInfo::Category::Int)
        {
            // good
        }
        else
        {
            throw SymbolException("BinaryOperatorExpr",
                                  "'%' operator requires type of operands is int");
        }
        b->typeInfo = leftType;
        break;
    }
    case BinaryOperatorExpr::Op::Assign:
    {
        if (*leftType != *rightType)
        {
            throw SymbolException("BinaryOperatorExpr",
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
            throw SymbolException("BinaryOperatorExpr",
                                  "'=' operator requires type of operands is int/float/string");
        }
        b->typeInfo = leftType;
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
            throw SymbolException("UnaryOperatorExpr",
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
            throw SymbolException("UnaryOperatorExpr",
                                  "Unary operator '!' can only used for int expr");
        }
        break;
    }
    }
}

void SymbolVisitor::visit(CallExpr *e)
{
    assert(e != nullptr);
    if (e->funcExpr->category == Expr::Category::Ref
            || e->funcExpr->category == Expr::Category::Member)
    {
        // good
    }
    else
    {
        throw SymbolException("CallExpr", "Only f(...) and obj.f(...) is valid");
    }

    if (curScope()->category() != Scope::Category::Local)
    {
        throw SymbolException("CallExpr",
                              "CallExpr can only be used in LocalScope");
    }

    string functionName;
    vector<shared_ptr<TypeInfo>> paramTypes;

    if (e->funcExpr->category == Expr::Category::Ref)
    {
        RefExpr *r = dynamic_cast<RefExpr *>(e->funcExpr.get());
        assert(r != nullptr);
        std::shared_ptr<Symbol> func = curScope()->resolve(r->name);
        if (!func)
        {
            throw SymbolException("CallExpr", "No symbol: " + r->name);
        }

        if (func->category() == Symbol::Category::Function)
        {
            shared_ptr<FunctionSymbol> funcSymbol = dynamic_pointer_cast<FunctionSymbol>(func);
            assert(funcSymbol);
            functionName = funcSymbol->name();
            paramTypes = funcSymbol->paramTypes();
        }
        else if (func->category() == Symbol::Category::Method)
        {
            shared_ptr<MethodSymbol> methodSymbol = dynamic_pointer_cast<MethodSymbol>(func);
            assert(methodSymbol);
            functionName = methodSymbol->name();
            paramTypes = methodSymbol->paramTypes();
        }
        else
        {
            throw SymbolException("CallExpr",
                                  r->name
                                  + " is a "
                                  + Symbol::symbolCategoryString(func->category()));
        }

        util::condPrint(option::showSymbolRef, "ref %s\n", func->symbolString().c_str());
        e->funcExpr->typeInfo = func->typeInfo();
    }
    else // Expr::Category::Member
    {
        MemberExpr *m = dynamic_cast<MemberExpr *>(e->funcExpr.get());
        assert(m != nullptr);
        visit(m->instanceExpr.get());
        shared_ptr<TypeInfo> instanceType = m->instanceExpr->typeInfo;
        if (instanceType->category() == TypeInfo::Category::List)
        {
            throw SymbolException("CallExpr", "List type doesn't contain named method");
        }
        string typeName = instanceType->toString();
        std::shared_ptr<Symbol> instanceTypeSymbol = curScope()->resolve(typeName);
        if (!instanceTypeSymbol)
        {
            throw SymbolException("CallExpr", "No symbol for typeName: " + typeName);
        }
        std::shared_ptr<ScopeSymbol> scopeSym = dynamic_pointer_cast<ScopeSymbol>(instanceTypeSymbol);
        if (!scopeSym)
        {
            throw SymbolException("CallExpr", typeName + " is a "
                                  + Symbol::symbolCategoryString(instanceTypeSymbol->category())
                                  + ", doesn't contains method");
        }
        std::shared_ptr<Symbol> method = scopeSym->resolve(m->name);
        if (!method)
        {
            throw SymbolException("CallExpr", typeName + " doesn't contains method named " + m->name);
        }

        if (method->category() == Symbol::Category::Method)
        {
            shared_ptr<MethodSymbol> methodSymbol = dynamic_pointer_cast<MethodSymbol>(method);
            assert(methodSymbol);
            functionName = methodSymbol->name();
            paramTypes = methodSymbol->paramTypes();
        }
        else
        {
            throw SymbolException("CallExpr",
                                  m->name
                                  + " is a "
                                  + Symbol::symbolCategoryString(method->category()));
        }

        util::condPrint(option::showSymbolRef, "ref %s\n", method->symbolString().c_str());
        e->funcExpr->typeInfo = method->typeInfo();
    }

    for (auto &p : e->paramList)
    {
        visit(p.get());
    }

    if (paramTypes.size() != e->paramList.size())
    {
        char buf[BUF_LEN];
        snprintf(buf, sizeof(buf), "%s requires %d parameters but is passed %d parameters",
                 functionName.c_str(),
                 static_cast<int>(paramTypes.size()),
                 static_cast<int>(e->paramList.size()));
        throw SymbolException("CallExpr", string(buf));
    }

    for (size_t i = 0; i < paramTypes.size(); i++)
    {
        if (*paramTypes[i] != *(e->paramList[i]->typeInfo))
        {
            char buf[BUF_LEN];
            snprintf(buf, sizeof(buf), "%s requires %s in %dth parameters but is passed %s",
                     functionName.c_str(),
                     paramTypes[i]->toString().c_str(),
                     static_cast<int>(i),
                     e->paramList[i]->typeInfo->toString().c_str());
            throw SymbolException("CallExpr", string(buf));
        }
    }

    e->typeInfo = e->funcExpr->typeInfo;
}

void SymbolVisitor::visit(ListSubscriptExpr *e)
{
    assert(e != nullptr);
    visit(e->listExpr.get());
    visit(e->indexExpr.get());
}

void SymbolVisitor::visit(MemberExpr *e)
{
    assert(e != nullptr);

    visit(e->instanceExpr.get());
    shared_ptr<TypeInfo> typeInfo = e->instanceExpr->typeInfo;
    if (typeInfo->category() == TypeInfo::Category::List)
    {
        throw SymbolException("MemberExpr", "List type doesn't contain named member");
    }

    string typeString = typeInfo->toString();

    shared_ptr<Symbol> instanceTypeSymbol;
    if (typeInfo->category() == TypeInfo::Category::Group)
    {
        shared_ptr<GroupTypeInfo> groupTypeInfo = dynamic_pointer_cast<GroupTypeInfo>(typeInfo);
        assert(groupTypeInfo != nullptr);

        string componentName = groupTypeInfo->componentType()->toString();
        string groupName = groupTypeInfo->name();

        shared_ptr<Symbol> componentSym = curScope()->resolve(componentName);
        if (!componentSym)
        {
            throw SymbolException("MemberExpr",
                                  "No symbol for componentName " + componentName);
        }
        if (componentSym->category() != Symbol::Category::Component)
        {
            throw SymbolException("MemberExpr",
                                  componentName + " is not a component symbol");
        }
        shared_ptr<ScopeSymbol> componentScope = dynamic_pointer_cast<ScopeSymbol>(componentSym);
        assert(componentScope != nullptr);

        shared_ptr<Symbol> groupSym = componentScope->resolve(groupName);
        if (!groupSym)
        {
            throw SymbolException("MemberExpr", componentName + " doesn't contains group named " + groupName);
        }

        instanceTypeSymbol = groupSym;
    }
    else
    {
        instanceTypeSymbol = curScope()->resolve(typeString);
        if (!instanceTypeSymbol)
        {
            throw SymbolException("MemberExpr", "No symbol for typeName: " + typeString);
        }
    }

    std::shared_ptr<ScopeSymbol> scopeSym = dynamic_pointer_cast<ScopeSymbol>(instanceTypeSymbol);
    if (!scopeSym)
    {
        throw SymbolException("MemberExpr", typeString + " is a "
                              + Symbol::symbolCategoryString(instanceTypeSymbol->category())
                              + ", doesn't contains member");
    }

    std::shared_ptr<Symbol> member = scopeSym->resolve(e->name);
    if (!member)
    {
        throw SymbolException("MemberExpr", typeString + " doesn't contains member named " + e->name);
    }

    util::condPrint(option::showSymbolRef, "ref %s\n", member->symbolString().c_str());
    e->typeInfo = member->typeInfo();

    if (member->category() == Symbol::Category::Property && m_analyzingPropertyDep)
    {
        ASTNode *ast = member->astNode();
        assert(ast != nullptr);
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
        assert(pd != nullptr);

        assert(m_curAnalyzingProperty != nullptr);
        m_curAnalyzingProperty->out.push_back(pd->index);
        pd->in.push_back(m_curAnalyzingProperty->index);
        util::condPrint(option::showPropertyDep, "property [%d] -> [%d]\n", m_curAnalyzingProperty->index, pd->index);
    }
}

void SymbolVisitor::visit(RefExpr *e)
{
    assert(e != nullptr);

    std::shared_ptr<Symbol> sym = curScope()->resolve(e->name);
    if (!sym)
    {
        throw SymbolException("RefExpr", "No symbol named " + e->name);
    }

    util::condPrint(option::showSymbolRef, "ref %s\n", sym->symbolString().c_str());
    e->typeInfo = sym->typeInfo();

    if (sym->category() == Symbol::Category::Property && m_analyzingPropertyDep)
    {
        ASTNode *ast = sym->astNode();
        assert(ast != nullptr);
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
        assert(pd != nullptr);

        assert(m_curAnalyzingProperty != nullptr);
        m_curAnalyzingProperty->out.push_back(pd->index);
        pd->in.push_back(m_curAnalyzingProperty->index);
        util::condPrint(option::showPropertyDep, "property [%d] -> [%d]\n", m_curAnalyzingProperty->index, pd->index);
    }
}

void SymbolVisitor::visit(VarDecl *vd)
{
    assert(vd != nullptr);

    shared_ptr<Symbol> paramSym(new Symbol(Symbol::Category::Variable, vd->name, vd->type, vd));
    curScope()->define(paramSym);

    if (vd->expr)
    {
        visit(vd->expr.get());
    }

    vd->localIndex = m_functionLocals;
    m_functionLocals++;
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
        shared_ptr<Symbol> propertySym(new Symbol(Symbol::Category::Property, propertyName, pd->type, pd));
        curScope()->define(propertySym);

        util::condPrint(option::showPropertyDep, "property [%d] %s\n", pd->index, propertySym->symbolString().c_str());
    }
}

void SymbolVisitor::visitPropertyDefination(GroupedPropertyDecl *gpd)
{
    assert(gpd != nullptr);

    string groupName = gpd->groupName;
    string propertyName = gpd->name;

    std::shared_ptr<Symbol> group = curScope()->resolve(groupName);
    if (group)
    {
        // the group is already defined
        if (group->category() == Symbol::Category::PropertyGroup)
        {
            // the group is defined as a PropertyGroup, that's good
            std::shared_ptr<ScopeSymbol> scopeSym = dynamic_pointer_cast<ScopeSymbol>(group);
            assert(scopeSym != nullptr);
            util::condPrint(option::showSymbolRef, "ref %s\n", scopeSym->symbolString().c_str());

            shared_ptr<Symbol> propertySym(new Symbol(Symbol::Category::Property, propertyName, gpd->type, gpd));
            scopeSym->define(propertySym);

            util::condPrint(option::showPropertyDep, "property [%d] %s\n", gpd->index, propertySym->symbolString().c_str());
        }
        else
        {
            // the group is defined as a non-PropertyGroup, that's bad
            throw SymbolException("GroupedPropertyDecl",
                                  group->name() + " is already defined as "
                                  + group->symbolString());
        }
    }
    else
    {
        // the group is not defined, define it
        shared_ptr<ScopeSymbol> componentSym = dynamic_pointer_cast<ScopeSymbol>(curScope());
        shared_ptr<TypeInfo> customType(new CustomTypeInfo(componentSym->name()));

        shared_ptr<Symbol> groupSym(new ScopeSymbol(Symbol::Category::PropertyGroup,
                                                    groupName,
                                                    Scope::Category::Group,
                                                    curScope(),
                                                    shared_ptr<TypeInfo>(new GroupTypeInfo(groupName, customType))));
        curScope()->define(groupSym);

        shared_ptr<Symbol> propertySym(new Symbol(Symbol::Category::Property, propertyName, gpd->type, gpd));
        dynamic_cast<ScopeSymbol *>(groupSym.get())->define(propertySym);

        util::condPrint(option::showPropertyDep, "property [%d] %s\n", gpd->index, propertySym->symbolString().c_str());
    }
}

void SymbolVisitor::visitPropertyInitialization(PropertyDecl *pd)
{
    assert(pd != nullptr);

    m_analyzingPropertyDep = true;
    m_curAnalyzingProperty = pd;

    GroupedPropertyDecl *gpd = dynamic_cast<GroupedPropertyDecl *>(pd);
    if (gpd)
    {
        visitPropertyInitialization(gpd);
    }
    else
    {
        visit(pd->expr.get());
    }

    m_analyzingPropertyDep = false;
}

void SymbolVisitor::visitPropertyInitialization(GroupedPropertyDecl *gpd)
{
    assert(gpd != nullptr);

    visit(gpd->expr.get());
}

void SymbolVisitor::visit(ParamDecl *pd)
{
    assert(pd != nullptr);

    shared_ptr<Symbol> paramSym(new Symbol(Symbol::Category::Parameter, pd->name, pd->type));
    curScope()->define(paramSym);
}

void SymbolVisitor::visit(Stmt *s)
{
    assert(s != nullptr);

    switch (s->category)
    {
    case Stmt::Category::If:
    {
        auto *st = dynamic_cast<IfStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Decl:
    {
        auto *st = dynamic_cast<DeclStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Expr:
    {
        auto *st = dynamic_cast<ExprStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Break:
    {
        auto *st = dynamic_cast<BreakStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::While:
    {
        auto *st = dynamic_cast<WhileStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Return:
    {
        auto *st = dynamic_cast<ReturnStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Compound:
    {
        auto *st = dynamic_cast<CompoundStmt *>(s);
        visit(st);
        break;
    }
    case Stmt::Category::Continue:
    {
        auto *st = dynamic_cast<ContinueStmt *>(s);
        visit(st);
        break;
    }
    default:
    {
        throw SymbolException("Stmt", "Invalid statement category");
    }
    }
}

void SymbolVisitor::visit(CompoundStmt *cs)
{
    assert(cs != nullptr);

    shared_ptr<Scope> localScope(new Scope(Scope::Category::Local, curScope()));
    pushScope(localScope);

    for (auto &s : cs->stmtList)
    {
        visit(s.get());
    }

    popScope();
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
        throw SymbolException("IfStmt", "if statement requires type of condition expression is int, but actually "
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

void SymbolVisitor::visit(ReturnStmt *rs)
{
    assert(rs != nullptr);

    visit(rs->returnExpr.get());
}

void SymbolVisitor::visit(ExprStmt *es)
{
    assert(es != nullptr);

    visit(es->expr.get());
}

void SymbolVisitor::visitMethodHeader(FunctionDecl *fd)
{
    assert(fd != nullptr);

    shared_ptr<Scope> componentScope = curScope();
    shared_ptr<Symbol> componentSymbol = dynamic_pointer_cast<Symbol>(componentScope);
    assert(componentSymbol);

    vector<shared_ptr<TypeInfo>> paramTypes;
    for (auto &p : fd->paramList)
    {
        paramTypes.push_back(p->type);
    }

    shared_ptr<Symbol> methodSym(new MethodSymbol(fd->name, fd->returnType, componentSymbol, paramTypes));
    curScope()->define(methodSym);
}

void SymbolVisitor::visitMethodBody(FunctionDecl *fd)
{
    assert(fd != nullptr);

    shared_ptr<Scope> methodScope(new Scope(Scope::Category::Method, curScope()));
    methodScope->setScopeName(fd->name);
    pushScope(methodScope);

    for (auto &p : fd->paramList)
    {
        visit(p.get());
    }

    m_functionLocals = 0;
    visit(fd->body.get());

    string name = fd->name;
    int args = static_cast<int>(fd->paramList.size());
    if (fd->component)
    {
        name = fd->component->name + "::" + name;
        args += 1;
    }
    int locals = m_functionLocals;

    printf(".def %s args=%d locals=%d\n", name.c_str(), args, locals);

    popScope();
}

void SymbolVisitor::visit(EnumConstantDecl *ecd)
{
    assert(ecd != nullptr);

    shared_ptr<Symbol> enumSym(new Symbol(Symbol::Category::EnumConstants, ecd->name, make_shared<TypeInfo>(TypeInfo::Category::Int)));
    curScope()->define(enumSym);
}

void SymbolVisitor::visit(EnumDecl *ed)
{
    assert(ed != nullptr);

    for (size_t i = 0; i < ed->constantList.size(); i++)
    {
        ed->constantList[i]->value = static_cast<int>(i);
        visit(ed->constantList[i].get());
    }
}
