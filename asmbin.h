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
#include "object.h"

#include <vector>

#pragma once

class AsmBin
{
public:
    enum Instruction : unsigned char
    {
        INVALID,
        IADD,
        ISUB,
        IMUL,
        IDIV,
        IREM,
        IEQ,
        INE,
        ILT,
        IGT,
        ILE,
        IGE,
        IAND,
        IOR,
        FADD,
        FSUB,
        FMUL,
        FDIV,
        FEQ,
        FNE,
        FLT,
        FGT,
        FLE,
        FGE,
        SADD,
        SEQ,
        SNE,
        ICONST,
        FCONST,
        SCONST,
        STRUCT,
        POP,
        GLOAD,
        GSTORE,
        LLOAD,
        LSTORE,
        FLOAD,
        FSTORE,
        VECTOR,
        VAPPEND,
        VLOAD,
        VSTORE,
        BR,
        BRT,
        BRF,
        CALL,
        RET,
        PRINT,
        VLEN,
        SLEN,
        HALT,
        DRAWRECT,
        DRAWTEXT
    };

    struct FunctionItem
    {
        FunctionItem(const std::string &name_, int addr_ = -1, int args_ = -1, int locals_ = -1)
            : name(name_), addr(addr_), args(args_), locals(locals_)
        {}
        std::string name;
        int addr;
        int args;
        int locals;
    };

public:
    AsmBin(const AsmText &t);
    ~AsmBin();

    void assemble(const AsmText &t);

    void dump();

    unsigned char getByte(int addr);
    int getInt(int addr);

private:
    int defineFloat(float f);
    int defineString(const std::string &s);
    int defineFunction(const std::string &name, int addr = -1, int args = -1, int locals = -1);
    int defineLabel(const std::string &name, int addr = -1);

    void appendByte(unsigned char c);
    void appendInt(int n);

    void setInt(int addr, int n);

    void fillLabelAddr();

private:
    struct LabelItem
    {
        LabelItem(const std::string &name_, int addr_ = -1)
            : name(name_), addr(addr_)
        {}
        std::string name;
        int addr;
    };

    int m_offset = 0;
    std::vector<unsigned char> m_code;
    std::vector<Object *> m_constants;
    std::vector<FunctionItem> m_functions;
    std::vector<LabelItem> m_labels;

    std::vector<int> m_labelIndexAddr;
};

