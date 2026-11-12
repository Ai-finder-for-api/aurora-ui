#pragma once
// Aurora UI - 2D software painter (QPainter analogue)
#include <algorithm>
#include <string_view>
#include "../core/Geometry.hpp"
#include "FontData.hpp"
#include "Image.hpp"

namespace aui {

enum class Align { Left = 1, HCenter = 2, Right = 4, Top = 8, VCenter = 16, Bottom = 32 };
inline Align operator|(Align a, Align b) { return Align(int(a) | int(b)); }

class Painter {
public:
    explicit Painter(Image& img) : img_(img), clip_({0, 0, img.width(), img.height()}) {}

    void setPen(Color c) { pen_ = c; }
    void setBrush(Color c) { brush_ = c; }
    Color pen() const { return pen_; }

    void setClipRect(Rect r) { clip_ = Rect{0, 0, img_.width(), img_.height()}.intersected(r); }
    void resetClip() { clip_ = {0, 0, img_.width(), img_.height()}; }
    Rect clipRect() const { return clip_; }

    void fillRect(Rect r, Color c) {
        r = r.intersected(clip_);
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x) img_.blendPixel(x, y, c);
    }
    void fillRect(Rect r) { fillRect(r, brush_); }

    void drawRect(Rect r) { drawRect(r, pen_); }
    void drawRect(Rect r, Color c) {
        fillRect({r.x, r.y, r.w, 1}, c);
        fillRect({r.x, r.y + r.h - 1, r.w, 1}, c);
        fillRect({r.x, r.y, 1, r.h}, c);
        fillRect({r.x + r.w - 1, r.y, 1, r.h}, c);
    }

    void drawLine(Point a, Point b) { drawLine(a, b, pen_); }
    void drawLine(Point a, Point b, Color c) {
        int dx = std::abs(b.x - a.x), sx = a.x < b.x ? 1 : -1;
        int dy = -std::abs(b.y - a.y), sy = a.y < b.y ? 1 : -1;
        int err = dx + dy;
        for (;;) {
            if (clip_.contains(a)) img_.blendPixel(a.x, a.y, c);
            if (a == b) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; a.x += sx; }
            if (e2 <= dx) { err += dx; a.y += sy; }
        }
    }

    void drawEllipse(Rect r) {
        for (int y = 0; y < r.h; ++y)
            for (int x = 0; x < r.w; ++x) {
                double nx = (x - r.w / 2.0 + 0.5) / (r.w / 2.0);
                double ny = (y - r.h / 2.0 + 0.5) / (r.h / 2.0);
                double d = nx * nx + ny * ny;
                if (std::abs(d - 1.0) < 4.0 / std::max(2, std::min(r.w, r.h)))
                    if (clip_.contains({r.x + x, r.y + y}))
                        img_.blendPixel(r.x + x, r.y + y, pen_);
            }
    }
    void fillEllipse(Rect r, Color c) {
        for (int y = 0; y < r.h; ++y)
            for (int x = 0; x < r.w; ++x) {
                double nx = (x - r.w / 2.0 + 0.5) / (r.w / 2.0);
                double ny = (y - r.h / 2.0 + 0.5) / (r.h / 2.0);
                if (nx * nx + ny * ny <= 1.0 && clip_.contains({r.x + x, r.y + y}))
                    img_.blendPixel(r.x + x, r.y + y, c);
            }
    }

    void drawRoundedRect(Rect r, int radius) { // outline
        fillRect({r.x + radius, r.y, r.w - 2 * radius, 1}, pen_);
        fillRect({r.x + radius, r.y + r.h - 1, r.w - 2 * radius, 1}, pen_);
        fillRect({r.x, r.y + radius, 1, r.h - 2 * radius}, pen_);
        fillRect({r.x + r.w - 1, r.y + radius, 1, r.h - 2 * radius}, pen_);
        for (int q = 0; q < 4; ++q) {
            int cx = (q == 0 || q == 3) ? r.x + radius : r.x + r.w - 1 - radius;
            int cy = (q < 2) ? r.y + radius : r.y + r.h - 1 - radius;
            for (int y = 0; y <= radius; ++y)
                for (int x = 0; x <= radius; ++x) {
                    int d2 = x * x + y * y;
                    if (std::abs(d2 - radius * radius) <= radius) {
                        int px = (q == 0 || q == 3) ? cx - x : cx + x;
                        int py = (q < 2) ? cy - y : cy + y;
                        if (clip_.contains({px, py})) img_.blendPixel(px, py, pen_);
                    }
                }
        }
    }
    void fillRoundedRect(Rect r, int radius, Color c) {
        fillRect({r.x + radius, r.y, r.w - 2 * radius, r.h}, c);
        fillRect({r.x, r.y + radius, radius, r.h - 2 * radius}, c);
        fillRect({r.x + r.w - radius, r.y + radius, radius, r.h - 2 * radius}, c);
        for (int q = 0; q < 4; ++q) {
            int cx = (q == 0 || q == 3) ? r.x + radius : r.x + r.w - 1 - radius;
            int cy = (q < 2) ? r.y + radius : r.y + r.h - 1 - radius;
            for (int y = 0; y <= radius; ++y)
                for (int x = 0; x <= radius; ++x)
                    if (x * x + y * y <= radius * radius) {
                        int px = (q == 0 || q == 3) ? cx - x : cx + x;
                        int py = (q < 2) ? cy - y : cy + y;
                        if (clip_.contains({px, py})) img_.blendPixel(px, py, c);
                    }
        }
    }

    // ---- text (built-in bitmap font) ----
    static int textWidth(std::string_view s) {
        int w = 0;
        for (char ch : s) w += advance(ch);
        return w;
    }
    static int fontHeight() { return font::kLineHeight; }

    void drawText(Point baselineTop, std::string_view s) { drawText(baselineTop, s, pen_); }
    void drawText(Point pos, std::string_view s, Color c) {
        int x = pos.x;
        for (char ch : s) {
            drawGlyph(x, pos.y, ch, c);
            x += advance(ch);
        }
    }
    void drawText(Rect r, Align a, std::string_view s) {
        int tw = textWidth(s), th = font::kLineHeight;
        int x = r.x, y = r.y;
        if (int(a) & int(Align::HCenter)) x = r.x + (r.w - tw) / 2;
        else if (int(a) & int(Align::Right)) x = r.x + r.w - tw;
        if (int(a) & int(Align::VCenter)) y = r.y + (r.h - th) / 2;
        else if (int(a) & int(Align::Bottom)) y = r.y + r.h - th;
        drawText({x, y}, s);
    }

private:
    static int glyphIndex(char ch) {
        if (ch < font::kFirst || ch > font::kLast) ch = '?';
        return ch - font::kFirst;
    }
    static int advance(char ch) { return font::kAdvances[glyphIndex(ch)]; }
    static int bitOffset(int idx) {
        int off = 0;
        for (int i = 0; i < idx; ++i)
            off += ((font::kWidths[i] + 7) / 8) * font::kLineHeight;
        return off;
    }
    void drawGlyph(int x, int y, char ch, Color c) {
        int idx = glyphIndex(ch);
        int w = font::kWidths[idx];
        int bytesPerRow = (w + 7) / 8;
        int off = bitOffset(idx);
        for (int row = 0; row < font::kLineHeight; ++row)
            for (int bx = 0; bx < bytesPerRow; ++bx) {
                std::uint8_t byte = font::kBits[off + row * bytesPerRow + bx];
                for (int bit = 0; bit < 8 && bx * 8 + bit < w; ++bit)
                    if (byte & (1 << bit)) {
                        Point p{x + bx * 8 + bit, y + row};
                        if (clip_.contains(p)) img_.blendPixel(p.x, p.y, c);
                    }
            }
    }
    Image& img_;
    Color pen_ = colors::Black, brush_ = colors::White;
    Rect clip_;
};

} // namespace aui
