#pragma once
// Aurora UI - LineEdit (QLineEdit analogue)
#include "Widget.hpp"

namespace aui {

class LineEdit : public Widget {
public:
    explicit LineEdit(std::string placeholder = {}, Widget* parent = nullptr)
        : Widget(parent), placeholder_(std::move(placeholder)) {}
    void setText(std::string t) {
        if (t != text_) { text_ = std::move(t); cursor_ = int(text_.size()); textChanged.emit(text_); }
    }
    const std::string& text() const { return text_; }
    Size sizeHint() const override {
        const auto& st = Style::current();
        return {160, Painter::fontHeight() + 2 * st.paddingV + 4};
    }
    Signal<const std::string&> textChanged;
    Signal<> returnPressed;

protected:
    void onPaint(Painter& p, Rect abs) override {
        const auto& st = Style::current();
        const auto& pal = st.palette;
        p.fillRoundedRect(abs, st.radius, enabled.get() ? pal.base : pal.window);
        p.setPen(hasFocus() ? pal.highlight : pal.border);
        p.drawRoundedRect(abs, st.radius);
        int ty = abs.y + (abs.h - Painter::fontHeight()) / 2;
        int tx = abs.x + st.paddingH;
        if (text_.empty()) {
            p.setPen(pal.placeholder);
            p.drawText({tx, ty}, placeholder_);
        } else {
            p.setPen(enabled.get() ? pal.text : pal.disabled);
            p.drawText({tx, ty}, text_);
        }
        if (hasFocus()) { // caret
            int cx = tx + Painter::textWidth(std::string_view(text_).substr(0, cursor_));
            p.fillRect({cx, ty, 1, Painter::fontHeight()}, pal.text);
        }
    }
    void onMousePress(MouseEvent& e) override {
        setFocus();
        const auto& st = Style::current();
        int rel = e.pos().x - st.paddingH;
        cursor_ = int(text_.size());
        for (int i = 0; i <= int(text_.size()); ++i)
            if (Painter::textWidth(std::string_view(text_).substr(0, i)) > rel) {
                cursor_ = i - (i > 0 ? 1 : 0);
                break;
            }
        e.accept();
    }
    void onKeyPress(KeyEvent& e) override {
        e.accept();
        switch (e.key()) {
        case int(Key::Backspace):
            if (cursor_ > 0) {
                text_.erase(cursor_ - 1, 1);
                --cursor_;
                textChanged.emit(text_);
            }
            break;
        case int(Key::Delete):
            if (cursor_ < int(text_.size())) {
                text_.erase(cursor_, 1);
                textChanged.emit(text_);
            }
            break;
        case int(Key::Left): cursor_ = std::max(0, cursor_ - 1); break;
        case int(Key::Right): cursor_ = std::min(int(text_.size()), cursor_ + 1); break;
        case int(Key::Home): cursor_ = 0; break;
        case int(Key::End): cursor_ = int(text_.size()); break;
        case int(Key::Return): case int(Key::Enter): returnPressed.emit(); break;
        default:
            if (!e.text().empty() && e.text()[0] >= 32 && e.text()[0] < 127) {
                text_.insert(cursor_, e.text());
                cursor_ += int(e.text().size());
                textChanged.emit(text_);
            } else e.ignore();
        }
    }
private:
    std::string text_, placeholder_;
    int cursor_ = 0;
};

} // namespace aui
