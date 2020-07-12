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

#include "asmmachine.h"
#include "structinfo.h"
#include "util.h"
#include "option.h"

using namespace std;

AsmMachine::AsmMachine()
    : m_asm(AsmText())
{

}

string AsmMachine::run(const AsmBin &bin, const std::string &funcName)
{
    AsmBin::FunctionItem func = bin.getFunction(funcName);
    assert(func.isValid());

    m_asm = bin;
    reset();

    interpret(instr::CALL, func.index);
    mainLoop();

    return m_painter.generate();
}

string AsmMachine::run(const AsmBin &bin, const int addr)
{
    assert(addr >= 0 && addr < bin.codeSize());

    m_asm = bin;
    reset();

    m_ip = addr;
    mainLoop();

    return m_painter.generate();
}

void AsmMachine::reset()
{
    m_ip = 0;
    m_halt = false;
    m_frames.clear();
    m_operands.clear();
    m_painter.clear();
}

void AsmMachine::pushOperand(ObjectPointer &&p)
{
    m_operands.push_back(move(p));
}

ObjectPointer AsmMachine::popOperand()
{
    assert(m_operands.size() > 0);

    ObjectPointer result = move(m_operands.back());
    m_operands.pop_back();
    return result;
}

void AsmMachine::mainLoop()
{
    while (m_ip < m_asm.codeSize() && !m_halt)
    {
        unsigned char instr = m_asm.getByte(m_ip);
        m_ip += 1;
        int op = -1;
        if (instr::is1OpInstr(instr))
        {
            op = m_asm.getInt(m_ip);
            m_ip += 4;
        }
        interpret(static_cast<instr::AsmInstruction>(instr), op);
    }
}

void AsmMachine::interpret(instr::AsmInstruction instr, int op)
{
    switch (instr)
    {
    case instr::INVALID:
    {
        assert(false);
        break;
    }
    case instr::IADD:
    case instr::FADD:
    case instr::SADD:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 + op1), true));
        break;
    }
    case instr::ISUB:
    case instr::FSUB:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 - op1), true));
        break;
    }
    case instr::IMUL:
    case instr::FMUL:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 * op1), true));
        break;
    }
    case instr::IDIV:
    case instr::FDIV:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 / op1), true));
        break;
    }
    case instr::IREM:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 % op1), true));
        break;
    }
    case instr::IEQ:
    case instr::FEQ:
    case instr::SEQ:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 == op1), true));
        break;
    }
    case instr::INE:
    case instr::FNE:
    case instr::SNE:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 != op1), true));
        break;
    }
    case instr::ILT:
    case instr::FLT:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 < op1), true));
        break;
    }
    case instr::IGT:
    case instr::FGT:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 > op1), true));
        break;
    }
    case instr::ILE:
    case instr::FLE:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 <= op1), true));
        break;
    }
    case instr::IGE:
    case instr::FGE:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 >= op1), true));
        break;
    }
    case instr::INEG:
    case instr::FNEG:
    {
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(-op0), true));
        break;
    }
    case instr::IAND:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 && op1), true));
        break;
    }
    case instr::IOR:
    {
        Object op1 = popOperand();
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(op0 || op1), true));
        break;
    }
    case instr::INOT:
    {
        Object op0 = popOperand();
        pushOperand(ObjectPointer(new Object(!op0), true));
        break;
    }
    case instr::ICONST:
    {
        pushOperand(ObjectPointer(new Object(op), true));
        break;
    }
    case instr::FCONST:
    case instr::SCONST:
    {
        pushOperand(ObjectPointer(new Object(m_asm.getConstant(op)), true));
        break;
    }
    case instr::STRUCT:
    {
        pushOperand(ObjectPointer(new Object(Object::Category::Struct, op), true));
        break;
    }
    case instr::POP:
    {
        popOperand();
        break;
    }
    case instr::GLOAD:
    case instr::GSTORE:
    {
        assert(false);
        break;
    }
    case instr::LLOAD:
    {
        pushOperand(ObjectPointer(&m_frames.back().locals[static_cast<size_t>(op)], false));
        break;
    }
    case instr::LSTORE:
    {
        m_frames.back().locals[static_cast<size_t>(op)] = popOperand();
        break;
    }
    case instr::FLOAD:
    {
        ObjectPointer p = popOperand();
        if (p.isTmp())
        {
            pushOperand(ObjectPointer(new Object(p.get()->field(op)), true));
        }
        else
        {
            pushOperand(ObjectPointer(&(p.get()->field(op)), false));
        }
        break;
    }
    case instr::FSTORE:
    {
        Object op0 = popOperand();
        ObjectPointer p = popOperand();
        p.get()->field(op) = op0;
        break;
    }
    case instr::VECTOR:
    {
        pushOperand(ObjectPointer(new Object(Object::Category::List, 0), true));
        break;
    }
    case instr::VAPPEND:
    {
        Object op0 = popOperand();
        ObjectPointer p = popOperand();
        p.get()->append(op0);
        pushOperand(move(p));
        break;
    }
    case instr::VLOAD:
    {
        int index = popOperand().get()->intData();
        ObjectPointer p = popOperand();
        if (p.isTmp())
        {
            pushOperand(ObjectPointer(new Object(p.get()->at(index)), true));
        }
        else
        {
            pushOperand(ObjectPointer(&(p.get()->at(index)), false));
        }
        break;
    }
    case instr::VSTORE:
    {
        Object op0 = popOperand();
        int index = popOperand().get()->intData();
        ObjectPointer p = popOperand();
        p.get()->at(index) = op0;
        break;
    }
    case instr::BR:
    {
        m_ip = op;
        break;
    }
    case instr::BRT:
    {
        Object cond = popOperand();
        if (cond.intData() != 0)
        {
            m_ip = op;
        }
        break;
    }
    case instr::BRF:
    {
        Object cond = popOperand();
        if (cond.intData() == 0)
        {
            m_ip = op;
        }
        break;
    }
    case instr::CALL:
    {
        AsmBin::FunctionItem func = m_asm.getFunction(op);
        StackFrame frame(func, m_ip);
        for (int i = func.args - 1; i >= 0; i--)
        {
            frame.locals[static_cast<size_t>(i)] = popOperand();
        }
        m_frames.push_back(frame);
        m_ip = func.addr;
        break;
    }
    case instr::RET:
    {
        m_ip = m_frames.back().returnAddr;
        m_frames.pop_back();
        if (m_frames.size() == 0)
        {
            m_halt = true;
        }
        break;
    }
    case instr::LEN:
    {
        Object o = popOperand();
        if (o.category() == Object::Category::List)
        {
            pushOperand(ObjectPointer(new Object(o.elementCount()), true));
        }
        else if (o.category() == Object::Category::String)
        {
            pushOperand(ObjectPointer(new Object(static_cast<int>(o.stringData().size())), true));
        }
        else
        {
            assert(false);
        }

        break;
    }
    case instr::PRINT:
    {
        Object o = popOperand();
        printf("> %s\n", o.toString().c_str());
        break;
    }
    case instr::HALT:
    {
        m_halt = true;
        break;
    }
    case instr::PUSHORIGIN:
    {
        Object y = popOperand();
        Object x = popOperand();
        pushOrigin(x.intData(), y.intData());
        util::condPrint(option::showSvgDraw, "svg: pushOrigin (%d, %d)\n", x.intData(), y.intData());
        break;
    }
    case instr::POPORIGIN:
    {
        popOrigin();
        util::condPrint(option::showSvgDraw, "svg: popOrigin\n");
        break;
    }
    case instr::DEFINESCENE:
    {
        Object o = popOperand();
        util::condPrint(option::showSvgDraw, "svg: defineScene %s\n", o.toString().c_str());
        defineScene(o);
        break;
    }
    case instr::DRAWRECT:
    {
        Object o = popOperand();
        util::condPrint(option::showSvgDraw, "svg: drawRect %s\n", o.toString().c_str());
        drawRect(o);
        break;
    }
    case instr::DRAWTEXT:
    {
        Object o = popOperand();
        util::condPrint(option::showSvgDraw, "svg: drawText %s\n", o.toString().c_str());
        drawText(o);
        break;
    }
    case instr::DRAWELLIPSE:
    {
        Object o = popOperand();
        util::condPrint(option::showSvgDraw, "svg: drawEllipse %s\n", o.toString().c_str());
        drawEllipse(o);
        break;
    }
    }
}

void AsmMachine::defineScene(const Object &o)
{
    draw::SceneData d;
    d.leftMargin = o.field(builtin::sceneInfo.fieldIndex("leftMargin")).intData();
    d.topMargin = o.field(builtin::sceneInfo.fieldIndex("topMargin")).intData();
    d.rightMargin = o.field(builtin::sceneInfo.fieldIndex("rightMargin")).intData();
    d.bottomMargin = o.field(builtin::sceneInfo.fieldIndex("bottomMargin")).intData();
    d.width = o.field(builtin::sceneInfo.fieldIndex("width")).intData();
    d.height = o.field(builtin::sceneInfo.fieldIndex("height")).intData();
    m_painter.defineScene(d);
}

void AsmMachine::pushOrigin(int x, int y)
{
    m_painter.pushOrigin(x, y);
}

void AsmMachine::popOrigin()
{
    m_painter.popOrigin();
}

void AsmMachine::drawRect(const Object &o)
{
    draw::RectangleData d;
    d.x = o.field(builtin::rectInfo.fieldIndex("x")).intData();
    d.y = o.field(builtin::rectInfo.fieldIndex("y")).intData();
    d.width = o.field(builtin::rectInfo.fieldIndex("width")).intData();
    d.height = o.field(builtin::rectInfo.fieldIndex("height")).intData();
    d.fill_color = o.field(builtin::rectInfo.fieldIndex("fill_color")).stringData();
    d.stroke_width = o.field(builtin::rectInfo.fieldIndex("stroke_width")).intData();
    d.stroke_color = o.field(builtin::rectInfo.fieldIndex("stroke_color")).stringData();
    d.stroke_dasharray = o.field(builtin::rectInfo.fieldIndex("stroke_dasharray")).stringData();
    m_painter.draw(d);
}

void AsmMachine::drawText(const Object &o)
{
    draw::TextData d;
    d.x = o.field(builtin::textInfo.fieldIndex("x")).intData();
    d.y = o.field(builtin::textInfo.fieldIndex("y")).intData();
    d.size = o.field(builtin::textInfo.fieldIndex("size")).intData();
    d.text = o.field(builtin::textInfo.fieldIndex("text")).stringData();
    m_painter.draw(d);
}

void AsmMachine::drawEllipse(const Object &o)
{
    draw::EllipseData d;
    d.x = o.field(builtin::ellipseInfo.fieldIndex("x")).intData();
    d.y = o.field(builtin::ellipseInfo.fieldIndex("y")).intData();
    d.x_radius = o.field(builtin::ellipseInfo.fieldIndex("x_radius")).intData();
    d.y_radius = o.field(builtin::ellipseInfo.fieldIndex("y_radius")).intData();
    d.fill_color = o.field(builtin::ellipseInfo.fieldIndex("fill_color")).stringData();
    d.stroke_width = o.field(builtin::ellipseInfo.fieldIndex("stroke_width")).intData();
    d.stroke_color = o.field(builtin::ellipseInfo.fieldIndex("stroke_color")).stringData();
    d.stroke_dasharray = o.field(builtin::ellipseInfo.fieldIndex("stroke_dasharray")).stringData();
    m_painter.draw(d);
}
