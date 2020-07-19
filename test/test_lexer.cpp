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

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#define private public
#include "lexer.h"
#undef private

using namespace testing;
using namespace std;

using namespace rectangle;
using namespace rectangle::frontend;

static void singleTokenHelper(const std::string &code, int type, const std::string &str)
{
    Lexer l;
    l.setCode(code);

    Token tok = l.nextToken();

    EXPECT_EQ(tok.type, type);
    EXPECT_EQ(tok.str, str);
}

static void singleTokenErrorHelper(const std::string &code, int error)
{
    Lexer l;
    l.setCode(code);

    Token tok = l.nextToken();

    EXPECT_EQ(tok.type, Token::T_ERROR);
    EXPECT_EQ(l.m_error, error);
}


TEST(lexer, KEYWORD)
{
    string code = "if\ndef\nint\nelse\nenum\nlist\nvoid\nbreak\nfloat\n" \
        "while\nreturn\nstring\ncontinue";
    vector<int> expect = { Token::T_IF, Token::T_DEF, Token::T_INT, Token::T_ELSE, 
        Token::T_ENUM, Token::T_LIST, Token::T_VOID, Token::T_BREAK, 
        Token::T_FLOAT, Token::T_WHILE, Token::T_RETURN, 
        Token::T_STRING, Token::T_CONTINUE, Token::T_EOF };
    vector<int> actual;

    Lexer l;
    l.setCode(code);
    while (true)
    {
        Token tok = l.nextToken();
        actual.push_back(tok.type);
        if (tok.type == Token::T_EOF)
        {
            break;
        }
    }
    EXPECT_EQ(expect, actual);
}

TEST(lexer, COMMENT)
{
    string code = "//abcde";
    singleTokenHelper(code, Token::T_COMMENT, code);
}

TEST(lexer, SYMBOL)
{
    string code = "{}[](),:&&||!+-*/%. >= <= > < == != =;";
    vector<int> expect = { Token::T_L_BRACE, Token::T_R_BRACE, Token::T_L_BRACKET, Token::T_R_BRACKET, 
        Token::T_L_PAREN, Token::T_R_PAREN, Token::T_COMMA, Token::T_COLON, 
        Token::T_AND_AND, Token::T_OR_OR, 
        Token::T_NOT, Token::T_PLUS, Token::T_MINUS,  Token::T_STAR,
        Token::T_SLASH, Token::T_REMAINDER, Token::T_DOT, Token::T_GE,
        Token::T_LE, Token::T_GT, Token::T_LT, Token::T_EQUAL,
        Token::T_NOT_EQUAL, Token::T_ASSIGN, Token::T_SEMICOLON, Token::T_EOF };
    vector<int> actual;

    Lexer l;
    l.setCode(code);
    while (true)
    {
        Token tok = l.nextToken();
        actual.push_back(tok.type);
        if (tok.type == Token::T_EOF)
        {
            break;
        }
    }
    EXPECT_EQ(expect, actual);
}

TEST(lexer, SYMBOL_ERROR)
{
    {
        string code = "|";
        singleTokenErrorHelper(code, Lexer::IllegalSymbol);
    }
    {
        string code = "&";
        singleTokenErrorHelper(code, Lexer::IllegalSymbol);
    }
}

TEST(lexer, STRING_LITERAL)
{
    string code = "   abcde   ";
    singleTokenHelper('"' + code + '"', Token::T_STRING_LITERAL, code);
}

TEST(lexer, STRING_LITERAL_ERROR)
{
    {
        string code = "     abcde  \n   ";
        singleTokenErrorHelper("\"" + code + "\"", Lexer::StrayNewlineInStringLiteral);
    }
    {
        string code = "     abcde     ";
        singleTokenErrorHelper("\"" + code, Lexer::UnclosedStringLiteral);
    }
}

TEST(lexer, NUMBER_LITERAL)
{
    {
        string code = "12345";
        singleTokenHelper(code, Token::T_NUMBER_LITERAL, code);
    }
    {
        string code = "123.45";
        singleTokenHelper(code, Token::T_NUMBER_LITERAL, code);
    }
}

TEST(lexer, CHARACTOR_ERROR)
{
    string code = "@";
    singleTokenErrorHelper(code, Lexer::IllegalCharacter);
}