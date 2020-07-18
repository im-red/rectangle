#include "parser.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;

using namespace rectangle;
using namespace rectangle::util;
using namespace rectangle::frontend;

TEST(parser, PARSE_DEFINATION_RECT)
{
    std::ifstream t("../rect/defination.rect");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string code = buffer.str();

    Parser p;
    Lexer l;
    auto tokens = l.scan(code);
    p.parse(tokens);
}

TEST(parser, PARSE_INSTANCE_RECT)
{
    std::ifstream t("../rect/instance.rect");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string code = buffer.str();

    Parser p;
    Lexer l;
    auto tokens = l.scan(code);
    p.parse(tokens);
}