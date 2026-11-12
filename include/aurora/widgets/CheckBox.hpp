#pragma once
// Aurora UI - CheckBox (QCheckBox analogue)
#include "Widget.hpp"

namespace aui {

class CheckBox : public Widget {
public:
    explicit CheckBox(std::string text = {}, Widget* parent = nullptr)
        : Widget(parent), text_(std::move(text)) {}
    void setChecked(bool c) { if (checked_ != c) { checked_ = c; toggled.emit(c); } }
    bool isChecked() const { return checked_; }
    Size sizeHint() const override {
        return {20 + 6 + Painter::textWidth(text_), std::max(20, Painter::fontHeight() + 2)};
    }
    Signal<bool> toggled;

protected:
    void onPaint(Painter& p, Rect abs) override {
        const auto& st = Style::current();
        const auto& pal = st.palette;
        Rect box{abs.x, abs.y + (abs.h - 18) / 2, 18, 18};
        p.fillRoundedRect(box, 4, checked_ ? pal.highlight : pal.base);
        p.setPen(checked_ ? pal.highlight : pal.border);
        p.drawRoundedRect(box, 4);
        if (checked_) {
            p.setPen(pal.highlightText);
            p.drawLine({box.x + 4, box.y + 9}, {box.x + 8, box.y + 13});
            p.drawLine({box.x + 8, box.y + 13}, {box.x + 14, box.y + 5});
            p.drawLine({box.x + 4, box.y + 10}, {box.x + 8, box.y + 14});
            p.drawLine({box.x + 8, box.y + 14}, {box.x + 14, box.y + 6});
        }
        p.setPen(enabled.get() ? pal.windowText : pal.disabled);
        p.drawText({abs.x + 24, abs.y + (abs.h - Painter::fontHeight()) / 2}, text_);
    }
    void onMouseRelease(MouseEvent& e) override {
        if (e.button() == MouseButton::Left && rect().contains(e.pos())) {
            setChecked(!checked_);
            e.accept();
        }
    }
    void onKeyPress(KeyEvent& e) override {
        if (e.key() == int(Key::Space)) { setChecked(!checked_); e.accept(); }
    }
private:
    std::string text_;
    bool checked_ = false;
};

} // namespace aui
