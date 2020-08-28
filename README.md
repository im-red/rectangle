# Rectangle

Rectangle is a DSL for generating SVG image. You can use QML-like syntax to describe graphics. And then output these graphics in SVG format.

## Example

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

![example.svg](./example.svg)

