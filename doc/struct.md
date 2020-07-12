# Built-in struct

```cpp
struct svg_scene
{
    int leftMargin;
    int topMargin;
    int rightMargin;
    int bottomMargin;
    int width;
    int height;
};

struct svg_rect
{
    int x;
    int y;
    int width;
    int height;
    string fill_color;
    int stroke_width;
    string stroke_color;
    string stroke_dasharray;
};

struct svg_text
{
    int x;
    int y;
    int size;
    string text;
};

struct svg_ellipse
{
    int x;
    int y;
    int x_radius;
    int y_radius;
    string fill_color;
    int stroke_width;
    string stroke_color;
    string stroke_dasharray;
};

struct svg_polygon
{
    int x;
    int y;
    list<list<int>> points;
    string fill_color;
    int stroke_width;
    string stroke_color;
    string stroke_dasharray;
};
```
