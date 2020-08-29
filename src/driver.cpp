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

#include "driver.h"
#include "sourcefile.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "asmtext.h"
#include "asmbin.h"
#include "asmmachine.h"
#include "symbolvisitor.h"
#include "asmvisitor.h"
#include "exception.h"
#include "errorprinter.h"
#include "util.h"
#include "dumpvisitor.h"

#include <map>

using namespace std;
using namespace rectangle::frontend;
using namespace rectangle::backend;
using namespace rectangle::runtime;
using namespace rectangle::diag;
using namespace rectangle::util;

namespace rectangle
{
namespace driver
{

Driver::Driver()
{
}

string Driver::compile(const vector<string> &paths)
{
    map<string, SourceFile> path2file;
    for (auto &path : paths)
    {
        path2file[path] = SourceFile(path);
        if (!path2file[path].valid())
        {
            fprintf(stderr, "error: open %s failed\n", path.c_str());
            return "";
        }
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
            return "";
        }

        unique_ptr<DocumentDecl> document;
        try
        {
            document = Parser().parse(tokens);
        }
        catch (SyntaxError &e)
        {
            printSyntaxError(sc, e);
            return "";
        }
        document->filepath = sc.path();

        ast.addDocument(move(document));
    }

    if (option::dumpAst)
    {
        DumpVisitor dv;
        dv.visit(&ast);
    }

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
            fprintf(stderr, "error: %s\n", e.what());
        }
        else
        {
            printSyntaxError(iter->second, e);
        }
        return "";
    }

    AsmText txt;
    try
    {
        AsmVisitor av;
        txt = av.visit(&ast);
    }
    catch (SyntaxError &e)
    {
        auto iter = path2file.find(e.path());
        if (iter == path2file.end())
        {
            fprintf(stderr, "error: %s\n", e.what());
        }
        else
        {
            printSyntaxError(iter->second, e);
        }
        return "";
    }

    if (option::dumpAsm)
    {
        txt.dump();
    }

    AsmBin bin(txt);
    if (option::dumpBytecode)
    {
        bin.dump();
    }

    AsmMachine machine;
    string svg = machine.run(bin, "main");

    return svg;
}

}
}
