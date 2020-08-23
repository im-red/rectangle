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

#include <stdexcept>
#include <string>

namespace rectangle
{
namespace diag
{

class SyntaxError : public std::runtime_error
{
public:
    SyntaxError(const std::string &msg)
        : std::runtime_error(msg)
    {
    }
    SyntaxError(const std::string &msg, int line, int column, const std::string &token, const std::string &path = "")
        : std::runtime_error(msg
                             + " at line "
                             + std::to_string(line)
                             + " column "
                             + std::to_string(column)
                             + " (\""
                             + token
                             + "\")")
        , m_msg(msg)
        , m_line(line)
        , m_column(column)
        , m_token(token)
        , m_path(path)
    {
    }
    SyntaxError(const std::string &msg, const frontend::Token &tok, const std::string &path = "")
        : SyntaxError(msg, tok.line, tok.column, tok.str, path)
    {
    }
    std::string msg() const
    {
        return m_msg;
    }
    int line() const
    {
        return m_line;
    }
    int column() const
    {
        return m_column;
    }
    std::string token() const
    {
        return m_token;
    }
    std::string path() const
    {
        return m_path;
    }

private:
    std::string m_msg;
    int m_line;
    int m_column;
    std::string m_token;
    std::string m_path;
};

}
}
