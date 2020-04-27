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

#define ENUM_ELEMENT(x) { x, #x }

string Lexer::tokenTypeString(TokenType token)
{
    static const std::map<int, std::string> STRING_MAP =
    {
        ENUM_ELEMENT(T_LIST),
        ENUM_ELEMENT(T_STRING),
        ENUM_ELEMENT(T_INT),
        ENUM_ELEMENT(T_FLOAT),
        ENUM_ELEMENT(T_VOID),
        ENUM_ELEMENT(T_POINT),
        ENUM_ELEMENT(T_STRING_LITERAL),
        ENUM_ELEMENT(T_NUMBER_LITERAL),
        ENUM_ELEMENT(T_IF),
        ENUM_ELEMENT(T_DEF),
        ENUM_ELEMENT(T_ELSE),
        ENUM_ELEMENT(T_ENUM),
        ENUM_ELEMENT(T_BREAK),
        ENUM_ELEMENT(T_WHILE),
        ENUM_ELEMENT(T_RETURN),
        ENUM_ELEMENT(T_CONTINUE),
        ENUM_ELEMENT(T_COMMENT),
        ENUM_ELEMENT(T_L_BRACE),
        ENUM_ELEMENT(T_R_BRACE),
        ENUM_ELEMENT(T_L_BRACKET),
        ENUM_ELEMENT(T_R_BRACKET),
        ENUM_ELEMENT(T_L_PAREN),
        ENUM_ELEMENT(T_R_PAREN),
        ENUM_ELEMENT(T_COMMA),
        ENUM_ELEMENT(T_COLON),
        ENUM_ELEMENT(T_QUOTE),
        ENUM_ELEMENT(T_SINGLE_QUOTE),
        ENUM_ELEMENT(T_AND_AND),
        ENUM_ELEMENT(T_OR_OR),
        ENUM_ELEMENT(T_NOT),
        ENUM_ELEMENT(T_PLUS),
        ENUM_ELEMENT(T_MINUS),
        ENUM_ELEMENT(T_STAR),
        ENUM_ELEMENT(T_SLASH),
        ENUM_ELEMENT(T_REMAINDER),
        ENUM_ELEMENT(T_DOT),
        ENUM_ELEMENT(T_GE),
        ENUM_ELEMENT(T_LE),
        ENUM_ELEMENT(T_GT),
        ENUM_ELEMENT(T_LT),
        ENUM_ELEMENT(T_EQUAL),
        ENUM_ELEMENT(T_NOT_EQUAL),
        ENUM_ELEMENT(T_ASSIGN),
        ENUM_ELEMENT(T_SEMICOLON),
        ENUM_ELEMENT(T_BLANK),
        ENUM_ELEMENT(T_LINE_TERMINATOR),
        ENUM_ELEMENT(T_IDENTIFIER),
        ENUM_ELEMENT(T_EOF),
        ENUM_ELEMENT(T_ERROR),
    };

    assert(STRING_MAP.size() == TokenCount);
    assert(token >= 0 && token < TokenCount);

    return STRING_MAP.at(token);
}

string Lexer::errorTypeString(Lexer::ErrorType error)
{
    static const std::map<int, std::string> STRING_MAP =
    {
        ENUM_ELEMENT(NoError),
        ENUM_ELEMENT(IllegalSymbol),
        ENUM_ELEMENT(IllegalCharacter),
        ENUM_ELEMENT(IllegalInt),
        ENUM_ELEMENT(IllegalFloat),
        ENUM_ELEMENT(UnclosedStringLiteral),
        ENUM_ELEMENT(StrayNewlineInStringLiteral),
        ENUM_ELEMENT(IllegalIdentifier)
    };

    assert(STRING_MAP.size() == ErrorCount);
    assert(error >= 0 && error < ErrorCount);

    return STRING_MAP.at(error);
}
