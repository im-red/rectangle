#include "symbol.h"
#include "parser.h"
#include "option.h"
#include "symbolvisitor.h"
#include "asmbin.h"
#include "symboltable.h"
#include "ast.h"
#include "asmvisitor.h"
#include "asmmachine.h"
#include "dumpvisitor.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

TEST(symbol, INSTANCE)
{
    vector<string> files = {"../../template/Rectangle.rect", "../../template/Text.rect", "../rect/symbol_instance_instance.rect"};

    AST ast;
    for (auto &file : files)
    {
        ifstream t(file);
        stringstream buffer;
        buffer << t.rdbuf();

        string code = buffer.str();

        Parser p;
        Lexer l;
        l.setCode(code);
        unique_ptr<DocumentDecl> document = p.parse(l.tokens());

        ast.addDocument(move(document));
    }

    DumpVisitor dv;
    dv.visit(&ast);

    SymbolVisitor sv;
    sv.visit(&ast);

    AsmVisitor av;
    AsmText txt = av.visit(&ast);
    //txt.dump();

    AsmBin bin(txt);
    //bin.dump();

    AsmMachine machine;
    string svg = machine.run(bin, "main");

    printf("%s\n", svg.c_str());
}
