#include "lexer.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

static void singleTokenHelper(const std::string &code, int type, const std::string &str)
{
    Lexer l;
    l.setCode(code, 1, 1);

    Token tok = l.nextToken();

    EXPECT_EQ(tok.type, type);
    EXPECT_EQ(tok.str, str);
}

static void singleTokenErrorHelper(const std::string &code, int error)
{
    Lexer l;
    l.setCode(code, 1, 1);

    Token tok = l.nextToken();

    EXPECT_EQ(tok.type, Lexer::T_ERROR);
    EXPECT_EQ(l.error(), error);
}


TEST(lexer, KEYWORD)
{
    string code = "if\ndef\nint\nelse\nenum\nlist\nvoid\nbreak\nfloat\npoint\n" \
        "while\nreturn\nstring\ncontinue";
    vector<int> expect = { Lexer::T_IF, Lexer::T_DEF, Lexer::T_INT, Lexer::T_ELSE, 
        Lexer::T_ENUM, Lexer::T_LIST, Lexer::T_VOID, Lexer::T_BREAK, 
        Lexer::T_FLOAT, Lexer::T_POINT, Lexer::T_WHILE, Lexer::T_RETURN, 
        Lexer::T_STRING, Lexer::T_CONTINUE, Lexer::T_EOF };
    vector<int> actual;

    Lexer l;
    l.setCode(code, 1, 1);
    while (true)
    {
        Token tok = l.nextToken();
        actual.push_back(tok.type);
        if (tok.type == Lexer::T_EOF)
        {
            break;
        }
    }
    EXPECT_EQ(expect, actual);
}

TEST(lexer, COMMENT)
{
    string code = "//abcde";
    singleTokenHelper(code, Lexer::T_COMMENT, code);
}

TEST(lexer, SYMBOL)
{
    string code = "{}[](),:&&||!+-*/%. >= <= > < == != =;";
    vector<int> expect = { Lexer::T_L_BRACE, Lexer::T_R_BRACE, Lexer::T_L_BRACKET, Lexer::T_R_BRACKET, 
        Lexer::T_L_PAREN, Lexer::T_R_PAREN, Lexer::T_COMMA, Lexer::T_COLON, 
        Lexer::T_AND_AND, Lexer::T_OR_OR, 
        Lexer::T_NOT, Lexer::T_PLUS, Lexer::T_MINUS,  Lexer::T_STAR,
        Lexer::T_SLASH, Lexer::T_REMAINDER, Lexer::T_DOT, Lexer::T_GE,
        Lexer::T_LE, Lexer::T_GT, Lexer::T_LT, Lexer::T_EQUAL,
        Lexer::T_NOT_EQUAL, Lexer::T_ASSIGN, Lexer::T_SEMICOLON, Lexer::T_EOF };
    vector<int> actual;

    Lexer l;
    l.setCode(code, 1, 1);
    while (true)
    {
        Token tok = l.nextToken();
        actual.push_back(tok.type);
        if (tok.type == Lexer::T_EOF)
        {
            break;
        }
    }
    EXPECT_EQ(expect, actual);
}

TEST(lexer, SYMBOL_ERROR)
{
    string code = "|";
    singleTokenErrorHelper(code, Lexer::IllegalSymbol);
}

TEST(lexer, STRING_LITERAL)
{
    string code = "   abcde   ";
    singleTokenHelper('"' + code + '"', Lexer::T_STRING_LITERAL, code);
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
        singleTokenHelper(code, Lexer::T_NUMBER_LITERAL, code);
    }
    {
        string code = "123.45";
        singleTokenHelper(code, Lexer::T_NUMBER_LITERAL, code);
    }
}

TEST(lexer, CHARACTOR_ERROR)
{
    string code = "@";
    singleTokenErrorHelper(code, Lexer::IllegalCharacter);
}