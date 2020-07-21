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

#include "asminstruction.h"
#include "asmbin.h"
#include "util.h"
#include "option.h"

#include <map>
#include <set>

using namespace std;
using namespace rectangle;
using namespace rectangle::runtime;

namespace rectangle
{
namespace backend
{

AsmBin::AsmBin(const AsmText &t)
{
    assemble(t);
}

AsmBin::~AsmBin()
{
}

void AsmBin::assemble(const AsmText &t)
{
    vector<vector<string>> text = t.text();
    for (auto &line : text)
    {
        if (line.size() == 0)
        {
            continue;
        }
        const std::string &firstWord = line[0];

        if (firstWord == ".def")
        {
            assert(line.size() == 4);
            string funcName = line[1];
            int args = atoi(line[2].c_str());
            int locals = atoi(line[3].c_str());
            defineFunction(funcName, m_offset, args, locals);
        }
        else if (firstWord[0] == '.' && firstWord[1] == 'L')
        {
            assert(line.size() == 1);
            defineLabel(firstWord, m_offset);
        }
        else
        {
            instr::AsmInstruction ins = static_cast<instr::AsmInstruction>(instr::getAsmValue(firstWord));

            if (instr::isBranchInstr(ins))
            {
                assert(line.size() == 2);

                string label = line[1];
                int index = defineLabel(label);
                appendByte(ins);
                m_labelIndexAddr.push_back(m_offset);
                appendInt(index);
            }
            else if (instr::isCallInstr(ins))
            {
                assert(line.size() == 2);

                string funcName = line[1];
                int index = defineFunction(funcName);
                appendByte(instr::CALL);
                appendInt(index);
            }
            else if (instr::is0OpInstr(ins))
            {
                assert(line.size() == 1);

                appendByte(ins);
            }
            else if (instr::is1OpInstr(ins))
            {
                assert(line.size() == 2);
                int op;
                if (firstWord == "fconst")
                {
                    op = defineFloat(static_cast<float>(atof(line[1].c_str())));
                }
                else if (firstWord == "sconst")
                {
                    op = defineString(line[1]);
                }
                else
                {
                    op = atoi(line[1].c_str());
                }
                appendByte(ins);
                appendInt(op);
            }
            else
            {
                assert(false);
            }
        }
    }

    fillLabelAddr();
}

void AsmBin::dump()
{
    printf("Constants:\n");
    for (size_t i = 0; i < m_constants.size(); i++)
    {
        printf("    %04x: %s\n", static_cast<unsigned>(i), m_constants[i].toString().c_str());
    }

    printf("Functions:\n");
    for (size_t i = 0; i < m_functions.size(); i++)
    {
        FunctionItem &f = m_functions[i];
        printf("    %04x: %08x %s %d %d\n", static_cast<unsigned>(i), f.addr, f.name.c_str(), f.args, f.locals);
    }

    printf("Code:\n");
    int offset = 0;
    while (offset < static_cast<int>(m_code.size()))
    {
        unsigned char instr = m_code[static_cast<size_t>(offset)];

        if (instr::is0OpInstr(instr))
        {
            unsigned addr = static_cast<unsigned>(offset);
            printf("    %04x: %02x             ; %-9s\n", addr, instr, instr::getAsmName(instr).c_str());
            offset += 1;
        }
        else if (instr::is1OpInstr(instr))
        {
            unsigned addr = static_cast<unsigned>(offset);
            int op = getInt(offset + 1);
            printf("    %04x: %02x %02x %02x %02x %02x ; %-9s %x\n",
                   offset, instr,
                   m_code[addr + 1], m_code[addr + 2], m_code[addr + 3], m_code[addr + 4],
                    instr::getAsmName(instr).c_str(), op);
            offset += 5;
        }
        else
        {
            printf("%d\n", instr);
            assert(false);
        }
    }
}

int AsmBin::codeSize() const
{
    return static_cast<int>(m_code.size());
}

int AsmBin::defineFloat(float f)
{
    util::condPrint(option::printAssemble, "assemble: def float %lf\n", static_cast<double>(f));

    m_constants.emplace_back(f);
    return static_cast<int>(m_constants.size() - 1);
}

int AsmBin::defineString(const std::string &s)
{
    util::condPrint(option::printAssemble, "assemble: def string %s\n", s.c_str());

    for (size_t i = 0; i < m_constants.size(); i++)
    {
        Object &o = m_constants[i];
        if (o.category() == Object::Category::String && o.stringData() == s)
        {
            return static_cast<int>(i);
        }
    }

    m_constants.emplace_back(s);
    return static_cast<int>(m_constants.size() - 1);
}

int AsmBin::defineFunction(const std::string &name, int addr, int args, int locals)
{
    size_t index = 0;
    for (index = 0; index < m_functions.size(); index++)
    {
        if (m_functions[index].name == name)
        {
            break;
        }
    }
    bool alreadyDefined = (index != m_functions.size());
    bool isRef = false;

    if (addr == -1)
    {
        // it's a ref

        isRef = true;
        if (alreadyDefined)
        {
            // do nothing
        }
        else
        {
            m_functions.emplace_back(name);
            index = m_functions.size() - 1;
        }
    }
    else
    {
        // it's a def
        if (alreadyDefined)
        {
            m_functions[index].addr = addr;
            m_functions[index].args = args;
            m_functions[index].locals = locals;
        }
        else
        {
            m_functions.emplace_back(name, addr, args, locals);
            index = m_functions.size() - 1;
        }
    }
    m_functions[index].index = static_cast<int>(index);

    string printName = name;
    if (alreadyDefined)
    {
        printName += "(defined)";
    }
    if (isRef)
    {
        util::condPrint(option::printAssemble, "assemble: ref [%u] function %s\n",
                        index, printName.c_str());
    }
    else
    {
        util::condPrint(option::printAssemble, "assemble: def [%u] function %s %d %d %d\n",
                        index, printName.c_str(), addr, args, locals);
    }

    return static_cast<int>(index);
}

int AsmBin::defineLabel(const string &name, int addr)
{
    size_t index = 0;
    for (index = 0; index < m_labels.size(); index++)
    {
        if (m_labels[index].name == name)
        {
            break;
        }
    }
    bool alreadyDefined = (index != m_labels.size());
    bool isRef = false;

    if (addr == -1)
    {
        // it's a ref

        isRef = true;
        if (alreadyDefined)
        {
            // do nothing
        }
        else
        {
            m_labels.emplace_back(name);
            index = m_labels.size() - 1;
        }
    }
    else
    {
        // it's a def
        if (alreadyDefined)
        {
            m_labels[index].addr = addr;
        }
        else
        {
            m_labels.emplace_back(name, addr);
            index = m_labels.size() - 1;
        }
    }

    string printName = name;
    if (alreadyDefined)
    {
        printName += "(defined)";
    }
    else
    {
        printName += "(new)";
    }
    if (isRef)
    {
        util::condPrint(option::printAssemble, "assemble: ref [%u] label %s\n",
                        index, printName.c_str());
    }
    else
    {
        util::condPrint(option::printAssemble, "assemble: def [%u] label %s %d\n",
                        index, printName.c_str(), addr);
    }

    return static_cast<int>(index);
}

void AsmBin::appendByte(unsigned char c)
{
    assert(c != instr::INVALID);
    m_code.push_back(c);
    m_offset += 1;
}

void AsmBin::appendInt(int n)
{
    for (int i = 0; i < 4; i++)
    {
        m_code.push_back(n & 0xff);
        n >>= 8;
    }
    m_offset += 4;
}

unsigned char AsmBin::getByte(int addr) const
{
    assert(addr >= 0 && addr < static_cast<int>(m_code.size()));
    return m_code[static_cast<size_t>(addr)];
}

void AsmBin::setInt(int addr, int n)
{
    assert(addr >= 0 && addr < static_cast<int>(m_code.size()));
    for (int i = 0; i < 4; i++)
    {
        m_code[static_cast<size_t>(addr + i)] = n & 0xff;
        n >>= 8;
    }
}

void AsmBin::fillLabelAddr()
{
    for (auto a : m_labelIndexAddr)
    {
        int index = getInt(a);
        int labelAddr = m_labels[static_cast<size_t>(index)].addr;
        setInt(a, labelAddr);
    }
}

int AsmBin::getInt(int addr) const
{
    assert(addr >= 0 && addr < static_cast<int>(m_code.size()));
    int result = 0;
    for (int i = 0; i < 4; i++)
    {
        int cur = m_code[static_cast<size_t>(addr + i)];
        result |= (cur << (i * 8));
    }
    return result;
}

Object AsmBin::getConstant(int index) const
{
    assert(index >= 0 && index < static_cast<int>(m_constants.size()));
    return m_constants[static_cast<size_t>(index)];
}

AsmBin::FunctionItem AsmBin::getFunction(int index) const
{
    assert(index >= 0 && index < static_cast<int>(m_functions.size()));
    return m_functions[static_cast<size_t>(index)];
}

AsmBin::FunctionItem AsmBin::getFunction(const string &funcName) const
{
    for (size_t i = 0; i < m_functions.size(); i++)
    {
        if (m_functions[i].name == funcName)
        {
            return m_functions[i];
        }
    }
    return FunctionItem("(invalid)");
}

}
}
