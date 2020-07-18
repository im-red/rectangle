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

namespace rectangle
{
namespace backend
{

class AsmBin
{
public:
    struct FunctionItem
    {
        FunctionItem(const std::string &name_ = "", int addr_ = -1, int args_ = -1, int locals_ = -1)
            : name(name_), addr(addr_), args(args_), locals(locals_), index(-1)
        {}
        bool isValid() const
        {
            return name != "" && addr != -1 && args != -1 && locals != -1;
        }
        std::string name;
        int addr;
        int args;
        int locals;
        int index;
    };

public:
    explicit AsmBin(const AsmText &t);
    ~AsmBin();

    void assemble(const AsmText &t);

    void dump();

    int codeSize() const;

    unsigned char getByte(int addr) const;
    int getInt(int addr) const;
    runtime::Object getConstant(int index) const;
    FunctionItem getFunction(int index) const;
    FunctionItem getFunction(const std::string &funcName) const;

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
        explicit LabelItem(const std::string &name_, int addr_ = -1)
            : name(name_), addr(addr_)
        {}
        std::string name;
        int addr;
    };

    int m_offset = 0;
    std::vector<unsigned char> m_code;
    std::vector<runtime::Object> m_constants;
    std::vector<FunctionItem> m_functions;
    std::vector<LabelItem> m_labels;

    std::vector<int> m_labelIndexAddr;
};

}
}
