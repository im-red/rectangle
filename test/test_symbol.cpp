#include "symbol.h"
#include "parser.h"
#include "option.h"
#include "symbolvisitor.h"
#include "asmbin.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

// TEST(symbol, METHOD_CALL)
// {
//     //option::verbose = true;
//     ifstream t("../symbol_method_call.rect");
//     stringstream buffer;
//     buffer << t.rdbuf();
//     string code = buffer.str();

//     Parser p;
//     Lexer l;
//     l.setCode(code);
//     p.setTokens(l.tokens());
//     p.parse();
//     p.print();

//     vector<DocumentDecl *> documents;
//     documents.push_back(p.document());

//     SymbolVisitor sv;
//     sv.setDocuments(documents);
//     sv.visit();
// }

// TEST(symbol, IF_STATEMENT)
// {
//     ifstream t("../symbol_if_statement.rect");
//     stringstream buffer;
//     buffer << t.rdbuf();
//     string code = buffer.str();

//     Parser p;
//     Lexer l;
//     l.setCode(code);
//     p.setTokens(l.tokens());
//     p.parse();
//     p.print();

//     vector<DocumentDecl *> documents;
//     documents.push_back(p.document());

//     SymbolVisitor sv;
//     sv.setDocuments(documents);
//     sv.visit();
// }

// TEST(symbol, PROPERTY_DEP)
// {
//     ifstream t("../symbol_property_dep.rect");
//     stringstream buffer;
//     buffer << t.rdbuf();
//     string code = buffer.str();

//     Parser p;
//     Lexer l;
//     l.setCode(code);
//     p.setTokens(l.tokens());
//     p.parse();
//     p.print();

//     vector<DocumentDecl *> documents;
//     documents.push_back(p.document());

//     SymbolVisitor sv;
//     sv.setDocuments(documents);
//     sv.visit();
// }

TEST(symbol, ASM)
{
    option::showAst = true;
    option::showGenAsm = true;
    option::showAssemble = true;

    ifstream t("../symbol_asm.rect");
    stringstream buffer;
    buffer << t.rdbuf();
    string code = buffer.str();

    Parser p;
    Lexer l;
    l.setCode(code);
    p.setTokens(l.tokens());
    p.parse();
    p.print();

    vector<DocumentDecl *> documents;
    documents.push_back(p.document());

    SymbolVisitor sv;
    sv.setDocuments(documents);
    AsmText txt = sv.visit();
    txt.dump();
    AsmBin bin(txt);
    bin.dump();
}
