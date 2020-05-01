#include "lexer.h"

#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;

TEST(lexer, STRING_LITERAL)
{
    std::string code = "     abcde     ";

    Lexer l;
    l.setCode("\"" + code + "\"", 0, 0);
    Lexer::TokenType type = l.scanToken();
    std::string s = l.tokenString();

    EXPECT_EQ(type, Lexer::T_STRING_LITERAL);
    EXPECT_EQ(s, code);
}

TEST(lexer, STRING_LITERAL_BAD)
{
    {
        std::string code = "     abcde  \n   ";

        Lexer l;
        l.setCode("\"" + code + "\"", 0, 0);
        Lexer::TokenType type = l.scanToken();
        Lexer::ErrorType err = l.error();

        EXPECT_EQ(type, Lexer::T_ERROR);
        EXPECT_EQ(err, Lexer::StrayNewlineInStringLiteral);
    }
    {
        std::string code = "     abcde     ";

        Lexer l;
        l.setCode("\"" + code, 0, 0);
        Lexer::TokenType type = l.scanToken();
        Lexer::ErrorType err = l.error();

        EXPECT_EQ(type, Lexer::T_ERROR);
        EXPECT_EQ(err, Lexer::UnclosedStringLiteral);
    }
}

TEST(lexer, NUMBER_LITERAL)
{
    {
        std::string code = "12345";

        Lexer l;
        l.setCode(code, 0, 0);
        Lexer::TokenType type = l.scanToken();
        std::string s = l.tokenString();

        EXPECT_EQ(type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(s, code);
    }
    {
        std::string code = "12345";

        Lexer l;
        l.setCode("   " + code + "   ", 0, 0);
        Lexer::TokenType type = l.scanToken();
        std::string s = l.tokenString();

        EXPECT_EQ(type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(s, code);
    }
    {
        std::string code = "123.45";

        Lexer l;
        l.setCode(code, 0, 0);
        Lexer::TokenType type = l.scanToken();
        std::string s = l.tokenString();

        EXPECT_EQ(type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(s, code);
    }
    {
        std::string code = "123.45";

        Lexer l;
        l.setCode("   " + code + "   ", 0, 0);
        Lexer::TokenType type = l.scanToken();
        std::string s = l.tokenString();

        EXPECT_EQ(type, Lexer::T_NUMBER_LITERAL);
        EXPECT_EQ(s, code);
    }
}

TEST(lexer, parse_file)
{
    std::ifstream t("../example.rect");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string code = buffer.str();

    Lexer l;
    l.setCode(code, 0, 0);

    Lexer::TokenType token;
    while ((token = l.scanToken()) != Lexer::T_EOF)
    {
        std::string s = l.tokenString();
        std::cout << l.tokenLine() << " " << l.tokenColumn() << " " << Lexer::tokenTypeString(token) << " " << s << std::endl;
    }
    std::cout << l.tokenLine() << " " << l.tokenColumn() << " " << Lexer::tokenTypeString(Lexer::T_EOF) << std::endl;
}