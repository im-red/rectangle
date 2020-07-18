#include "symbol.h"
#include "parser.h"
#include "option.h"
#include "symbolvisitor.h"
#include "asmbin.h"
#include "asmmachine.h"
#include "symboltable.h"
#include "asmvisitor.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

using namespace rectangle;
using namespace rectangle::util;
using namespace rectangle::frontend;
using namespace rectangle::backend;
using namespace rectangle::runtime;

TEST(machine, run)
{
    option::verbose = true;

    ifstream t("../rect/test_machine.rect");
    stringstream buffer;
    buffer << t.rdbuf();
    string code = buffer.str();

    Parser p;
    Lexer l;
    auto tokens = l.scan(code);
    unique_ptr<DocumentDecl> document = p.parse(tokens);
    document->print();

    AST ast;
    ast.addDocument(move(document));

    SymbolVisitor sv;
    sv.visit(&ast);

    AsmVisitor av;
    AsmText txt = av.visit(&ast);
    txt.dump();

    AsmBin bin(txt);
    bin.dump();

    AsmMachine machine;
    string svg = machine.run(bin, "Rectangle::main");

    printf("%s\n", svg.c_str());
}
