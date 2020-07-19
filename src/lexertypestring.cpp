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

#include "lexer.h"

#include <string>
#include <map>

#include <assert.h>

using namespace std;

namespace rectangle
{
namespace frontend
{

string Lexer::errorTypeString(Lexer::ErrorType error)
{
    static const map<int, string> MAP =
    {
        {IllegalSymbol, "Illegal symbol" },
        {IllegalCharacter, "Illegal character" },
        {UnclosedStringLiteral, "Unclosed string literal" },
        {StrayNewlineInStringLiteral, "Stray new line in string literal" }
    };

    assert(error > NoError && error < ErrorCount);

    return MAP.at(error);
}

}
}
