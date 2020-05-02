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

struct Token
{
    Token(int t, const std::string &s, int l, int col)
        : type(t), str(s), line(l), column(col)
    {
    }

    int type;
    std::string str;
    int line;
    int column;
};

class Lexer
{
public:
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
        T_POINT,
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
        T_QUOTE,
        T_SINGLE_QUOTE,
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
        T_BLANK,
        T_LINE_TERMINATOR,
        T_IDENTIFIER,
        T_EOF,
        T_ERROR,
        TokenCount
    };

    enum ErrorType
    {
        NoError,
        IllegalSymbol,
        IllegalCharacter,
        IllegalInt,
        IllegalFloat,
        UnclosedStringLiteral,
        StrayNewlineInStringLiteral,
        IllegalIdentifier,
        ErrorCount
    };

public:
    Lexer();
    void setCode(const std::string &code, int line, int column);

    Token nextToken();

    static std::string tokenTypeString(TokenType token);
    static std::string errorTypeString(ErrorType error);

    int line() const;
    int column() const;
    ErrorType error() const;

    std::string tokenString() const;
    int tokenLine() const;
    int tokenColumn() const;
    int tokenPos() const;

private:
    TokenType scanToken();
    TokenType scanString(char c);
    TokenType scanNumber(char c);

    void nextChar();

    static bool isLineTerminator(char c);
    static bool isSpace(char c);
    static bool isDigit(char c);
    static bool isIdentifierStart(char c);
    static bool isIdentifierPart(char c);

    static TokenType classify(const char *s, int n);

private:
    std::string m_code;
    int m_line = 0;
    int m_column = 0;
    int m_pos = 0;
    char m_char = 0;
    TokenType m_tokenType = TokenCount;
    std::string m_tokenString;
    int m_tokenLine = 0;
    int m_tokenColumn = 0;
    int m_tokenPos = 0;
    ErrorType m_error = NoError;
    bool m_skipLineFeed = false;
};

