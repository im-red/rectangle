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

#include "argsparser.h"

#include <vector>
#include <assert.h>

using namespace std;

namespace rectangle
{
namespace util
{

ArgsParser::ArgsParser()
{

}

void ArgsParser::dumpHelp() const
{
    fprintf(stderr, "Supported option:\n");
    for (auto &pair : m_opt2msg)
    {
        const string &opt = pair.first;
        const string &msg = pair.second;
        fprintf(stderr, "    --%-20s: %s\n", opt.c_str(), msg.c_str());
    }
}

void ArgsParser::dumpOpt() const
{
    fprintf(stderr, "Configured option:\n");
    for (auto &pair : m_onOffLongOpt)
    {
        const string &opt = pair.first;
        bool *p = pair.second;
        fprintf(stderr, "    --%-20s: %s\n", opt.c_str(), *p ? "true" : "false");
    }
}

void ArgsParser::addOnOffLongOption(const std::string &opt, const std::string &msg, bool &onoff)
{
    m_opt2msg[opt] = msg;
    m_onOffLongOpt[opt] = &onoff;
}

static bool isLongOpt(const std::string &opt)
{
    return opt[0] == '-' && opt[1] == '-';
}

static string longOpt(const std::string &opt)
{
    assert(isLongOpt(opt));
    return opt.substr(2);
}

static bool isShortOpt(const std::string &opt)
{
    return opt[0] == '-' && opt[1] != '-';
}

static string shortOpt(const std::string &opt)
{
    assert(isShortOpt(opt));
    return opt.substr(1);
}

std::vector<string> ArgsParser::parse(int argc, char **argv)
{
    assert(argv != nullptr);
    assert(argc >= 1);

    initDefaultValue();

    vector<string> files;
    for (int i = 1; i < argc; i++)
    {
        assert(argv[i] != nullptr);
        string s(argv[i]);
        if (isLongOpt(s))
        {
            string opt = longOpt(s);
            if (m_opt2msg.count(opt) == 0)
            {
                throw ArgsException("Unknown option: " + s);
            }
            else
            {
                *(m_onOffLongOpt[opt]) = true;
            }
        }
        else if (isShortOpt(s))
        {
            string opt = shortOpt(s);
            throw ArgsException("Unknown option: " + s);
        }
        else
        {
            files.push_back(s);
        }
    }
    return files;
}

void ArgsParser::initDefaultValue()
{
    for (auto &pair : m_onOffLongOpt)
    {
        *(pair.second) = false;
    }
}

}
}
