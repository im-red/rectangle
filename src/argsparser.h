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

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace rectangle {
namespace util {

class ArgsException : public std::runtime_error {
 public:
  ArgsException(const std::string &msg) : std::runtime_error(msg) {}
};

class ArgsParser {
 public:
  ArgsParser();

  void dumpHelp() const;
  void dumpOpt() const;

  void addOnOffLongOption(const std::string &opt, const std::string &msg,
                          bool &onoff);
  std::vector<std::string> parse(int argc, char **argv);

 private:
  void initDefaultValue();

 private:
  std::map<std::string, std::string> m_opt2msg;
  std::map<std::string, bool *> m_onOffLongOpt;
};

}  // namespace util
}  // namespace rectangle
