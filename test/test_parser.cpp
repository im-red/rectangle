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