#include "symbol.h"
#include "parser.h"
#include "option.h"
#include "symbolvisitor.h"
#include "asmbin.h"
#include "asmmachine.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

TEST(machine, run)
{
    option::verbose = true;

    ifstream t("../test_machine.rect");
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

    AsmMachine machine;
    string svg = machine.run(bin, "Rectangle::main");

    printf("%s\n", svg.c_str());
}
