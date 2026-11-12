#pragma once
// Aurora UI - Slider (QSlider analogue)
#include "Widget.hpp"

namespace aui {

class Slider : public Widget {
public:
    explicit Slider(Widget* parent = nullptr) : Widget(parent) {}
    void setRange(int lo, int hi) { min_ = lo; max_ = std::max(lo, hi); setValue(value_); }
    void setValue(int v) {
        v = std::clamp(v, min_, max_);
        if (v != value_) { value_ = v; valueChanged.emit(v); }
    }
    int value() const { return value_; }
    Size sizeHint() const override { return {120, 24}; }
    Signal<int> valueChanged;

protected:
    void onPaint(Painter& p, Rect abs) override {
        const auto& pal = Style::current().palette;
        int cy = abs.y + abs.h / 2;
        double t = max_ > min_ ? double(value_ - min_) / (max_ - min_) : 0.0;
        int hx = abs.x + 8 + int(t * (abs.w - 16));
        p.fillRoundedRect({abs.x, cy - 2, abs.w, 4}, 2, pal.border);
        p.fillRoundedRect({abs.x, cy - 2, hx - abs.x, 4}, 2, pal.highlight);
        p.fillEllipse({hx - 8, cy - 8, 16, 16}, enabled.get() ? pal.highlight : pal.disabled);
        p.setPen(pal.highlight.darker(140));
        p.drawEllipse({hx - 8, cy - 8, 16, 16});
    }
    void onMousePress(MouseEvent& e) override { jump(e.pos()); e.accept(); }
    void onMouseMove(MouseEvent& e) override {
        if (e.buttons() == MouseButton::Left) jump(e.pos());
    }
    void onKeyPress(KeyEvent& e) override {
        if (e.key() == int(Key::Left)) { setValue(value_ - 1); e.accept(); }
        else if (e.key() == int(Key::Right)) { setValue(value_ + 1); e.accept(); }
    }
private:
    void jump(Point local) {
        double t = std::clamp((local.x - 8) / double(std::max(1, rect_.w - 16)), 0.0, 1.0);
        setValue(min_ + int(std::round(t * (max_ - min_))));
    }
    int min_ = 0, max_ = 100, value_ = 0;
};

} // namespace aui
