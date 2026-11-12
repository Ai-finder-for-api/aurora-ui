#pragma once
// Aurora UI - 2D geometry primitives
#include <algorithm>

namespace aui {

struct Point {
    int x = 0, y = 0;
    Point operator+(const Point& o) const { return {x + o.x, y + o.y}; }
    Point operator-(const Point& o) const { return {x - o.x, y - o.y}; }
    bool operator==(const Point&) const = default;
};

struct Size {
    int w = 0, h = 0;
    Size expandedTo(const Size& o) const { return {std::max(w, o.w), std::max(h, o.h)}; }
    bool isValid() const { return w >= 0 && h >= 0; }
    bool operator==(const Size&) const = default;
};

struct Margins {
    int left = 0, top = 0, right = 0, bottom = 0;
    int horizontal() const { return left + right; }
    int vertical() const { return top + bottom; }
    bool operator==(const Margins&) const = default;
};

struct Rect {
    int x = 0, y = 0, w = 0, h = 0;
    int left() const { return x; }
    int top() const { return y; }
    int right() const { return x + w - 1; }
    int bottom() const { return y + h - 1; }
    Point center() const { return {x + w / 2, y + h / 2}; }
    Point topLeft() const { return {x, y}; }
    Size size() const { return {w, h}; }
    bool contains(Point p) const { return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h; }
    bool isEmpty() const { return w <= 0 || h <= 0; }
    Rect translated(Point d) const { return {x + d.x, y + d.y, w, h}; }
    Rect adjusted(int l, int t, int r, int b) const { return {x + l, y + t, w - l - r, h - t - b}; }
    Rect intersected(const Rect& o) const {
        int nx = std::max(x, o.x), ny = std::max(y, o.y);
        int nr = std::min(x + w, o.x + o.w), nb = std::min(y + h, o.y + o.h);
        return {nx, ny, std::max(0, nr - nx), std::max(0, nb - ny)};
    }
    Rect united(const Rect& o) const {
        if (isEmpty()) return o;
        if (o.isEmpty()) return *this;
        int nx = std::min(x, o.x), ny = std::min(y, o.y);
        return {nx, ny, std::max(x + w, o.x + o.w) - nx, std::max(y + h, o.y + o.h) - ny};
    }
    Rect margined(Margins m) const { return adjusted(m.left, m.top, m.right, m.bottom); }
    bool operator==(const Rect&) const = default;
};

} // namespace aui
