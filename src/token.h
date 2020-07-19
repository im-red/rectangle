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

#include <string>
#include <set>
#include <stdio.h>

namespace rectangle
{
namespace frontend
{

struct Token
{
    enum TokenType
    {
        T_IF,
        T_DEF,
        T_INT,
        T_ELSE,
        T_ENUM,
        T_LIST,
        T_VOID,
        T_BREAK,
        T_FLOAT,
        T_WHILE,
        T_RETURN,
        T_STRING,
        T_CONTINUE,
        T_STRING_LITERAL,
        T_NUMBER_LITERAL,
        T_COMMENT,
        T_L_BRACE,
        T_R_BRACE,
        T_L_BRACKET,
        T_R_BRACKET,
        T_L_PAREN,
        T_R_PAREN,
        T_COMMA,
        T_COLON,
        T_AND_AND,
        T_OR_OR,
        T_NOT,
        T_PLUS,
        T_MINUS,
        T_STAR,
        T_SLASH,
        T_REMAINDER,
        T_DOT,
        T_GE,
        T_LE,
        T_GT,
        T_LT,
        T_EQUAL,
        T_NOT_EQUAL,
        T_ASSIGN,
        T_SEMICOLON,
        T_IDENTIFIER,
        T_QUOTE,
        T_SINGLE_QUOTE,
        T_BLANK,
        T_LINE_TERMINATOR,
        T_EOF,
        T_ERROR,
        TokenCount
    };

    Token(TokenType type_ = T_ERROR, const std::string &s = "", int line_ = -1, int column_ = -1)
        : type(type_), str(s), line(line_), column(column_)
    {
    }

    static std::string tokenTypeString(TokenType type);

    bool is(int t) const { return t == type; }
    bool isNot(int t) const { return t != type; }
    bool isIn(const std::set<int> &s) const
    {
        return s.find(type) != s.end();
    }

    std::string toString()
    {
        char buf[512];
        snprintf(buf, sizeof(buf), "line %d column %d(%s)", line, column, str.c_str());
        return std::string(buf);
    }

    TokenType type;
    std::string str;
    int line;
    int column;
};

}
}
