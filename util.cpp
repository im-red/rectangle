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

#include "util.h"
#include "option.h"

#include <stdio.h>

using namespace std;

namespace util
{

void condPrint(bool cond, const char * const fmt, ...)
{
    if (option::verbose || cond)
    {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
}

static vector<string> asmStorage;

void collectAsm(const char * const fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[512];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    asmStorage.push_back(buf);
}

std::vector<std::string> dumpAsm()
{
    vector<string> result;
    result.swap(asmStorage);
    return result;
}

}
