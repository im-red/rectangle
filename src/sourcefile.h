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
namespace frontend {

class SourceFile {
 public:
  SourceFile(const std::string &path = "");

  std::string path() const;
  std::string source() const;
  std::vector<std::string> lines() const;
  std::string line(int n) const;

  bool valid() const;

 private:
  std::string m_path;
  bool m_valid = false;
  std::string m_source;
  std::vector<std::string> m_lines;
};

}  // namespace frontend
}  // namespace rectangle
