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

#pragma once

#include "token.h"

#include <string>
#include <vector>
#include <set>

namespace rectangle
{
namespace frontend
{

class Lexer
{
public:
    enum ErrorType
    {
        NoError,
        IllegalSymbol,
        IllegalCharacter,
        UnclosedStringLiteral,
        StrayNewlineInStringLiteral,
        ErrorCount
    };

public:
    static std::string errorTypeString(ErrorType error);

    Lexer();
    std::vector<Token> scan(const std::string &code);

private:
    static Token::TokenType classify(const char *s, int n);

    void setCode(const std::string &code);
    Token nextToken();

    Token::TokenType scanToken();
    Token::TokenType scanString(char c);
    Token::TokenType scanNumber(char c);

    void nextChar();

    static bool isLineTerminator(char c);
    static bool isSpace(char c);
    static bool isDigit(char c);
    static bool isIdentifierStart(char c);
    static bool isIdentifierPart(char c);

    void clear();

private:
    std::string m_code;
    int m_line = 0;
    int m_column = 0;
    int m_nextPos = 0;
    char m_char = 0;
    Token::TokenType m_tokenType = Token::TokenCount;
    std::string m_tokenString;
    int m_tokenLine = 0;
    int m_tokenColumn = 0;
    int m_tokenPos = 0;
    ErrorType m_error = NoError;
    bool m_skipLineFeed = false;
};

}
}
