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
#include <memory>

namespace draw
{

struct Point
{
    Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
    int x;
    int y;
};

struct SceneData
{
    int leftMargin;
    int topMargin;
    int rightMargin;
    int bottomMargin;
    int width;
    int height;
};

struct RectangleData
{
    int x;
    int y;
    int width;
    int height;
    std::string fill_color;
    int stroke_width;
    std::string stroke_color;
    std::string stroke_dasharray;
};

struct TextData
{
    int x;
    int y;
    int size;
    std::string text;
};

struct EllipseData
{
    int x;
    int y;
    int x_radius;
    int y_radius;
    std::string fill_color;
    int stroke_width;
    std::string stroke_color;
    std::string stroke_dasharray;
};

struct PolygonData
{
    int x;
    int y;
    std::vector<std::vector<int>> points;
    std::string fill_color;
    std::string fill_rule;
    int stroke_width;
    std::string stroke_color;
    std::string stroke_dasharray;
};

class Shape
{
public:
    Shape(int originX = 0, int originY = 0);
    virtual ~Shape();
    virtual std::string generate() = 0;

protected:
    int m_originX;
    int m_originY;
};

class RectangleShape : public Shape
{
public:
    explicit RectangleShape(const RectangleData &rect, int originX, int originY);
    std::string generate() override;

private:
    RectangleData m_data;
};

class TextShape : public Shape
{
public:
    explicit TextShape(const TextData &text, int originX, int originY);
    std::string generate() override;

private:
    TextData m_data;
};

class EllipseShape : public Shape
{
public:
    explicit EllipseShape(const EllipseData &ellipse, int originX, int originY);
    std::string generate() override;

private:
    EllipseData m_data;
};

class PolygonShape : public Shape
{
public:
    explicit PolygonShape(const PolygonData &polygon, int originX, int originY);
    std::string generate() override;

private:
    PolygonData m_data;
};

class SvgPainter
{
public:
    SvgPainter();

    void clear();

    void defineScene(const SceneData &d);

    void pushOrigin(int x, int y);
    void popOrigin();

    void draw(const RectangleData &d);
    void draw(const TextData &d);
    void draw(const EllipseData &d);
    void draw(const PolygonData &d);

    std::string generate() const;

private:
    std::vector<std::unique_ptr<Shape>> m_shapes;
    std::vector<Point> m_originStack;
    Point m_curOrigin;

    int m_svgWidth = 0;
    int m_svgHeight = 0;

    int m_leftMargin = 10;
    int m_rightMargin = 10;
    int m_topMargin = 10;
    int m_bottomMargin = 10;
};

}
