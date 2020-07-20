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
#include "sourcefile.h"
#include "exception.h"
#include "errorprinter.h"
#include "lexer.h"

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
using namespace rectangle::diag;

TEST(symbol, INSTANCE)
{
    option::showLLTry = true;
    vector<string> paths = 
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

    map<string, SourceFile> path2file;
    for (auto &path : paths)
    {
        path2file[path] = SourceFile(path);
    }

    AST ast;
    for (auto &pair : path2file)
    {
        SourceFile &sc = pair.second;
        string code = sc.source();

        vector<rectangle::frontend::Token> tokens;
        try
        {
            tokens = Lexer().scan(code);
        }
        catch (SyntaxError &e)
        {
            printSyntaxError(sc, e);
        }

        unique_ptr<DocumentDecl> document;
        try
        {
            document = Parser().parse(tokens);
        }
        catch (SyntaxError &e)
        {
            printSyntaxError(sc, e);
        }
        document->filepath = sc.path();

        ast.addDocument(move(document));
    }

    DumpVisitor dv;
    dv.visit(&ast);

    SymbolVisitor sv;
    try
    {
        sv.visit(&ast);
    }
    catch (SyntaxError &e)
    {
        auto iter = path2file.find(e.path());
        if (iter == path2file.end())
        {
            fprintf(stderr, "Internal error: %s\n", e.what());
        }
        else
        {
            printSyntaxError(iter->second, e);
        }
    }

    AsmVisitor av;
    AsmText txt;
    try
    {
        txt = av.visit(&ast);
    }
    catch (SyntaxError &e)
    {
        auto iter = path2file.find(e.path());
        if (iter == path2file.end())
        {
            fprintf(stderr, "Internal error: %s\n", e.what());
        }
        else
        {
            printSyntaxError(iter->second, e);
        }
    }
    //txt.dump();

    AsmBin bin(txt);
    //bin.dump();

    AsmMachine machine;
    string svg = machine.run(bin, "main");

    printf("%s\n", svg.c_str());
}
