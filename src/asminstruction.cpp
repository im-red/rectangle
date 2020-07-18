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

#include <map>
#include <set>

#include <assert.h>

using namespace std;

namespace rectangle
{
namespace backend
{
namespace instr
{

std::string getAsmName(unsigned char value)
{
    static const map<unsigned char, string> s_instrValue2name =
    {
        { IADD, "iadd" },
        { ISUB, "isub" },
        { IMUL, "imul" },
        { IDIV, "idiv" },
        { IREM, "irem" },
        { IEQ, "ieq" },
        { INE, "ine" },
        { ILT, "ilt" },
        { IGT, "igt" },
        { ILE, "ile" },
        { IGE, "ige" },
        { INEG, "ineg" },
        { IAND, "iand" },
        { IOR, "ior" },
        { INOT, "inot" },
        { FADD, "fadd" },
        { FSUB, "fsub" },
        { FMUL, "fmul" },
        { FDIV, "fdiv" },
        { FEQ, "feq" },
        { FNE, "fne" },
        { FLT, "flt" },
        { FGT, "fgt" },
        { FLE, "fle" },
        { FGE, "fge" },
        { FNEG, "fneg" },
        { SADD, "sadd" },
        { SEQ, "seq" },
        { SNE, "sne" },
        { POP, "pop" },
        { VECTOR, "vector" },
        { VAPPEND, "vappend" },
        { VLOAD, "vload" },
        { VSTORE, "vstore" },
        { RET, "ret" },
        { LEN, "len" },
        { PRINT, "print" },
        { HALT, "halt" },
        { PUSHORIGIN, "pushOrigin" },
        { POPORIGIN, "popOrigin" },
        { DEFINESCENE, "defineScene" },
        { DRAWRECT, "drawRect" },
        { DRAWTEXT, "drawText" },
        { DRAWELLIPSE, "drawEllipse" },
        { DRAWPOLYGON, "drawPolygon" },
        { DRAWLINE, "drawLine" },
        { DRAWPOLYLINE, "drawPolyline" },
        { GLOAD, "gload" },
        { GSTORE, "gstore" },
        { LLOAD, "lload" },
        { LSTORE, "lstore" },
        { FLOAD, "fload" },
        { FSTORE, "fstore" },
        { ICONST, "iconst" },
        { FCONST, "fconst" },
        { SCONST, "sconst" },
        { STRUCT, "struct" },
        { BR, "br" },
        { BRT, "brt" },
        { BRF, "brf" },
        { CALL, "call" },
    };

    auto iter = s_instrValue2name.find(value);
    assert(iter != s_instrValue2name.end());

    return iter->second;
}

unsigned char getAsmValue(const std::string &name)
{
    static const map<string, unsigned char> s_instrName2value =
    {
        { "iadd", IADD },
        { "isub", ISUB },
        { "imul", IMUL },
        { "idiv", IDIV },
        { "irem", IREM },
        { "ieq", IEQ },
        { "ine", INE },
        { "ilt", ILT },
        { "igt", IGT },
        { "ile", ILE },
        { "ige", IGE },
        { "ineg", INEG },
        { "iand", IAND },
        { "ior", IOR },
        { "inot", INOT },
        { "fadd", FADD },
        { "fsub", FSUB },
        { "fmul", FMUL },
        { "fdiv", FDIV },
        { "feq", FEQ },
        { "fne", FNE },
        { "flt", FLT },
        { "fgt", FGT },
        { "fle", FLE },
        { "fge", FGE },
        { "fneg", FNEG },
        { "sadd", SADD },
        { "seq", SEQ },
        { "sne", SNE },
        { "pop", POP },
        { "vector", VECTOR },
        { "vappend", VAPPEND },
        { "vload", VLOAD },
        { "vstore", VSTORE },
        { "ret", RET },
        { "len", LEN },
        { "print", PRINT },
        { "halt", HALT },
        { "pushOrigin", PUSHORIGIN },
        { "defineScene", DEFINESCENE },
        { "popOrigin", POPORIGIN },
        { "drawRect", DRAWRECT },
        { "drawPolygon", DRAWPOLYGON },
        { "drawEllipse", DRAWELLIPSE },
        { "drawText", DRAWTEXT },
        { "drawLine", DRAWLINE },
        { "drawPolyline", DRAWPOLYLINE },
        { "gload", GLOAD },
        { "gstore", GSTORE },
        { "lload", LLOAD },
        { "lstore", LSTORE },
        { "fload", FLOAD },
        { "fstore", FSTORE },
        { "iconst", ICONST },
        { "fconst", FCONST },
        { "sconst", SCONST },
        { "struct", STRUCT },
        { "br", BR },
        { "brt", BRT },
        { "brf", BRF },
        { "call", CALL },
    };

    auto iter = s_instrName2value.find(name);
    assert(iter != s_instrName2value.end());

    return iter->second;
}

bool is0OpInstr(unsigned char value)
{
    static const set<unsigned char> s_instr0 =
    {
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
        INEG,
        IAND,
        IOR,
        INOT,
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
        FNEG,
        SADD,
        SEQ,
        SNE,
        POP,
        VECTOR,
        VAPPEND,
        VLOAD,
        VSTORE,
        RET,
        LEN,
        PRINT,
        HALT,
        DEFINESCENE,
        PUSHORIGIN,
        POPORIGIN,
        DRAWRECT,
        DRAWTEXT,
        DRAWELLIPSE,
        DRAWPOLYGON,
        DRAWLINE,
        DRAWPOLYLINE
    };

    return s_instr0.find(value) != s_instr0.end();
}

bool is1OpInstr(unsigned char value)
{
    static const set<unsigned char> s_instr1 =
    {
        GLOAD,
        GSTORE,
        LLOAD,
        LSTORE,
        FLOAD,
        FSTORE,
        ICONST,
        FCONST,
        SCONST,
        STRUCT,
        BR,
        BRT,
        BRF,
        CALL,
    };

    return s_instr1.find(value) != s_instr1.end();
}

bool isBranchInstr(unsigned char value)
{
    static const set<unsigned char> s_instrBranch =
    {
        BR,
        BRT,
        BRF,
    };

    return s_instrBranch.find(value) != s_instrBranch.end();
}

bool isCallInstr(unsigned char value)
{
    static const set<unsigned char> s_instrCall =
    {
        CALL,
    };

    return s_instrCall.find(value) != s_instrCall.end();
}

}
}
}
