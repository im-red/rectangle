#include "parser.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

TEST(parser, TYPE)
{
    string code = "list<list<list<point>>>";
    Parser p;
    Lexer l;
    l.setCode(code, 1, 1);
    p.setTokens(l.tokens());
    p.parseRule(Parser::Type, 0);
}

TEST(parser, TYPE_ERROR)
{
    string code = "list<list<list<point>";
    Parser p;
    Lexer l;
    l.setCode(code, 1, 1);
    p.setTokens(l.tokens());
    p.parseRule(Parser::Type, 0);
}

TEST(parser, LITERAL)
{
    string code = "123";
    Parser p;
    Lexer l;
    l.setCode(code);
    p.setTokens(l.tokens());
    p.parseRule(Parser::Literal, 0);
}

TEST(parser, LITERAL_ERROR)
{
    string code = "\n <  ";
    Parser p;
    Lexer l;
    l.setCode(code);
    p.setTokens(l.tokens());
    p.parseRule(Parser::Literal, 0);
}

TEST(parser, CLASS_INSTANCE)
{
    string code = "Rectangle { id: root width: 20 border.color: \"red\" Rectangle { id: child x: 10 } }";
    Parser p;
    Lexer l;
    l.setCode(code);
    p.setTokens(l.tokens());
    p.parseRule(Parser::ComponentInstance, 0);
}

TEST(parser, PARSE_DEFINATION_RECT)
{
    std::ifstream t("../defination.rect");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string code = buffer.str();

    Parser p;
    Lexer l;
    l.setCode(code);
    p.setTokens(l.tokens());
    p.parse();
}

TEST(parser, PARSE_INSTANCE_RECT)
{
    std::ifstream t("../instance.rect");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string code = buffer.str();

    Parser p;
    Lexer l;
    l.setCode(code);
    p.setTokens(l.tokens());
    p.parse();
}