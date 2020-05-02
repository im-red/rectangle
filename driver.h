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

#pragma once

#include "lexer.h"

#include <vector>

class Driver
{
public:
    enum class Stage
    {
        Lex,
        Parse,
        End
    };

public:
    Driver();

    Stage compile(const std::string &code);

    std::string svgResult() const { return m_svgResult; }
    std::string errorString() const { return m_errorString; }

private:
    std::string m_svgResult;
    std::string m_errorString;

    Lexer m_lexer;
    std::vector<Token> m_tokens;
};

