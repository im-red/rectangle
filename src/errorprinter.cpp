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

#include "errorprinter.h"

#include <stdio.h>

using namespace std;

namespace rectangle
{
namespace diag
{

void printSyntaxError(const frontend::SourceFile &sc, const SyntaxError &e)
{
    static const char *COLOR_PREFIX = "\033[40;31m";
    static const char *COLOR_SUFFIX = "\033[0m";

    fprintf(stderr, "%s:%d:%d: %serror%s: %s\n",
            sc.path().c_str(), e.line(), e.column(),
            COLOR_PREFIX, COLOR_SUFFIX,
            e.msg().c_str());

    const size_t tokenBegin = static_cast<size_t>(e.column()) - 1;
    const size_t tokenEnd = static_cast<size_t>(e.column()) - 1 + e.token().size();

    string line = sc.line(e.line() - 1);

    string left = line.substr(0, tokenBegin);
    string mid = line.substr(tokenBegin, tokenEnd - tokenBegin);
    string right = line.substr(tokenEnd);

    fprintf(stderr, "%s%s%s%s%s\n", left.c_str(), COLOR_PREFIX, mid.c_str(), COLOR_SUFFIX, right.c_str());

    string indicator = string(static_cast<size_t>(e.column() - 1), ' ') + '^';
    fprintf(stderr, "%s%s%s\n", COLOR_PREFIX, indicator.c_str(), COLOR_SUFFIX);
}

}
}
