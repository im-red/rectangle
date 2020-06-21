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

class Shape
{
public:
    virtual ~Shape();
    virtual std::string generate() = 0;
};

class RectangleShape : public Shape
{
public:
    explicit RectangleShape(const RectangleData &rect);
    std::string generate() override;

private:
    RectangleData m_data;
};

class TextShape : public Shape
{
public:
    explicit TextShape(const TextData &text);
    std::string generate() override;

private:
    TextData m_data;
};

class SvgPainter
{
public:
    SvgPainter();

    void clear();

    void draw(const RectangleData &d);
    void draw(const TextData &d);

    std::string generate() const;

private:
    std::vector<std::unique_ptr<Shape>> m_shapes;
};

}
