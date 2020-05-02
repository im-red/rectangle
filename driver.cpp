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

Driver::Driver()
{

}

Driver::Stage Driver::compile(const std::string &code)
{
    m_svgResult.clear();
    m_errorString.clear();
    m_tokens.clear();

    m_lexer.setCode(code, 1, 1);
    while (true)
    {
        Token tok = m_lexer.nextToken();
        m_tokens.push_back(tok);

        if (tok.type == Lexer::T_ERROR)
        {
            m_errorString = m_lexer.errorString();
            return Stage::Lex;
        }
        else if (tok.type == Lexer::T_EOF)
        {
            break;
        }
    }

    return Stage::Parse;
}
