#include "parser.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

TEST(parser, PARSE_DEFINATION_RECT)
{
    std::ifstream t("../rect/defination.rect");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string code = buffer.str();

    Parser p;
    Lexer l;
    l.setCode(code);
    p.parse(l.tokens());
}

TEST(parser, PARSE_INSTANCE_RECT)
{
    std::ifstream t("../rect/instance.rect");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string code = buffer.str();

    Parser p;
    Lexer l;
    l.setCode(code);
    p.parse(l.tokens());
}