#pragma once
// Aurora UI - ProgressBar (QProgressBar analogue)
#include "Widget.hpp"

namespace aui {

class ProgressBar : public Widget {
public:
    explicit ProgressBar(Widget* parent = nullptr) : Widget(parent) {}
    void setRange(int lo, int hi) { min_ = lo; max_ = std::max(lo, hi); setValue(value_); }
    void setValue(int v) {
        v = std::clamp(v, min_, max_);
        if (v != value_) { value_ = v; valueChanged.emit(v); }
    }
    int value() const { return value_; }
    void setTextVisible(bool v) { textVisible_ = v; }
    Size sizeHint() const override { return {160, 22}; }
    Signal<int> valueChanged;

protected:
    void onPaint(Painter& p, Rect abs) override {
        const auto& st = Style::current();
        const auto& pal = st.palette;
        p.fillRoundedRect(abs, st.radius, pal.alternateBase);
        double t = max_ > min_ ? double(value_ - min_) / (max_ - min_) : 0.0;
        if (t > 0)
            p.fillRoundedRect({abs.x, abs.y, int(t * abs.w), abs.h}, st.radius, pal.highlight);
        p.setPen(pal.border);
        p.drawRoundedRect(abs, st.radius);
        if (textVisible_) {
            char buf[16];
            std::snprintf(buf, sizeof buf, "%d%%", int(t * 100));
            p.setPen(pal.windowText);
            p.drawText(abs, Align::HCenter | Align::VCenter, buf);
        }
    }
private:
    int min_ = 0, max_ = 100, value_ = 0;
    bool textVisible_ = true;
};

} // namespace aui
