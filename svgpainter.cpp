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

#include <assert.h>

using namespace std;

namespace draw
{

SvgPainter::SvgPainter()
{
    clear();
}

void SvgPainter::clear()
{
    m_shapes.clear();
    m_originStack.clear();
    m_curOrigin.x = 0;
    m_curOrigin.y = 0;
    m_svgWidth = 0;
    m_svgHeight = 0;
    m_leftMargin = 0;
    m_topMargin = 0;
    m_rightMargin = 0;
    m_bottomMargin = 0;
}

void SvgPainter::defineScene(const SceneData &d)
{
    m_leftMargin = d.leftMargin;
    m_topMargin = d.topMargin;
    m_rightMargin = d.rightMargin;
    m_bottomMargin = d.bottomMargin;

    m_svgWidth = m_leftMargin + m_rightMargin + d.width;
    m_svgHeight = m_topMargin + m_bottomMargin + d.height;
}

void SvgPainter::pushOrigin(int x, int y)
{
    m_curOrigin.x += x;
    m_curOrigin.y += y;
    m_originStack.emplace_back(x, y);
}

void SvgPainter::popOrigin()
{
    assert(m_originStack.size() > 0);
    m_curOrigin.x -= m_originStack.back().x;
    m_curOrigin.y -= m_originStack.back().y;
    m_originStack.pop_back();
}

void SvgPainter::draw(const RectangleData &d)
{
    m_shapes.emplace_back(new RectangleShape(d, m_curOrigin.x, m_curOrigin.y));
}

void SvgPainter::draw(const TextData &d)
{
    m_shapes.emplace_back(new TextShape(d, m_curOrigin.x, m_curOrigin.y));
}

void SvgPainter::draw(const EllipseData &d)
{
    m_shapes.emplace_back(new EllipseShape(d, m_curOrigin.x, m_curOrigin.y));
}

void SvgPainter::draw(const PolygonData &d)
{
    m_shapes.emplace_back(new PolygonShape(d, m_curOrigin.x, m_curOrigin.y));
}

std::string SvgPainter::generate() const
{
    char buf[512];
    snprintf(buf, sizeof(buf), "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n",
             m_svgWidth, m_svgHeight);

    const std::string BEGIN = buf;
    const std::string END = "</svg>\n";

    string result;
    result += BEGIN;
    for (auto &shape : m_shapes)
    {
        result += "    " + shape->generate() + "\n";
    }
    result += END;
    return result;
}

Shape::Shape(int originX, int originY)
    : m_originX(originX)
    , m_originY(originY)
{

}

Shape::~Shape()
{

}

RectangleShape::RectangleShape(const RectangleData &rect, int originX, int originY)
    : Shape(originX, originY)
    , m_data(rect)
{

}

std::string RectangleShape::generate()
{
    char buf[512];
    snprintf(buf, sizeof(buf),
             "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:%s; stroke-width:%d; stroke:%s; stroke-dasharray:%s\"/>",
             m_data.x + m_originX,
             m_data.y + m_originY,
             m_data.width,
             m_data.height,
             m_data.fill_color.c_str(),
             m_data.stroke_width,
             m_data.stroke_color.c_str(),
             m_data.stroke_dasharray.c_str());
    return string(buf);
}

TextShape::TextShape(const TextData &text, int originX, int originY)
    : Shape(originX, originY)
    , m_data(text)
{

}

std::string TextShape::generate()
{
    char buf[512];
    snprintf(buf, sizeof(buf),
             "<text x=\"%d\" y=\"%d\" font-size=\"%d\" dominant-baseline=\"text-before-edge\">%s</text>",
             m_data.x + m_originX,
             m_data.y + m_originY,
             m_data.size,
             m_data.text.c_str());
    return string(buf);
}

EllipseShape::EllipseShape(const EllipseData &ellipse, int originX, int originY)
    : Shape(originX, originY)
    , m_data(ellipse)
{

}

string EllipseShape::generate()
{
    char buf[512];
    snprintf(buf, sizeof(buf),
             "<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" style=\"fill:%s; stroke-width:%d; stroke:%s; stroke-dasharray:%s\"/>",
             m_data.x + m_data.x_radius + m_originX,
             m_data.y + m_data.y_radius + m_originY,
             m_data.x_radius,
             m_data.y_radius,
             m_data.fill_color.c_str(),
             m_data.stroke_width,
             m_data.stroke_color.c_str(),
             m_data.stroke_dasharray.c_str());
    return string(buf);
}

PolygonShape::PolygonShape(const PolygonData &polygon, int originX, int originY)
    : Shape(originX, originY)
    , m_data(polygon)
{

}

string PolygonShape::generate()
{
    string points;
    for (auto &point : m_data.points)
    {
        assert(point.size() == 2);
        points += to_string(m_originX + m_data.x + point[0]) + "," + to_string(m_originY + m_data.y + point[1]) + " ";
    }
    char buf[4096];
    snprintf(buf, sizeof(buf),
             "<polygon points=\"%s\" style=\"fill:%s; stroke-width:%d; stroke:%s; stroke-dasharray:%s; fill-rule:%s\"/>",
             points.c_str(),
             m_data.fill_color.c_str(),
             m_data.stroke_width,
             m_data.stroke_color.c_str(),
             m_data.stroke_dasharray.c_str(),
             m_data.fill_rule.c_str());
    return string(buf);
}

}
