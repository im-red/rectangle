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
#include <vector>

namespace rectangle {
namespace backend {

class AsmText {
  friend bool operator==(const AsmText &lhs, const AsmText &rhs);

 public:
  AsmText();

 public:
  void appendLine(const std::vector<std::string> &line);
  int appendBlank();
  void setLine(int lineNumber, const std::vector<std::string> &line);

  void dump();

  std::vector<std::vector<std::string>> text() const;
  void clear();

 private:
  std::vector<std::vector<std::string>> m_text;
};
}  // namespace backend
}  // namespace rectangle

bool operator==(const rectangle::backend::AsmText &lhs,
                const rectangle::backend::AsmText &rhs);
