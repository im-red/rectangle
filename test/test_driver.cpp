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

#include "driver.h"

#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace testing;
using namespace std;
using namespace rectangle::driver;

TEST(driver, COMPILE)
{
    vector<string> paths = 
    {
        "../../template/Scene.rect",
        "../../template/Rectangle.rect", 
        "../../template/Text.rect",
        "../../template/Ellipse.rect",
        "../../template/Polygon.rect",
        "../../template/Line.rect",
        "../../template/Polyline.rect",
        "../rect/symbol_instance_instance.rect"
    };

    Driver d;
    string svg = d.compile(paths);
    printf("%s\n", svg.c_str());
}