#include "lexer.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

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

TEST(lexer, STRING_LITERAL)
{
    string code = "     abcde     ";

    Lexer l;
    l.setCode("\"" + code + "\"", 0, 0);

    Token tok = l.nextToken();

    EXPECT_EQ(tok.type, Lexer::T_STRING_LITERAL);
    EXPECT_EQ(tok.str, code);
}

TEST(lexer, STRING_LITERAL_BAD)
{
    {
        string code = "     abcde  \n   ";

        Lexer l;
        l.setCode("\"" + code + "\"", 0, 0);
        
        Token tok = l.nextToken();

        EXPECT_EQ(tok.type, Lexer::T_ERROR);
        EXPECT_EQ(l.error(), Lexer::StrayNewlineInStringLiteral);
    }
    {
        string code = "     abcde     ";

        Lexer l;
        l.setCode("\"" + code, 0, 0);
        
        Token tok = l.nextToken();

        EXPECT_EQ(tok.type, Lexer::T_ERROR);
        EXPECT_EQ(l.error(), Lexer::UnclosedStringLiteral);
    }
}

TEST(lexer, NUMBER_LITERAL)
{
    {
        string code = "12345";

        Lexer l;
        l.setCode(code, 0, 0);
        
        Token tok = l.nextToken();

        EXPECT_EQ(tok.type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(tok.str, code);
    }
    {
        string code = "12345";

        Lexer l;
        l.setCode("   " + code + "   ", 0, 0);
        
        Token tok = l.nextToken();

        EXPECT_EQ(tok.type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(tok.str, code);
    }
    {
        string code = "123.45";

        Lexer l;
        l.setCode(code, 0, 0);
        
        Token tok = l.nextToken();

        EXPECT_EQ(tok.type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(tok.str, code);
    }
    {
        string code = "123.45";

        Lexer l;
        l.setCode("   " + code + "   ", 0, 0);
        
        Token tok = l.nextToken();

        EXPECT_EQ(tok.type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(tok.str, code);
    }
}

// TEST(lexer, parse_file)
// {
//     ifstream t("../example.rect");
//     stringstream buffer;
//     buffer << t.rdbuf();
//     string code = buffer.str();

//     Lexer l;
//     l.setCode(code, 0, 0);

//     Lexer::TokenType token;
//     while ((token = l.scanToken()) != Lexer::T_EOF)
//     {
//         string s = l.tokenString();
//         cout << l.tokenLine() << " " << l.tokenColumn() << " " << Lexer::tokenTypeString(token) << " " << s << endl;
//     }
//     cout << l.tokenLine() << " " << l.tokenColumn() << " " << Lexer::tokenTypeString(Lexer::T_EOF) << endl;
// }