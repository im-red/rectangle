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

#include "asmbin.h"
#include "util.h"
#include "option.h"

#include <map>
#include <set>

using namespace std;

AsmBin::AsmBin(const AsmText &t)
{
    assemble(t);
}

AsmBin::~AsmBin()
{
    for (auto &o : m_constants)
    {
        delete o;
    }
}

static const map<string, unsigned char> s_instrName2value =
{
    { "iadd", AsmBin::IADD },
    { "isub", AsmBin::ISUB },
    { "imul", AsmBin::IMUL },
    { "idiv", AsmBin::IDIV },
    { "irem", AsmBin::IREM },
    { "ieq", AsmBin::IEQ },
    { "ine", AsmBin::INE },
    { "ilt", AsmBin::ILT },
    { "igt", AsmBin::IGT },
    { "ile", AsmBin::ILE },
    { "ige", AsmBin::IGE },
    { "iand", AsmBin::IAND },
    { "ior", AsmBin::IOR },
    { "fadd", AsmBin::FADD },
    { "fsub", AsmBin::FSUB },
    { "fmul", AsmBin::FMUL },
    { "fdiv", AsmBin::FDIV },
    { "feq", AsmBin::FEQ },
    { "fne", AsmBin::FNE },
    { "flt", AsmBin::FLT },
    { "fgt", AsmBin::FGT },
    { "fle", AsmBin::FLE },
    { "fge", AsmBin::FGE },
    { "sadd", AsmBin::SADD },
    { "seq", AsmBin::SEQ },
    { "sne", AsmBin::SNE },
    { "pop", AsmBin::POP },
    { "vector", AsmBin::VECTOR },
    { "vappend", AsmBin::VAPPEND },
    { "vload", AsmBin::VLOAD },
    { "vstore", AsmBin::VSTORE },
    { "ret", AsmBin::RET },
    { "print", AsmBin::PRINT },
    { "vlen", AsmBin::VLEN },
    { "slen", AsmBin::SLEN },
    { "halt", AsmBin::HALT },
    { "drawRect", AsmBin::DRAWRECT },
    { "drawText", AsmBin::DRAWTEXT },
    { "gload", AsmBin::GLOAD },
    { "gstore", AsmBin::GSTORE },
    { "lload", AsmBin::LLOAD },
    { "lstore", AsmBin::LSTORE },
    { "fload", AsmBin::FLOAD },
    { "fstore", AsmBin::FSTORE },
    { "iconst", AsmBin::ICONST },
    { "fconst", AsmBin::FCONST },
    { "sconst", AsmBin::SCONST },
    { "struct", AsmBin::STRUCT },
    { "br", AsmBin::BR },
    { "brt", AsmBin::BRT },
    { "brf", AsmBin::BRF },
    { "call", AsmBin::CALL },
};

static const map<unsigned char, string> s_instrValue2name =
{
    { AsmBin::IADD, "iadd" },
    { AsmBin::ISUB, "isub" },
    { AsmBin::IMUL, "imul" },
    { AsmBin::IDIV, "idiv" },
    { AsmBin::IREM, "irem" },
    { AsmBin::IEQ, "ieq" },
    { AsmBin::INE, "ine" },
    { AsmBin::ILT, "ilt" },
    { AsmBin::IGT, "igt" },
    { AsmBin::ILE, "ile" },
    { AsmBin::IGE, "ige" },
    { AsmBin::IAND, "iand" },
    { AsmBin::IOR, "ior" },
    { AsmBin::FADD, "fadd" },
    { AsmBin::FSUB, "fsub" },
    { AsmBin::FMUL, "fmul" },
    { AsmBin::FDIV, "fdiv" },
    { AsmBin::FEQ, "feq" },
    { AsmBin::FNE, "fne" },
    { AsmBin::FLT, "flt" },
    { AsmBin::FGT, "fgt" },
    { AsmBin::FLE, "fle" },
    { AsmBin::FGE, "fge" },
    { AsmBin::SADD, "sadd" },
    { AsmBin::SEQ, "seq" },
    { AsmBin::SNE, "sne" },
    { AsmBin::POP, "pop" },
    { AsmBin::VECTOR, "vector" },
    { AsmBin::VAPPEND, "vappend" },
    { AsmBin::VLOAD, "vload" },
    { AsmBin::VSTORE, "vstore" },
    { AsmBin::RET, "ret" },
    { AsmBin::PRINT, "print" },
    { AsmBin::VLEN, "vlen" },
    { AsmBin::SLEN, "slen" },
    { AsmBin::HALT, "halt" },
    { AsmBin::DRAWRECT, "drawRect" },
    { AsmBin::DRAWTEXT, "drawText" },
    { AsmBin::GLOAD, "gload" },
    { AsmBin::GSTORE, "gstore" },
    { AsmBin::LLOAD, "lload" },
    { AsmBin::LSTORE, "lstore" },
    { AsmBin::FLOAD, "fload" },
    { AsmBin::FSTORE, "fstore" },
    { AsmBin::ICONST, "iconst" },
    { AsmBin::FCONST, "fconst" },
    { AsmBin::SCONST, "sconst" },
    { AsmBin::STRUCT, "struct" },
    { AsmBin::BR, "br" },
    { AsmBin::BRT, "brt" },
    { AsmBin::BRF, "brf" },
    { AsmBin::CALL, "call" },
};

static const set<unsigned char> s_instr0 =
{
    AsmBin::IADD,
    AsmBin::ISUB,
    AsmBin::IMUL,
    AsmBin::IDIV,
    AsmBin::IREM,
    AsmBin::IEQ,
    AsmBin::INE,
    AsmBin::ILT,
    AsmBin::IGT,
    AsmBin::ILE,
    AsmBin::IGE,
    AsmBin::IAND,
    AsmBin::IOR,
    AsmBin::FADD,
    AsmBin::FSUB,
    AsmBin::FMUL,
    AsmBin::FDIV,
    AsmBin::FEQ,
    AsmBin::FNE,
    AsmBin::FLT,
    AsmBin::FGT,
    AsmBin::FLE,
    AsmBin::FGE,
    AsmBin::SADD,
    AsmBin::SEQ,
    AsmBin::SNE,
    AsmBin::POP,
    AsmBin::VECTOR,
    AsmBin::VAPPEND,
    AsmBin::VLOAD,
    AsmBin::VSTORE,
    AsmBin::RET,
    AsmBin::PRINT,
    AsmBin::VLEN,
    AsmBin::SLEN,
    AsmBin::HALT,
    AsmBin::DRAWRECT,
    AsmBin::DRAWTEXT,
};

static const set<unsigned char> s_instr1 =
{
    AsmBin::GLOAD,
    AsmBin::GSTORE,
    AsmBin::LLOAD,
    AsmBin::LSTORE,
    AsmBin::FLOAD,
    AsmBin::FSTORE,
    AsmBin::ICONST,
    AsmBin::FCONST,
    AsmBin::SCONST,
    AsmBin::STRUCT,
};

static const set<unsigned char> s_instrBranch =
{
    AsmBin::BR,
    AsmBin::BRT,
    AsmBin::BRF,
};

static const set<unsigned char> s_instrCall =
{
    AsmBin::CALL,
};

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

        Instruction instr = INVALID;
        if (s_instrName2value.find(firstWord) != s_instrName2value.end())
        {
            instr = static_cast<Instruction>(s_instrName2value.at(firstWord));
        }

        auto iter0 = s_instr0.find(instr);
        auto iter1 = s_instr1.find(instr);
        auto iterBranch = s_instrBranch.find(instr);

        if (iter0 != s_instr0.end())
        {
            assert(line.size() == 1);
            appendByte(instr);
        }
        else if (iter1 != s_instr1.end())
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
            appendByte(instr);
            appendInt(op);
        }
        else if (iterBranch != s_instrBranch.end())
        {
            assert(line.size() == 2);
            string label = line[1];
            int index = defineLabel(label);
            appendByte(instr);
            m_labelIndexAddr.push_back(m_offset);
            appendInt(index);
        }
        else if (firstWord == "call")
        {
            assert(line.size() == 2);
            string funcName = line[1];
            int index = defineFunction(funcName);
            appendByte(CALL);
            appendInt(index);
        }
        else if (firstWord == ".def")
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
            assert(false);
        }
    }

    fillLabelAddr();
}

void AsmBin::dump()
{
    printf("Constants:\n");
    for (size_t i = 0; i < m_constants.size(); i++)
    {
        printf("    %04x: %s\n", static_cast<unsigned>(i), m_constants[i]->toString().c_str());
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
        auto iter0 = s_instr0.find(instr);
        auto iter1 = s_instr1.find(instr);
        auto iterBranch = s_instrBranch.find(instr);
        auto iterCall = s_instrCall.find(instr);
        if (iter0 != s_instr0.end())
        {
            unsigned addr = static_cast<unsigned>(offset);
            printf("    %04x: %02x             ; %-9s\n", addr, instr, s_instrValue2name.at(instr).c_str());
            offset += 1;
        }
        else if (iter1 != s_instr1.end() || iterBranch != s_instrBranch.end() || iterCall != s_instrCall.end())
        {
            unsigned addr = static_cast<unsigned>(offset);
            int op = getInt(offset + 1);
            printf("    %04x: %02x %02x %02x %02x %02x ; %-9s %x\n",
                   offset, instr,
                   m_code[addr + 1], m_code[addr + 2], m_code[addr + 3], m_code[addr + 4],
                   s_instrValue2name.at(instr).c_str(), op);
            offset += 5;
        }
        else
        {
            printf("%d\n", instr);
            assert(false);
        }
    }
}

int AsmBin::defineFloat(float f)
{
    util::condPrint(option::showAssemble, "assemble: def float %lf\n", static_cast<double>(f));

    Object *o = new Object(f);
    m_constants.push_back(o);
    return static_cast<int>(m_constants.size() - 1);
}

int AsmBin::defineString(const std::string &s)
{
    util::condPrint(option::showAssemble, "assemble: def string %s\n", s.c_str());

    for (size_t i = 0; i < m_constants.size(); i++)
    {
        Object *o = m_constants[i];
        if (o->category() == Object::Category::String && o->stringData() == s)
        {
            return static_cast<int>(i);
        }
    }
    Object *o = new Object(s);
    m_constants.push_back(o);
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

    string printName = name;
    if (alreadyDefined)
    {
        printName += "(defined)";
    }
    if (isRef)
    {
        util::condPrint(option::showAssemble, "assemble: ref [%u] function %s\n",
                        index, printName.c_str());
    }
    else
    {
        util::condPrint(option::showAssemble, "assemble: def [%u] function %s %d %d %d\n",
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
        util::condPrint(option::showAssemble, "assemble: ref [%u] label %s\n",
                        index, printName.c_str());
    }
    else
    {
        util::condPrint(option::showAssemble, "assemble: def [%u] label %s %d\n",
                        index, printName.c_str(), addr);
    }

    return static_cast<int>(index);
}

void AsmBin::appendByte(unsigned char c)
{
    assert(c != INVALID);
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

unsigned char AsmBin::getByte(int addr)
{
    assert(addr > 0 && addr < static_cast<int>(m_code.size()));
    return m_code[static_cast<size_t>(addr)];
}

void AsmBin::setInt(int addr, int n)
{
    assert(addr > 0 && addr < static_cast<int>(m_code.size()));
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

int AsmBin::getInt(int addr)
{
    assert(addr > 0 && addr < static_cast<int>(m_code.size()));
    int result = 0;
    for (int i = 0; i < 4; i++)
    {
        int cur = m_code[static_cast<size_t>(addr + i)];
        result |= (cur << (i * 8));
    }
    return result;
}
