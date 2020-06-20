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

#include <assert.h>

using namespace std;

constexpr int BUF_LEN = 512;

SymbolVisitor::SymbolVisitor()
{
    initBuiltInStructs();
}

void SymbolVisitor::setDocuments(std::vector<DocumentDecl *> documents)
{
    m_documents = documents;

    m_scopes.clear();
    m_symbols.clear();
    m_curScope.reset();
    Scope::resetNextScopeId();
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

    shared_ptr<TypeInfo> voidType = make_shared<TypeInfo>(TypeInfo::Category::Void);

    {
        shared_ptr<TypeInfo> rectType(new CustomTypeInfo("rect"));
        vector<shared_ptr<TypeInfo>> paramTypes(1, rectType);
        shared_ptr<Symbol> drawRect(new FunctionSymbol("drawRect", voidType, paramTypes));
        curScope()->define(drawRect);
    }

    {
        shared_ptr<TypeInfo> ptType(new CustomTypeInfo("pt"));
        vector<shared_ptr<TypeInfo>> paramTypes(1, ptType);
        shared_ptr<Symbol> drawPt(new FunctionSymbol("drawPt", voidType, paramTypes));
        curScope()->define(drawPt);
    }

    {
        shared_ptr<TypeInfo> textType(new CustomTypeInfo("text"));
        vector<shared_ptr<TypeInfo>> paramTypes(1, textType);
        shared_ptr<Symbol> drawPt(new FunctionSymbol("drawText", voidType, paramTypes));
        curScope()->define(drawPt);
    }

    {
        shared_ptr<TypeInfo> voidType(new TypeInfo(TypeInfo::Category::Void));
        vector<shared_ptr<TypeInfo>> paramTypes(1, voidType);
        shared_ptr<Symbol> len(new FunctionSymbol("len", make_shared<TypeInfo>(TypeInfo::Category::Int), paramTypes));
        curScope()->define(len);
        shared_ptr<Symbol> print(new FunctionSymbol("print", voidType, paramTypes));
        curScope()->define(print);
    }
}

void SymbolVisitor::initBuiltInStructs()
{
    shared_ptr<TypeInfo> intType = make_shared<TypeInfo>(TypeInfo::Category::Int);
    shared_ptr<TypeInfo> StringType = make_shared<TypeInfo>(TypeInfo::Category::String);
    {
        unique_ptr<StructDecl> sd(new StructDecl);
        sd->name = "rect";

        sd->fieldList.emplace_back(new FieldDecl("x", intType));
        sd->fieldList.emplace_back(new FieldDecl("y", intType));
        sd->fieldList.emplace_back(new FieldDecl("width", intType));
        sd->fieldList.emplace_back(new FieldDecl("height", intType));
        sd->fieldList.emplace_back(new FieldDecl("fill_color", StringType));
        sd->fieldList.emplace_back(new FieldDecl("stroke_width", intType));
        sd->fieldList.emplace_back(new FieldDecl("stroke_color", StringType));
        sd->fieldList.emplace_back(new FieldDecl("stroke_dasharray", StringType));

        m_builtInStructs.push_back(move(sd));
    }

    {
        unique_ptr<StructDecl> sd(new StructDecl);
        sd->name = "pt";

        sd->fieldList.emplace_back(new FieldDecl("x", intType));
        sd->fieldList.emplace_back(new FieldDecl("y", intType));
        sd->fieldList.emplace_back(new FieldDecl("radius", intType));
        sd->fieldList.emplace_back(new FieldDecl("fill_color", StringType));

        m_builtInStructs.push_back(move(sd));
    }

    {
        unique_ptr<StructDecl> sd(new StructDecl);
        sd->name = "text";

        sd->fieldList.emplace_back(new FieldDecl("x", intType));
        sd->fieldList.emplace_back(new FieldDecl("y", intType));
        sd->fieldList.emplace_back(new FieldDecl("size", intType));
        sd->fieldList.emplace_back(new FieldDecl("text", StringType));

        m_builtInStructs.push_back(move(sd));
    }
}

void SymbolVisitor::visit()
{
    m_asm.clear();

    initGlobalScope();
    for (auto &sd : m_builtInStructs)
    {
        visit(sd.get());
    }
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

    m_asm.dump();
    AsmBin asmBin(m_asm);
    asmBin.dump();
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

void SymbolVisitor::visit(StructDecl *sd)
{
    assert(sd != nullptr);

    string name = sd->name;
    shared_ptr<Symbol> sym(new ScopeSymbol(Symbol::Category::Struct, name,
                                           Scope::Category::Struct, curScope()));
    sym->setAstNode(sd);
    curScope()->define(sym);

    pushScope(dynamic_pointer_cast<Scope>(sym));
    for (size_t i = 0; i < sd->fieldList.size(); i++)
    {
        visit(sd->fieldList[i].get());
        sd->fieldList[i]->fieldIndex = static_cast<int>(i);
    }
    popScope();
}

void SymbolVisitor::visit(ComponentDefinationDecl *cdd)
{
    {
        string name = cdd->name;
        shared_ptr<Symbol> sym(new ScopeSymbol(Symbol::Category::Component, name,
                                               Scope::Category::Component, curScope()));
        sym->setAstNode(cdd);
        curScope()->define(sym);

        pushScope(dynamic_pointer_cast<Scope>(sym));
        m_curComponentName = name;

        for (auto &e : cdd->enumList)
        {
            visit(e.get());
        }

        for (size_t i = 0; i < cdd->propertyList.size(); i++)
        {
            cdd->propertyList[i]->fieldIndex = static_cast<int>(i);
            visitPropertyDefination(cdd->propertyList[i].get());
        }
        for (auto &p : cdd->propertyList)
        {
            visitPropertyInitialization(p.get());
        }

        m_asm.appendLine({".def", name + "::" + name, "1", "0"});
        m_asm.appendLine({"ret"});

        for (auto &f : cdd->methodList)
        {
            visitMethodHeader(f.get());
        }
        for (auto &f : cdd->methodList)
        {
            visitMethodBody(f.get());
        }

        m_curComponentName = "";
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
        m_asm.appendLine({"iconst", to_string(dynamic_cast<IntegerLiteral *>(e)->value)});
        break;
    }
    case Expr::Category::Float:
    {
        e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::Float));
        m_asm.appendLine({"fconst", to_string(dynamic_cast<FloatLiteral *>(e)->value)});
        break;
    }
    case Expr::Category::String:
    {
        e->typeInfo = shared_ptr<TypeInfo>(new TypeInfo(TypeInfo::Category::String));
        m_asm.appendLine({"sconst", dynamic_cast<StringLiteral *>(e)->value});
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

    m_asm.appendLine({"vector"});
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
            m_asm.appendLine({"vappend"});
        }

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

    bool visitingLvalueBackup = m_visitingLvalue;

    if (b->op == BinaryOperatorExpr::Op::Assign)
    {
        m_visitingLvalue = true;
        m_lvalueCategory = LvalueCategory::Invalid;
    }
    visit(b->left.get());
    if (b->op == BinaryOperatorExpr::Op::Assign)
    {
        m_visitingLvalue = visitingLvalueBackup;
    }

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

    switch (b->op)
    {
    case BinaryOperatorExpr::Op::LogicalAnd:
    {
        m_asm.appendLine({"iand"});
        break;
    }
    case BinaryOperatorExpr::Op::LogicalOr:
    {
        m_asm.appendLine({"ior"});
        break;
    }
    case BinaryOperatorExpr::Op::LessThan:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"ilt"});
        }
        else
        {
            m_asm.appendLine({"flt"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::GreaterThan:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"igt"});
        }
        else
        {
            m_asm.appendLine({"fgt"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::LessEqual:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"ile"});
        }
        else
        {
            m_asm.appendLine({"fle"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::GreaterEqual:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"ige"});
        }
        else
        {
            m_asm.appendLine({"fge"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::Equal:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"ieq"});
        }
        else if (leftType->category() == TypeInfo::Category::Float)
        {
            m_asm.appendLine({"feq"});
        }
        else
        {
            m_asm.appendLine({"seq"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::NotEqual:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"ine"});
        }
        else if (leftType->category() == TypeInfo::Category::Float)
        {
            m_asm.appendLine({"fne"});
        }
        else
        {
            m_asm.appendLine({"sne"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::Plus:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"iadd"});
        }
        else if (leftType->category() == TypeInfo::Category::Float)
        {
            m_asm.appendLine({"fadd"});
        }
        else
        {
            m_asm.appendLine({"sadd"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::Minus:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"isub"});
        }
        else
        {
            m_asm.appendLine({"fsub"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::Multiply:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"imul"});
        }
        else
        {
            m_asm.appendLine({"fmul"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::Divide:
    {
        if (leftType->category() == TypeInfo::Category::Int)
        {
            m_asm.appendLine({"idiv"});
        }
        else
        {
            m_asm.appendLine({"fdiv"});
        }
        break;
    }
    case BinaryOperatorExpr::Op::Remainder:
    {
        m_asm.appendLine({"irem"});
        break;
    }
    case BinaryOperatorExpr::Op::Assign:
    {
        if (m_lvalueCategory == LvalueCategory::List)
        {
            m_asm.appendLine({"vstore"});
        }
        else if (m_lvalueCategory == LvalueCategory::Local)
        {
            m_asm.appendLine({"lstore", to_string(m_lvalueIndex)});
        }
        else if (m_lvalueCategory == LvalueCategory::Global)
        {
            m_asm.appendLine({"gstore", to_string(m_lvalueIndex)});
        }
        else if (m_lvalueCategory == LvalueCategory::Field)
        {
            m_asm.appendLine({"fstore", to_string(m_lvalueIndex)});
        }
        else // (m_lvalueCategory == LvalueCategory::Invalid)
        {
            throw SymbolException("BinaryOperatorExpr", "Invalid lvalue category");
        }
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

    if (m_visitingLvalue)
    {
        throw SymbolException("CallExpr", "CallExpr only produce rvalue");
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
        visit(r);

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

        util::condPrint(option::showSymbolRef, "ref: %s\n", method->symbolString().c_str());
        e->funcExpr->typeInfo = method->typeInfo();
    }
    else
    {
        throw SymbolException("CallExpr", "Only f(...) and obj.f(...) is valid");
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

    if (functionName == "len")
    {
        if (e->paramList[0]->typeInfo->category() == TypeInfo::Category::String)
        {
            m_asm.appendLine({"slen"});
        }
        else if (e->paramList[0]->typeInfo->category() == TypeInfo::Category::List)
        {
            m_asm.appendLine({"vlen"});
        }
        else
        {
            char buf[BUF_LEN];
            snprintf(buf, sizeof(buf), "%s requires string/list in 0th parameters but is passed %s",
                     functionName.c_str(),
                     e->paramList[0]->typeInfo->toString().c_str());
            throw SymbolException("CallExpr", string(buf));
        }
    }
    else if (functionName == "print"
             || functionName == "drawRect"
             || functionName == "drawText"
             || functionName == "drawPt")
    {
        m_asm.appendLine({functionName});
    }
    else
    {
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
        if (e->funcExpr->category == Expr::Category::Ref)
        {
            RefExpr *r = dynamic_cast<RefExpr *>(e->funcExpr.get());
            assert(r != nullptr);
            std::shared_ptr<Symbol> func = curScope()->resolve(r->name);
            assert(func != nullptr);

            if (func->category() == Symbol::Category::Function)
            {
                m_asm.appendLine({"call", func->name()});
            }
            else if (func->category() == Symbol::Category::Method)
            {
                assert(m_curComponentName != "");
                m_asm.appendLine({"call", m_curComponentName + "::" + func->name()});
            }
            else
            {
                assert(false);
            }
        }
        else if (e->funcExpr->category == Expr::Category::Member)
        {
            MemberExpr *m = dynamic_cast<MemberExpr *>(e->funcExpr.get());
            assert(m != nullptr);

            shared_ptr<TypeInfo> instanceType = m->instanceExpr->typeInfo;
            string typeName = instanceType->toString();

            m_asm.appendLine({"call", typeName + "::" + m->name});
        }
    }

    e->typeInfo = e->funcExpr->typeInfo;
}

void SymbolVisitor::visit(ListSubscriptExpr *e)
{
    assert(e != nullptr);

    bool visitingLvalueBackup = m_visitingLvalue;
    m_visitingLvalue = false;

    visit(e->listExpr.get());
    if (e->listExpr->typeInfo->category() != TypeInfo::Category::List)
    {
        throw SymbolException("ListSubscriptExpr", "Type of listExpr is " + e->listExpr->typeInfo->toString());
    }

    visit(e->indexExpr.get());
    if (e->indexExpr->typeInfo->category() != TypeInfo::Category::Int)
    {
        throw SymbolException("ListSubscriptExpr", "Type of indexExpr is " + e->indexExpr->typeInfo->toString());
    }

    m_visitingLvalue = visitingLvalueBackup;

    if (m_visitingLvalue)
    {
        m_lvalueCategory = LvalueCategory::List;
    }
    else
    {
        m_asm.appendLine({"vload"});
    }

    shared_ptr<TypeInfo> ti = e->listExpr->typeInfo;
    shared_ptr<ListTypeInfo> lti = dynamic_pointer_cast<ListTypeInfo>(ti);
    assert(lti != nullptr);

    e->typeInfo = lti->elementType();
}

void SymbolVisitor::visit(MemberExpr *e)
{
    assert(e != nullptr);

    bool visitingLvalueBackup = m_visitingLvalue;

    m_visitingLvalue = false;
    visit(e->instanceExpr.get());
    m_visitingLvalue = visitingLvalueBackup;

    shared_ptr<TypeInfo> instanceTypeInfo = e->instanceExpr->typeInfo;
    string typeString = instanceTypeInfo->toString();

    shared_ptr<Symbol> instanceTypeSymbol;
    if (instanceTypeInfo->category() == TypeInfo::Category::Group)
    {
        shared_ptr<GroupTypeInfo> groupTypeInfo = dynamic_pointer_cast<GroupTypeInfo>(instanceTypeInfo);
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
    else if (instanceTypeInfo->category() == TypeInfo::Category::Custom)
    {
        instanceTypeSymbol = curScope()->resolve(typeString);
        if (!instanceTypeSymbol)
        {
            throw SymbolException("MemberExpr", "No symbol for type " + typeString);
        }
    }
    else
    {
        throw SymbolException("MemberExpr", instanceTypeInfo->toString() + " doesn't contains member");
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

    util::condPrint(option::showSymbolRef, "ref: %s\n", member->symbolString().c_str());
    e->typeInfo = member->typeInfo();

    ASTNode *ast = member->astNode();
    if (m_visitingLvalue)
    {
        if (member->category() == Symbol::Category::Field)
        {
            FieldDecl *fd = dynamic_cast<FieldDecl *>(ast);

            m_lvalueCategory = LvalueCategory::Field;
            m_lvalueIndex = fd->fieldIndex;
        }
        else if (member->category() == Symbol::Category::Property)
        {
            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);

            m_lvalueCategory = LvalueCategory::Field;
            m_lvalueIndex = pd->fieldIndex;
        }
        else
        {
            // do nothing
        }
    }
    else
    {
        if (member->category() == Symbol::Category::Field)
        {
            FieldDecl *fd = dynamic_cast<FieldDecl *>(ast);
            assert(fd != nullptr);

            int fieldIndex = fd->fieldIndex;
            m_asm.appendLine({"fload", to_string(fieldIndex)});
        }
        else if (member->category() == Symbol::Category::Property)
        {
            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
            assert(pd != nullptr);

            int fieldIndex = pd->fieldIndex;
            m_asm.appendLine({"fload", to_string(fieldIndex)});
        }
        else
        {
            // do nothing
        }
    }

    if (member->category() == Symbol::Category::Property && m_analyzingPropertyDep)
    {
        ASTNode *ast = member->astNode();
        assert(ast != nullptr);
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
        assert(pd != nullptr);

        assert(m_curAnalyzingProperty != nullptr);
        m_curAnalyzingProperty->out.push_back(pd->fieldIndex);
        pd->in.push_back(m_curAnalyzingProperty->fieldIndex);
        util::condPrint(option::showPropertyDep, "property [%d] -> [%d]\n", m_curAnalyzingProperty->fieldIndex, pd->fieldIndex);
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

    util::condPrint(option::showSymbolRef, "ref: %s\n", sym->symbolString().c_str());
    e->typeInfo = sym->typeInfo();

    ASTNode *ast = sym->astNode();
    if (m_visitingLvalue)
    {
        switch (sym->category())
        {
        case Symbol::Category::Variable:
        {
            m_lvalueCategory = LvalueCategory::Local;

            VarDecl *vd = dynamic_cast<VarDecl *>(ast);
            assert(vd != nullptr);

            m_lvalueIndex = vd->localIndex;

            break;
        }
        case Symbol::Category::Parameter:
        {
            m_lvalueCategory = LvalueCategory::Local;

            ParamDecl *pd = dynamic_cast<ParamDecl *>(ast);
            assert(pd != nullptr);

            m_lvalueIndex = pd->localIndex;

            break;
        }
        case Symbol::Category::Property:
        {
            m_lvalueCategory = LvalueCategory::Field;

            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
            assert(pd != nullptr);

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
            throw SymbolException("RefExpr",
                                  e->name + " should't in lvalue expr");
        }
        }
    }
    else
    {
        switch (sym->category())
        {
        case Symbol::Category::Variable:
        {
            VarDecl *vd = dynamic_cast<VarDecl *>(ast);
            assert(vd != nullptr);

            util::condPrint(option::showGenAsm,
                            "genAsm: ref %s(Variable) in RefExpr(rvalue)\n", sym->name().c_str());

            m_asm.appendLine({"lload", to_string(vd->localIndex)});

            break;
        }
        case Symbol::Category::Parameter:
        {
            ParamDecl *pd = dynamic_cast<ParamDecl *>(ast);
            assert(pd != nullptr);

            util::condPrint(option::showGenAsm,
                            "genAsm: ref %s(Parameter) in RefExpr(rvalue)\n", sym->name().c_str());

            m_asm.appendLine({"lload", to_string(pd->localIndex)});

            break;
        }
        case Symbol::Category::Property:
        {
            PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
            assert(pd != nullptr);

            util::condPrint(option::showGenAsm,
                            "genAsm: ref %s(Property) in RefExpr(rvalue)\n", sym->name().c_str());

            m_asm.appendLine({"lload", "0"});
            m_asm.appendLine({"fload", to_string(pd->fieldIndex)});

            break;
        }
        case Symbol::Category::PropertyGroup:
        {
            util::condPrint(option::showGenAsm,
                            "genAsm: ref %s(PropertyGroup) in RefExpr(rvalue)\n", sym->name().c_str());

            m_asm.appendLine({"lload", "0"});

            break;
        }
        case Symbol::Category::EnumConstants:
        {
            EnumConstantDecl *ecd = dynamic_cast<EnumConstantDecl *>(ast);
            assert(ecd != nullptr);

            util::condPrint(option::showGenAsm,
                            "genAsm: ref %s(EnumConstants) in RefExpr(rvalue)\n", sym->name().c_str());

            m_asm.appendLine({"iconst", to_string(ecd->value)});

            break;
        }
        case Symbol::Category::Method:
        {
            util::condPrint(option::showGenAsm,
                            "genAsm: ref %s(Method) in RefExpr(rvalue)\n", sym->name().c_str());

            m_asm.appendLine({"lload", "0"});
            break;
        }
        default:
        {
        }
        }
    }

    if (sym->category() == Symbol::Category::Property && m_analyzingPropertyDep)
    {
        ASTNode *ast = sym->astNode();
        assert(ast != nullptr);
        PropertyDecl *pd = dynamic_cast<PropertyDecl *>(ast);
        assert(pd != nullptr);

        assert(m_curAnalyzingProperty != nullptr);
        m_curAnalyzingProperty->out.push_back(pd->fieldIndex);
        pd->in.push_back(m_curAnalyzingProperty->fieldIndex);
        util::condPrint(option::showPropertyDep, "property [%d] -> [%d]\n", m_curAnalyzingProperty->fieldIndex, pd->fieldIndex);
    }
}

void SymbolVisitor::visit(VarDecl *vd)
{
    assert(vd != nullptr);

    shared_ptr<Symbol> paramSym(new Symbol(Symbol::Category::Variable, vd->name, vd->type, vd));
    curScope()->define(paramSym);

    vd->localIndex = m_functionLocals;
    m_functionLocals++;

    util::condPrint(option::showGenAsm, "genAsm: localIndex [%d] %s\n",
                    vd->localIndex,
                    vd->name.c_str());

    if (vd->expr)
    {
        visit(vd->expr.get());
        if (!(*vd->type == *vd->expr->typeInfo) && !vd->type->assignCompatible(vd->expr->typeInfo))
        {
            throw SymbolException("VarDecl", "Type doesn't match("
                                  + vd->type->toString()
                                  + ", "
                                  + vd->expr->typeInfo->toString()
                                  + ")");
        }
        m_asm.appendLine({"lstore", to_string(vd->localIndex)});
    }
    else
    {
        if (vd->type->category() == TypeInfo::Category::Custom)
        {
            std::string className = vd->type->toString();
            shared_ptr<Symbol> classSymbol = curScope()->resolve(className);
            if (classSymbol == nullptr)
            {
                throw SymbolException("VarDecl", "No symbol for type " + className);
            }
            util::condPrint(option::showSymbolRef, "ref: %s\n", classSymbol->symbolString().c_str());

            int memberCount = 0;
            ASTNode *ast = classSymbol->astNode();
            if (classSymbol->category() == Symbol::Category::Struct)
            {
                StructDecl *sd = dynamic_cast<StructDecl *>(ast);
                assert(sd != nullptr);
                memberCount = static_cast<int>(sd->fieldList.size());
            }
            else if (classSymbol->category() == Symbol::Category::Component)
            {
                ComponentDefinationDecl *cdd = dynamic_cast<ComponentDefinationDecl *>(ast);
                assert(cdd != nullptr);
                memberCount = static_cast<int>(cdd->propertyList.size());
            }
            util::condPrint(option::showGenAsm, "genAsm: %s member count %d\n",
                            classSymbol->symbolString().c_str(),
                            memberCount);

            m_asm.appendLine({"struct", to_string(memberCount)});
            if (classSymbol->category() == Symbol::Category::Component)
            {
                m_asm.appendLine({"call", classSymbol->name() + "::" + classSymbol->name()});
            }
            m_asm.appendLine({"lstore", to_string(vd->localIndex)});
        }
    }
}

void SymbolVisitor::visit(FieldDecl *md)
{
    assert(md != nullptr);

    string name = md->name;
    shared_ptr<Symbol> sym(new Symbol(Symbol::Category::Field, name, md->type, md));
    curScope()->define(sym);
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

        util::condPrint(option::showPropertyDep, "property [%d] %s\n", pd->fieldIndex, propertySym->symbolString().c_str());
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
            util::condPrint(option::showSymbolRef, "ref: %s\n", scopeSym->symbolString().c_str());

            shared_ptr<Symbol> propertySym(new Symbol(Symbol::Category::Property, propertyName, gpd->type, gpd));
            scopeSym->define(propertySym);

            util::condPrint(option::showPropertyDep, "property [%d] %s\n", gpd->fieldIndex, propertySym->symbolString().c_str());
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

        util::condPrint(option::showPropertyDep, "property [%d] %s\n", gpd->fieldIndex, propertySym->symbolString().c_str());
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

    shared_ptr<Symbol> paramSym(new Symbol(Symbol::Category::Parameter, pd->name, pd->type, pd));
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

    const int falseLabel = m_labelCounter++;

    visit(is->condition.get());
    if (is->condition->typeInfo->category() != TypeInfo::Category::Int)
    {
        throw SymbolException("IfStmt", "if statement requires type of condition expression is int, but actually "
                              + is->condition->typeInfo->toString());
    }
    m_asm.appendLine({"brf", string(".L") + to_string(falseLabel)});
    visit(is->thenStmt.get());
    m_asm.appendLine({string(".L") + to_string(falseLabel)});
    if (is->elseStmt)
    {
        visit(is->elseStmt.get());
    }
}

void SymbolVisitor::visit(WhileStmt *ws)
{
    assert(ws != nullptr);

    const int conditionLabel = m_labelCounter++;
    const int endLabel = m_labelCounter++;

    m_breakLabels.push_back(endLabel);
    m_continueLabels.push_back(conditionLabel);

    m_asm.appendLine({string(".L") + to_string(conditionLabel)});
    visit(ws->condition.get());
    m_asm.appendLine({"brf", string(".L") + to_string(endLabel)});
    visit(ws->bodyStmt.get());
    m_asm.appendLine({"br", string(".L") + to_string(conditionLabel)});
    m_asm.appendLine({string(".L") + to_string(endLabel)});

    m_breakLabels.pop_back();
    m_continueLabels.pop_back();
}

void SymbolVisitor::visit(BreakStmt *bs)
{
    assert(bs != nullptr);
    assert(m_breakLabels.size() != 0);

    m_asm.appendLine({"br", string(".L") + to_string(m_breakLabels.back())});
}

void SymbolVisitor::visit(ContinueStmt *cs)
{
    assert(cs != nullptr);
    assert(m_continueLabels.size() != 0);

    m_asm.appendLine({"br", string(".L") + to_string(m_continueLabels.back())});
}

void SymbolVisitor::visit(ReturnStmt *rs)
{
    assert(rs != nullptr);

    if (rs->returnExpr)
    {
        visit(rs->returnExpr.get());
    }

    m_asm.appendLine({"ret"});
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

    for (size_t i = 0; i < fd->paramList.size(); i++)
    {
        visit(fd->paramList[i].get());
        fd->paramList[i]->localIndex = static_cast<int>(i);
    }

    string name = fd->name;
    int args = static_cast<int>(fd->paramList.size());
    if (fd->component)
    {
        name = fd->component->name + "::" + name;
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

    int headerLine = m_asm.appendBlank();
    m_functionLocals = args;
    visit(fd->body.get());

    int locals = m_functionLocals - args;

    m_asm.setLine(headerLine, {".def", name, to_string(args), to_string(locals)});

    popScope();
}

void SymbolVisitor::visit(EnumConstantDecl *ecd)
{
    assert(ecd != nullptr);

    shared_ptr<Symbol> enumSym(new Symbol(Symbol::Category::EnumConstants, ecd->name, make_shared<TypeInfo>(TypeInfo::Category::Int), ecd));
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
