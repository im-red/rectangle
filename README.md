# Rectangle

Rectangle is a DSL for generating SVG image. You can use QML-like syntax to define graphic instances. And then output these instances in SVG format. In addition, you can use C-like syntax to define graphic components and reuse it in defining graphic instances.

## Example

### Graphic instance

The `Scene`, `Rectangle` and `Text` are all built-in components.

Rectangle input:

```
Scene {
    width: 175
    height: 150
    Rectangle {
        width: 100
        height: 100
        fill_color: "red"
    }
    Rectangle {
        x: 25
        y: 25
        width: 100
        height: 100
        fill_color: "yellow"
    }
    Rectangle {
        x: 50
        width: 100
        height: 100
        fill_color: "blue"
    }
    Rectangle {
        x: 75
        y: 25
        width: 100
        height: 100
        fill_color: "green"
    }
    Text {
        y: 125
        size: 20
        text: "Rectangle"
    }
}
```

SVG output:

![example.svg](./example/example.svg)

### Graphic component

We can define a new graphic component `Fanlaoshi`, and use it in graphic instance.

Fanlaoshi.rect:

```
def Fanlaoshi {
    int x: 0
    int y: 0
    int width: 100
    int height: 100
    string color: "black"

    void draw() {
        svg_polyline p;
        p.x = x;
        p.y = y;
        p.stroke_width = 1;
        p.stroke_color = color;
        p.stroke_dasharray = "1,0";
        list<list<int>> hair1Points = {{9 * width / 100, 38 * height / 100}, {16 * width / 100, 20 * height / 100}, {35 * width / 100, 13 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair1Points;
        drawPolyline(p);
        list<list<int>> hair2Points = {{12 * width / 100, 49 * height / 100}, {23 * width / 100, 23 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair2Points;
        drawPolyline(p);
        list<list<int>> hair3Points = {{18 * width / 100, 46 * height / 100}, {27 * width / 100, 28 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair3Points;
        drawPolyline(p);
        list<list<int>> hair4Points = {{25 * width / 100, 45 * height / 100}, {32 * width / 100, 32 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair4Points;
        drawPolyline(p);
        list<list<int>> hair5Points = {{29 * width / 100, 49 * height / 100}, {37 * width / 100, 36 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair5Points;
        drawPolyline(p);
        list<list<int>> hair6Points = {{66 * width / 100, 48 * height / 100}, {68 * width / 100, 38 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair6Points;
        drawPolyline(p);
        list<list<int>> hair7Points = {{76 * width / 100, 46 * height / 100}, {80 * width / 100, 32 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair7Points;
        drawPolyline(p);
        list<list<int>> hair8Points = {{88 * width / 100, 41 * height / 100}, {89 * width / 100, 23 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair8Points;
        drawPolyline(p);
        list<list<int>> hair9Points = {{93 * width / 100, 36 * height / 100}, {95 * width / 100, 19 * height / 100}, {71 * width / 100, 12 * height / 100}, {47 * width / 100, 21 * height / 100}};
        p.points = hair9Points;
        drawPolyline(p);
        list<list<int>> facePoints = {{18 * width / 100, 46 * height / 100}, {18 * width / 100, 74 * height / 100}, {23 * width / 100, 83 * height / 100}, {33 * width / 100, 88 * height / 100}, {63 * width / 100, 88 * height / 100}, {74 * width / 100, 84 * height / 100}, {81 * width / 100, 76 * height / 100}, {82 * width / 100, 45 * height / 100}};
        p.points = facePoints;
        drawPolyline(p);
        list<list<int>> ear1Points = {{17 * width / 100, 50 * height / 100}, {12 * width / 100, 52 * height / 100}, {10 * width / 100, 61 * height / 100}, {18 * width / 100, 64 * height / 100}};
        p.points = ear1Points;
        drawPolyline(p);
        list<list<int>> ear2Points = {{82 * width / 100, 49 * height / 100}, {89 * width / 100, 54 * height / 100}, {88 * width / 100, 61 * height / 100}, {81 * width / 100, 64 * height / 100}};
        p.points = ear2Points;
        drawPolyline(p);
        list<list<int>> glass1Points = {{18 * width / 100, 50 * height / 100}, {29 * width / 100, 57 * height / 100}};
        p.points = glass1Points;
        drawPolyline(p);
        list<list<int>> glass2Points = {{47 * width / 100, 58 * height / 100}, {55 * width / 100, 58 * height / 100}};
        p.points = glass2Points;
        drawPolyline(p);
        list<list<int>> glass3Points = {{73 * width / 100, 55 * height / 100}, {82 * width / 100, 48 * height / 100}};
        p.points = glass3Points;
        drawPolyline(p);
        list<list<int>> glass4Points = {{30 * width / 100, 53 * height / 100}, {46 * width / 100, 52 * height / 100}, {46 * width / 100, 63 * height / 100}, {30 * width / 100, 62 * height / 100}, {30 * width / 100, 53 * height / 100}};
        p.points = glass4Points;
        drawPolyline(p);
        list<list<int>> glass5Points = {{56 * width / 100, 53 * height / 100}, {71 * width / 100, 52 * height / 100}, {71 * width / 100, 63 * height / 100}, {57 * width / 100, 64 * height / 100}, {56 * width / 100, 53 * height / 100}};
        p.points = glass5Points;
        drawPolyline(p);
        list<list<int>> nousePoints = {{49 * width / 100, 68 * height / 100}, {47 * width / 100, 69 * height / 100}, {48 * width / 100, 70 * height / 100}, {50 * width / 100, 70 * height / 100}};
        p.points = nousePoints;
        drawPolyline(p);
        list<list<int>> nosePoints = {{49 * width / 100, 68 * height / 100}, {47 * width / 100, 69 * height / 100}, {48 * width / 100, 70 * height / 100}, {50 * width / 100, 70 * height / 100}};
        p.points = nosePoints;
        drawPolyline(p);
        list<list<int>> mousePoints = {{42 * width / 100, 75 * height / 100}, 
            {41 * width / 100, 77 * height / 100}, 
            {41 * width / 100, 79 * height / 100}, 
            {44 * width / 100, 80 * height / 100}, 
            {47 * width / 100, 77 * height / 100}, 
            {48 * width / 100, 74 * height / 100}, 
            {50 * width / 100, 78 * height / 100}, 
            {52 * width / 100, 80 * height / 100}, 
            {56 * width / 100, 80 * height / 100}, 
            {57 * width / 100, 78 * height / 100}, 
            {55 * width / 100, 75 * height / 100}
        };
        p.points = mousePoints;
        drawPolyline(p);

        svg_ellipse e;
        e.x_radius = 2 * width / 100;
        e.y_radius = 2 * height / 100;
        e.fill_color = color;
        e.stroke_width = 1;
        e.stroke_color = color;
        e.stroke_dasharray = "1,0";
        e.x = x + 37 * width / 100;
        e.y = y + 57 * height / 100;
        drawEllipse(e);
        e.x = x + 60 * width / 100;
        e.y = y + 58 * height / 100;
        drawEllipse(e);
    }
}
```

FanlaoshiInstance.rect:

```
Scene {
    width: 400
    height: 500
    Rectangle {
        width: parent.width
        height: 100
        fill_color: "#888888"
    }
    Fanlaoshi {
        color: "red"
    }
    Fanlaoshi {
        x: 100
        color: "yellow"
    }
    Fanlaoshi {
        x: 200
        color: "blue"
    }
    Fanlaoshi {
        x: 300
        color: "green"
    }
    Fanlaoshi {
        y: 100
        width: 400
        height: 400
    }
}
```

SVG output:

![Fanlaoshi.svg](./example/Fanlaoshi.svg)
