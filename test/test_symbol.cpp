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
    option::verbose = true;
    vector<string> files = 
    {
        "../../template/Scene.rect",
        "../../template/Rectangle.rect", 
        "../../template/Text.rect", 
        "../../template/Ellipse.rect", 
        "../../template/Polygon.rect", 
        "../../template/Line.rect",
        "../../template/Polyline.rect", 
        "../rect/symbol_instance_instance.rect"
    };

    AST ast;
    for (auto &file : files)
    {
        string code = util::readFile(file);

        Parser p;
        Lexer l;
        l.setCode(code);
        unique_ptr<DocumentDecl> document = p.parse(l.tokens());
        document->filename = file;

        ast.addDocument(move(document));
    }

    DumpVisitor dv;
    dv.visit(&ast);

    SymbolVisitor sv;
    sv.visit(&ast);

    AsmVisitor av;
    AsmText txt = av.visit(&ast);
    txt.dump();

    AsmBin bin(txt);
    //bin.dump();

    AsmMachine machine;
    string svg = machine.run(bin, "main");

    printf("%s\n", svg.c_str());
}
