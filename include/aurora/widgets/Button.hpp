#pragma once
// Aurora UI - PushButton (QPushButton analogue)
#include "Widget.hpp"

namespace aui {

class Button : public Widget {
public:
    explicit Button(std::string text = {}, Widget* parent = nullptr)
        : Widget(parent), text_(std::move(text)) {}
    void setText(std::string t) { text_ = std::move(t); }
    const std::string& text() const { return text_; }
    void setFlat(bool f) { flat_ = f; }
    Size sizeHint() const override {
        const auto& st = Style::current();
        return {Painter::textWidth(text_) + 2 * st.paddingH + 8,
                Painter::fontHeight() + 2 * st.paddingV};
    }
    Signal<> clicked;

protected:
    void onPaint(Painter& p, Rect abs) override {
        const auto& st = Style::current();
        const auto& pal = st.palette;
        Color bg = !enabled.get() ? pal.button.darker(160)
                 : down_          ? pal.button.darker(115)
                 : hovered_       ? pal.button.lighter(112)
                                  : pal.button;
        if (flat_ && !hovered_ && !down_) bg = pal.window;
        p.fillRoundedRect(abs, st.radius, bg);
        p.setPen(flat_ && !hovered_ && !down_ ? pal.border : pal.button.darker(130));
        p.drawRoundedRect(abs, st.radius);
        p.setPen(enabled.get() ? pal.buttonText : pal.disabled);
        p.drawText(abs, Align::HCenter | Align::VCenter, text_);
        if (hasFocus()) {
            p.setPen(pal.highlightText.withAlpha(160));
            p.drawRoundedRect(abs.adjusted(3, 3, 3, 3), st.radius - 2);
        }
    }
    void onMousePress(MouseEvent& e) override {
        if (e.button() == MouseButton::Left) { down_ = true; e.accept(); }
    }
    void onMouseRelease(MouseEvent& e) override {
        if (down_ && e.button() == MouseButton::Left) {
            down_ = false;
            if (rect().contains(e.pos())) clicked.emit();
            e.accept();
        }
    }
    void onMouseMove(MouseEvent&) override { hovered_ = true; }
    void onKeyPress(KeyEvent& e) override {
        if (e.key() == int(Key::Return) || e.key() == int(Key::Enter) ||
            e.key() == int(Key::Space)) { clicked.emit(); e.accept(); }
    }
private:
    std::string text_;
    bool down_ = false, hovered_ = false, flat_ = false;
};

} // namespace aui
