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

#include "asmbin.h"
#include "asminstruction.h"
#include "svgpainter.h"

namespace rectangle {
namespace runtime {

class AsmMachine {
 public:
  AsmMachine();

  std::string run(const backend::AsmBin &bin, const std::string &funcName);
  std::string run(const backend::AsmBin &bin, const int addr);

 private:
  struct StackFrame {
    StackFrame(const backend::AsmBin::FunctionItem &func_, int returnAddr_)
        : func(func_), returnAddr(returnAddr_) {
      assert(func.isValid());
      int localsCount = func.args + func.locals;
      locals.resize(static_cast<size_t>(localsCount));
    }
    backend::AsmBin::FunctionItem func;
    int returnAddr;
    std::vector<Object> locals;
  };

 private:
  void reset();

  void pushOperand(ObjectPointer &&p);
  ObjectPointer popOperand();

  void mainLoop();
  void interpret(backend::instr::AsmInstruction instr, int op);

  void defineScene(const Object &o);
  void pushOrigin(int x, int y);
  void popOrigin();
  void drawRect(const Object &o);
  void drawText(const Object &o);
  void drawEllipse(const Object &o);
  void drawPolygon(const Object &o);
  void drawLine(const Object &o);
  void drawPolyline(const Object &o);

 private:
  int m_ip = 0;
  bool m_halt = false;
  std::vector<StackFrame> m_frames;
  std::vector<ObjectPointer> m_operands;

  backend::AsmBin m_asm;
  draw::SvgPainter m_painter;
};

}  // namespace runtime
}  // namespace rectangle
