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

#include "option.h"

namespace rectangle
{
namespace option
{

bool verbose = false;
bool printSymbolDef = false;
bool printSymbolRef = false;
bool printPropertyDep = false;
bool printScopeStack = false;
bool printLLTry = false;
bool printLocalIndex = false;
bool printGenAsm = false;
bool printAssemble = false;
bool printBindingDep = false;
bool printSvgDraw = false;

bool dumpAst = false;
bool dumpAsm = false;
bool dumpBytecode = false;

bool showHelp = false;
bool showOpt = false;
bool showFiles = false;

}
}
