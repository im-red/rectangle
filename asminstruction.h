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

#include <string>

namespace instr
{

enum AsmInstruction : unsigned char
{
    INVALID,

    IADD,
    FADD,
    SADD,

    ISUB,
    FSUB,

    IMUL,
    FMUL,

    IDIV,
    FDIV,

    IREM,

    IEQ,
    FEQ,
    SEQ,

    INE,
    FNE,
    SNE,

    ILT,
    FLT,

    IGT,
    FGT,

    ILE,
    FLE,

    IGE,
    FGE,

    INEG,
    FNEG,

    IAND,

    IOR,

    INOT,

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

    LEN,
    PRINT,
    HALT,

    PUSHORIGIN,
    POPORIGIN,

    DRAWRECT,
    DRAWTEXT,
    DRAWELLIPSE
};

std::string getAsmName(unsigned char value);
unsigned char getAsmValue(const std::string &name);

bool is0OpInstr(unsigned char value);
bool is1OpInstr(unsigned char value);

bool isBranchInstr(unsigned char value);
bool isCallInstr(unsigned char value);

}

