#include "symbol.h"
#include "parser.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

TEST(symbol, DEFINATION_RECT)
{
    ifstream t("../symbol_case1.rect");
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
