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

#include "sourcefile.h"

#include <assert.h>

#include "util.h"

using namespace std;

namespace rectangle {
namespace frontend {

SourceFile::SourceFile(const string &path) : m_path(path) {
  if (!util::fileExists(path)) {
    m_valid = false;
  } else {
    m_source = util::readFile(path);
    m_lines = util::splitIntoLines(m_source);
    m_valid = true;
  }
}

std::string SourceFile::path() const { return m_path; }

std::string SourceFile::source() const { return m_source; }

std::vector<std::string> SourceFile::lines() const { return m_lines; }

string SourceFile::line(int n) const {
  assert(n >= 0 && n < static_cast<int>(m_lines.size()));
  return m_lines[static_cast<size_t>(n)];
}

bool SourceFile::valid() const { return m_valid; }

}  // namespace frontend
}  // namespace rectangle
