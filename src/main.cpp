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
#include "argsparser.h"
#include "option.h"

#include <vector>
#include <string>

#include <stdlib.h>

using namespace std;
using namespace rectangle;
using namespace rectangle::util;
using namespace rectangle::driver;

static vector<string> parseArgs(int argc, char **argv)
{
    ArgsParser ap;
    ap.addOnOffLongOption("verbose", "Show all verbose message", option::verbose);
    ap.addOnOffLongOption("print-symbol-def", "Print message when define a symbol", option::printSymbolDef);
    ap.addOnOffLongOption("print-symbol-ref", "Print message when reference a symbol", option::printSymbolRef);
    ap.addOnOffLongOption("print-property-dep", "Show property dependency information", option::printPropertyDep);
    ap.addOnOffLongOption("print-scope-stack", "Show scope stack of symbol", option::printScopeStack);
    ap.addOnOffLongOption("print-ll-try", "Show LL try in parsing", option::printLLTry);
    ap.addOnOffLongOption("print-local-index", "Print message when define a local variable", option::printLocalIndex);
    ap.addOnOffLongOption("print-gen-asm", "Show information in generating asm", option::printGenAsm);
    ap.addOnOffLongOption("print-assemble", "Show information in assembling asm", option::printAssemble);
    ap.addOnOffLongOption("print-binding-dep", "Show binding dependency information", option::printBindingDep);
    ap.addOnOffLongOption("print-svg-draw", "Show information in drawing svg", option::printSvgDraw);
    ap.addOnOffLongOption("dump-ast", "Dump the ast", option::dumpAst);
    ap.addOnOffLongOption("dump-asm", "Dump the asm source", option::dumpAsm);
    ap.addOnOffLongOption("dump-bytecode", "Dump the bytecode", option::dumpBytecode);
    ap.addOnOffLongOption("help", "Show help", option::showHelp);
    ap.addOnOffLongOption("show-opt", "Show option configured", option::showOpt);
    ap.addOnOffLongOption("show-files", "Show input files", option::showFiles);

    vector<string> files;

    try
    {
        files = ap.parse(argc, argv);
    }
    catch (ArgsException &e)
    {
        fprintf(stderr, "%s\n", e.what());
        ap.dumpHelp();
        exit(EXIT_FAILURE);
    }

    if (option::showHelp)
    {
        ap.dumpHelp();
        exit(EXIT_SUCCESS);
    }
    if (option::showOpt)
    {
        ap.dumpOpt();
    }
    if (option::showFiles)
    {
        fprintf(stderr, "Input files:\n");
        for (auto &f : files)
        {
            fprintf(stderr, "    %s\n", f.c_str());
        }
    }

    return files;
}

int main(int argc, char **argv)
{
    auto files = parseArgs(argc, argv);

    Driver d;
    string svg = d.compile(files);

    if (svg.size())
    {
        printf("%s\n", svg.c_str());
    }
    return 0;
}
