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

#include "svgpainter.h"

using namespace std;

namespace draw
{

SvgPainter::SvgPainter()
{

}

void SvgPainter::clear()
{
    m_shapes.clear();
}

void SvgPainter::draw(const RectangleData &d)
{
    m_shapes.emplace_back(new RectangleShape(d));
}

void SvgPainter::draw(const TextData &d)
{
    m_shapes.emplace_back(new TextShape(d));
}

string SvgPainter::generate() const
{
    static const std::string BEGIN = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    static const std::string END = "</svg>\n";

    string result;
    result += BEGIN;
    for (auto &shape : m_shapes)
    {
        result += "    " + shape->generate() + "\n";
    }
    result += END;
    return result;
}

Shape::~Shape()
{

}

RectangleShape::RectangleShape(const RectangleData &rect)
    : m_data(rect)
{

}

std::string RectangleShape::generate()
{
    char buf[512];
    snprintf(buf, sizeof(buf), "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:%s; stroke-width:%d; stroke:%s; stroke-dasharray:%s\"/>",
             m_data.x, m_data.y, m_data.width, m_data.height, m_data.fill_color.c_str(), m_data.stroke_width, m_data.stroke_color.c_str(), m_data.stroke_dasharray.c_str());
    return string(buf);
}

TextShape::TextShape(const TextData &text)
    : m_data(text)
{

}

string TextShape::generate()
{
    char buf[512];
    snprintf(buf, sizeof(buf), "<text x=\"%d\" y=\"%d\" font-size=\"%d\">%s</text>",
             m_data.x, m_data.y, m_data.size, m_data.text.c_str());
    return string(buf);
}

}
