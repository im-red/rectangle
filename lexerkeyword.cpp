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

#include "lexer.h"

static inline Lexer::TokenType classify2(const char *s)
{
    if (s[0] == 'i')
    {
        if (s[1] == 'f')
        {
            return Lexer::T_IF;
        }
    }
    return Lexer::T_IDENTIFIER;
}

static inline Lexer::TokenType classify3(const char *s)
{
    if (s[0] == 'd')
    {
        if (s[1] == 'e')
        {
            if (s[2] == 'f')
            {
                return Lexer::T_DEF;
            }
        }
    }
    if (s[0] == 'i')
    {
        if (s[1] == 'n')
        {
            if (s[2] == 't')
            {
                return Lexer::T_INT;
            }
        }
    }
    return Lexer::T_IDENTIFIER;
}

static inline Lexer::TokenType classify4(const char *s)
{
    if (s[0] == 'c')
    {
        if (s[1] == 'h')
        {
            if (s[2] == 'a')
            {
                if (s[3] == 'r')
                {
                    return Lexer::T_CHAR;
                }
            }
        }
    }
    if (s[0] == 'e')
    {
        if (s[1] == 'l')
        {
            if (s[2] == 's')
            {
                if (s[3] == 'e')
                {
                    return Lexer::T_ELSE;
                }
            }
        }
        if (s[1] == 'n')
        {
            if (s[2] == 'u')
            {
                if (s[3] == 'm')
                {
                    return Lexer::T_ENUM;
                }
            }
        }
    }
    if (s[0] == 'l')
    {
        if (s[1] == 'i')
        {
            if (s[2] == 's')
            {
                if (s[3] == 't')
                {
                    return Lexer::T_LIST;
                }
            }
        }
    }
    if (s[0] == 'v')
    {
        if (s[1] == 'o')
        {
            if (s[2] == 'i')
            {
                if (s[3] == 'd')
                {
                    return Lexer::T_VOID;
                }
            }
        }
    }
    return Lexer::T_IDENTIFIER;
}

static inline Lexer::TokenType classify5(const char *s)
{
    if (s[0] == 'b')
    {
        if (s[0] == 'r')
        {
            if (s[1] == 'e')
            {
                if (s[2] == 'a')
                {
                    if (s[3] == 'k')
                    {
                        return Lexer::T_BREAK;
                    }
                }
            }
        }
    }
    if (s[0] == 'f')
    {
        if (s[0] == 'l')
        {
            if (s[1] == 'o')
            {
                if (s[2] == 'a')
                {
                    if (s[3] == 't')
                    {
                        return Lexer::T_FLOAT;
                    }
                }
            }
        }
    }
    if (s[0] == 'p')
    {
        if (s[0] == 'o')
        {
            if (s[1] == 'i')
            {
                if (s[2] == 'n')
                {
                    if (s[3] == 't')
                    {
                        return Lexer::T_POINT;
                    }
                }
            }
        }
    }
    if (s[0] == 'w')
    {
        if (s[0] == 'h')
        {
            if (s[1] == 'i')
            {
                if (s[2] == 'l')
                {
                    if (s[3] == 'e')
                    {
                        return Lexer::T_WHILE;
                    }
                }
            }
        }
    }
    return Lexer::T_IDENTIFIER;
}

static inline Lexer::TokenType classify6(const char *s)
{
    if (s[0] == 'i')
    {
        if (s[1] == 'm')
        {
            if (s[2] == 'p')
            {
                if (s[3] == 'o')
                {
                    if (s[4] == 'r')
                    {
                        if (s[5] == 't')
                        {
                            return Lexer::T_IMPORT;
                        }
                    }
                }
            }
        }
    }
    if (s[0] == 'r')
    {
        if (s[1] == 'e')
        {
            if (s[2] == 't')
            {
                if (s[3] == 'u')
                {
                    if (s[4] == 'r')
                    {
                        if (s[5] == 'n')
                        {
                            return Lexer::T_RETURN;
                        }
                    }
                }
            }
        }
    }
    if (s[0] == 's')
    {
        if (s[1] == 't')
        {
            if (s[2] == 'r')
            {
                if (s[3] == 'i')
                {
                    if (s[4] == 'n')
                    {
                        if (s[5] == 'g')
                        {
                            return Lexer::T_STRING;
                        }
                    }
                }
            }
        }
    }
    return Lexer::T_IDENTIFIER;
}

static inline Lexer::TokenType classify8(const char *s)
{
    if (s[0] == 'c')
    {
        if (s[1] == 'o')
        {
            if (s[2] == 'n')
            {
                if (s[3] == 't')
                {
                    if (s[4] == 'i')
                    {
                        if (s[5] == 'n')
                        {
                            if (s[6] == 'u')
                            {
                                if (s[7] == 'e')
                                {
                                    return Lexer::T_CONTINUE;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Lexer::T_IDENTIFIER;
}

Lexer::TokenType Lexer::classify(const char *s, int n)
{
    switch (n)
    {
    case 2: return classify2(s);
    case 3: return classify3(s);
    case 4: return classify4(s);
    case 5: return classify5(s);
    case 6: return classify6(s);
    case 8: return classify8(s);
    default: return T_IDENTIFIER;
    }
}
