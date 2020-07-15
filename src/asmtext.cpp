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

#include "asmtext.h"

#include <assert.h>

using namespace std;

AsmText::AsmText()
{

}

void AsmText::appendLine(const std::vector<std::string> &line)
{
    m_text.push_back(line);
}

int AsmText::appendBlank()
{
    int result = static_cast<int>(m_text.size());
    m_text.emplace_back();
    return result;
}

void AsmText::setLine(int lineNumber, const std::vector<std::string> &line)
{
    assert(lineNumber >= 0 && lineNumber < static_cast<int>(m_text.size()));
    m_text[static_cast<size_t>(lineNumber)] = line;
}

void AsmText::dump()
{
    printf("---------- AsmText::dump begin ----------\n");
    for (auto &line : m_text)
    {
        if (line.size() == 0)
        {
            continue;
        }

        string firstWord = line.front();
        if (firstWord == ".def")
        {
            printf("\n");
        }
        if (firstWord.size() > 0 && firstWord[0] != '.')
        {
            printf("    ");
        }

        if (firstWord == "sconst")
        {
            assert(line.size() == 2);
            printf("%s \"%s\"", firstWord.c_str(), line[1].c_str());
        }
        else
        {
            for (auto &word : line)
            {
                printf("%s ", word.c_str());
            }
        }
        printf("\n");
    }
    printf("----------- AsmText::dump end -----------\n");
}

std::vector<std::vector<std::string> > AsmText::text() const
{
    return m_text;
}

void AsmText::clear()
{
    m_text.clear();
}

bool operator==(const AsmText &lhs, const AsmText &rhs)
{
    return lhs.m_text == rhs.m_text;
}
