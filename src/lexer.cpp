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
#include "exception.h"

#include <assert.h>

using namespace std;

Lexer::Lexer()
{

}

void Lexer::setCode(const std::string &code)
{
    clear();
    m_code = code;
}

vector<Token> Lexer::scan(const string &code)
{
    setCode(code);

    vector<Token> result;

    while (true)
    {
        Token tok = nextToken();
        if (tok.type == T_ERROR)
        {
            throw SyntaxError(errorTypeString(m_error), tok.line, tok.column, tok.str);
        }

        if (tok.type == Lexer::T_COMMENT)
        {
            continue;
        }
        else
        {
            result.push_back(tok);
            if (tok.type == Lexer::T_EOF)
            {
                break;
            }
        }
    }
    return result;
}

Token Lexer::nextToken()
{
    m_tokenType = scanToken();
    Token token(m_tokenType, m_tokenString, m_tokenLine, m_tokenColumn);
    return token;
}

Lexer::TokenType Lexer::scanToken()
{
    m_tokenString = "";

    while (isSpace(m_char))
    {
        nextChar();
    }

    m_tokenPos = m_nextPos - 1;
    m_tokenLine = m_line;
    m_tokenColumn = m_column;

    if (static_cast<size_t>(m_nextPos) > m_code.size())
    {
        return T_EOF;
    }

    const char c = m_char;
    nextChar();

    m_tokenString = c;

    switch (c)
    {
    case '{': return T_L_BRACE;
    case '}': return T_R_BRACE;
    case '[': return T_L_BRACKET;
    case ']': return T_R_BRACKET;
    case '(': return T_L_PAREN;
    case ')': return T_R_PAREN;
    case '|':
    {
        if (m_char == '|')
        {
            nextChar();
            m_tokenString = "||";
            return T_OR_OR;
        }
        m_error = IllegalSymbol;
        return T_ERROR;
    }
    case '>':
    {
        if (m_char == '=')
        {
            nextChar();
            m_tokenString = ">=";
            return T_GE;
        }
        return T_GT;
    }
    case '=':
    {
        if (m_char == '=')
        {
            nextChar();
            m_tokenString = "==";
            return T_EQUAL;
        }
        return T_ASSIGN;
    }
    case '<':
    {
        if (m_char == '=')
        {
            nextChar();
            m_tokenString = "<=";
            return T_LE;
        }
        return T_LT;
    }
    case ';': return T_SEMICOLON;
    case ':': return T_COLON;
    case '/':
    {
        if (m_char == '/')
        {
            m_tokenString = "/";
            while (static_cast<size_t>(m_nextPos) <= m_code.size() && !isLineTerminator(m_char))
            {
                m_tokenString += m_char;
                nextChar();
            }
            return T_COMMENT;
        }
        return T_SLASH;
    }
    case '.': return T_DOT;
    case '-': return T_MINUS;
    case '+': return T_PLUS;
    case ',': return T_COMMA;
    case '*': return T_STAR;
    case '&':
    {
        if (m_char == '&')
        {
            nextChar();
            m_tokenString = "&&";
            return T_AND_AND;
        }
        m_error = IllegalSymbol;
        return T_ERROR;
    }
    case '%': return T_REMAINDER;
    case '!':
    {
        if (m_char == '=')
        {
            nextChar();
            m_tokenString = "!=";
            return T_NOT_EQUAL;
        }
        return T_NOT;
    }
    case '\'':
    case '"':
    {
        m_tokenString = "";
        return scanString(c);
    }
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        m_tokenString = "";
        return scanNumber(c);
    }
    default:
    {
        if (isIdentifierStart(c))
        {
            m_tokenString = c;
            while (isIdentifierPart(m_char))
            {
                m_tokenString += m_char;
                nextChar();
            }
            return classify(m_tokenString.c_str(), static_cast<int>(m_tokenString.size()));
        }
        break;
    }
    }

    m_error = IllegalCharacter;
    return T_ERROR;
}

Lexer::TokenType Lexer::scanString(char c)
{
    while (static_cast<size_t>(m_nextPos) <= m_code.size())
    {
        if (isLineTerminator(m_char))
        {
            m_error = StrayNewlineInStringLiteral;
            return T_ERROR;
        }
        else if (m_char == c)
        {
            nextChar();
            return T_STRING_LITERAL;
        }
        else
        {
            m_tokenString += m_char;
        }
        nextChar();
    }
    m_error = UnclosedStringLiteral;
    return T_ERROR;
}

Lexer::TokenType Lexer::scanNumber(char c)
{
    m_tokenString += c;
    while (isDigit(m_char))
    {
        m_tokenString += m_char;
        nextChar();
    }

    if (m_char == '.')
    {
        m_tokenString += m_char;
        nextChar();
    }
    else
    {
        return T_NUMBER_LITERAL;
    }

    while (isDigit(m_char))
    {
        m_tokenString += m_char;
        nextChar();
    }

    return T_NUMBER_LITERAL;
}

void Lexer::nextChar()
{
    if (m_skipLineFeed)
    {
        assert(m_code[static_cast<size_t>(m_nextPos)] == '\n');
        m_nextPos++;
        m_skipLineFeed = false;
    }
    m_char = m_code[static_cast<size_t>(m_nextPos)];
    m_nextPos++;
    m_column++;
    if (isLineTerminator(m_char))
    {
        if (m_char == '\r')
        {
            if (m_code[static_cast<size_t>(m_nextPos)] == '\n')
            {
                m_skipLineFeed = true;
            }
            m_char = '\n';
        }
        m_line++;
        m_column = 0;
    }
}

bool Lexer::isLineTerminator(char c)
{
    return c == '\r' || c == '\n';
}

bool Lexer::isSpace(char c)
{
    return c == ' ' || c == '\t' || isLineTerminator(c);
}

bool Lexer::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool Lexer::isIdentifierStart(char c)
{
    return (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c == '_');
}

bool Lexer::isIdentifierPart(char c)
{
    return (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9')
            || (c == '_');
}

void Lexer::clear()
{
    m_line = 1;
    m_column = 0;
    m_nextPos = 0;
    m_char = '\n';
    m_tokenString = "";
    m_tokenLine = 0;
    m_tokenColumn = 0;
    m_tokenPos = 0;
    m_error = NoError;
    m_skipLineFeed = false;
}
