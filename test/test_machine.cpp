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
