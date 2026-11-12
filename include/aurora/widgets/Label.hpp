#pragma once
// Aurora UI - Label (QLabel analogue)
#include "Widget.hpp"

namespace aui {

class Label : public Widget {
public:
    explicit Label(std::string text = {}, Widget* parent = nullptr)
        : Widget(parent), text_(std::move(text)) {}
    void setText(std::string t) { text_ = std::move(t); }
    const std::string& text() const { return text_; }
    void setAlignment(Align a) { align_ = a; }
    Size sizeHint() const override {
        return {Painter::textWidth(text_) + 4, Painter::fontHeight() + 4};
    }
protected:
    void onPaint(Painter& p, Rect abs) override {
        p.setPen(enabled.get() ? Style::current().palette.windowText
                               : Style::current().palette.disabled);
        p.drawText(abs, align_, text_);
    }
private:
    std::string text_;
    Align align_ = Align::Left | Align::VCenter;
};

} // namespace aui
