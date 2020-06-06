#include "symbol.h"
#include "parser.h"
#include "option.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

TEST(symbol, METHOD_CALL)
{
    option::verbose = true;
    ifstream t("../symbol_method_call.rect");
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
    sv.visit();
}

TEST(symbol, IF_STATEMENT)
{
    option::verbose = true;
    ifstream t("../symbol_if_statement.rect");
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
    sv.visit();
}