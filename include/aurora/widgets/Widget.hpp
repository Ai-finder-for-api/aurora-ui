#pragma once
// Aurora UI - Widget base class (QWidget analogue)
#include "../core/Event.hpp"
#include "../core/Object.hpp"
#include "../core/Property.hpp"
#include "../layout/Layout.hpp"
#include "../paint/Painter.hpp"
#include "../style/Style.hpp"

namespace aui {

class Widget : public Object {
public:
    explicit Widget(Widget* parent = nullptr) : Object(parent) {
        if (parent) parent->adopted(this);
    }
    ~Widget() override {
        delete layout_;
        if (focus_ == this) focus_ = nullptr;
    }

    // ---- geometry ----
    Rect geometry() const { return rect_; }            // relative to parent
    Rect rect() const { return {0, 0, rect_.w, rect_.h} };
    Size size() const { return rect_.size(); }
    int width() const { return rect_.w; }
    int height() const { return rect_.h; }
    virtual void setGeometry(Rect r) {
        if (r.size() != rect_.size()) {
            ResizeEvent ev(r.size(), rect_.size());
            rect_ = r;
            event(ev);
        } else rect_ = r;
        if (layout_) layout_->apply(rect().margined(layout_->margins()));
    }
    virtual Size sizeHint() const {
        if (layout_) return layout_->sizeHint();
        return {64, 32};
    }

    // ---- state ----
    Property<bool> visible{true};
    Property<bool> enabled{true};
    bool hasFocus() const { return focus_ == this; }
    void setFocus() { if (enabled.get() && visible.get()) focus_ = this; }
    static Widget* focusWidget() { return focus_; }
    void show() { visible = true; }
    void hide() { visible = false; }

    // ---- layout ----
    void setLayout(Layout* l) {
        delete layout_;
        layout_ = l;
        if (layout_ && !rect_.isEmpty())
            layout_->apply(rect().margined(layout_->margins()));
    }
    Layout* layout() const { return layout_; }
    void doLayout() {
        if (layout_) layout_->apply(rect().margined(layout_->margins()));
        for (auto* c : children())
            if (auto* w = dynamic_cast<Widget*>(c)) w->doLayout();
    }

    // ---- painting ----
    void render(Painter& p, Point origin) {
        if (!visible.get() || rect_.isEmpty()) return;
        Rect abs = rect_.translated(origin);
        Rect oldClip = p.clipRect();
        Rect clip = oldClip.intersected(abs);
        if (clip.isEmpty()) return;
        p.setClipRect(clip);
        PaintEvent pe(abs);
        event(pe);
        onPaint(p, abs);
        for (auto* c : children())
            if (auto* w = dynamic_cast<Widget*>(c)) w->render(p, abs.topLeft());
        p.setClipRect(oldClip);
    }

    // ---- event dispatch ----
    virtual bool event(Event& e) {
        switch (e.type()) {
        case EventType::MousePress: onMousePress(static_cast<MouseEvent&>(e)); break;
        case EventType::MouseRelease: onMouseRelease(static_cast<MouseEvent&>(e)); break;
        case EventType::MouseMove: onMouseMove(static_cast<MouseEvent&>(e)); break;
        case EventType::Wheel: onWheel(static_cast<WheelEvent&>(e)); break;
        case EventType::KeyPress: onKeyPress(static_cast<KeyEvent&>(e)); break;
        case EventType::KeyRelease: onKeyRelease(static_cast<KeyEvent&>(e)); break;
        case EventType::Resize: onResize(static_cast<ResizeEvent&>(e)); break;
        case EventType::Paint: break;
        default: break;
        }
        return e.isAccepted();
    }

    /// Deepest visible child at parent-local point (this widget if none).
    Widget* childAt(Point local) {
        for (auto it = children().rbegin(); it != children().rend(); ++it) {
            auto* w = dynamic_cast<Widget*>(*it);
            if (!w || !w->visible.get()) continue;
            if (w->rect_.contains(local))
                return w->childAt(local - w->rect_.topLeft());
        }
        return this;
    }

    /// Route an input event (position in this widget's coordinates).
    bool dispatchInput(Event& e) {
        if (auto* me = dynamic_cast<MouseEvent*>(&e)) {
            Widget* target = childAt(me->pos());
            Point rel = me->pos();
            for (Widget* w = target; w && w != this; ) {
                rel = rel - w->rect_.topLeft();
                w = dynamic_cast<Widget*>(w->parent());
            }
            MouseEvent local(me->type(), rel, me->button(), me->buttons(), me->modifiers());
            if (me->type() == EventType::MousePress) {
                pressed_ = target;
                target->setFocus();
            }
            return target->event(local);
        }
        if (e.type() == EventType::KeyPress || e.type() == EventType::KeyRelease)
            return focus_ ? focus_->event(e) : event(e);
        return event(e);
    }
    Widget* pressedWidget() const { return pressed_; }

protected:
    virtual void onPaint(Painter& p, Rect abs) {
        if (!parent()) p.fillRect(abs, Style::current().palette.window);
    }
    virtual void onMousePress(MouseEvent&) {}
    virtual void onMouseRelease(MouseEvent&) {}
    virtual void onMouseMove(MouseEvent&) {}
    virtual void onWheel(WheelEvent&) {}
    virtual void onKeyPress(KeyEvent&) {}
    virtual void onKeyRelease(KeyEvent&) {}
    virtual void onResize(ResizeEvent&) {}

    void adopted(Widget* w) {
        if (layout_) layout_->add(w);
    }

    Rect rect_{};
    Layout* layout_ = nullptr;
    inline static Widget* focus_ = nullptr;
    inline static Widget* pressed_ = nullptr;
};

// ---- Layout implementations (need complete Widget) ----
inline void Layout::add(Widget* w, int stretch) { items_.push_back({w, stretch}); }

inline Size BoxLayout::sizeHint() const {
    Size s{};
    for (auto& it : items_) {
        Size h = it.w->sizeHint();
        if (orient_ == Horizontal) { s.w += h.w; s.h = std::max(s.h, h.h); }
        else { s.h += h.h; s.w = std::max(s.w, h.w); }
    }
    int n = int(items_.size());
    int gap = n > 1 ? (n - 1) * effSpacing() : 0;
    if (orient_ == Horizontal) s.w += gap; else s.h += gap;
    s.w += margins_.horizontal(); s.h += margins_.vertical();
    return s;
}

inline void BoxLayout::apply(Rect content) {
    int n = int(items_.size());
    if (!n) return;
    bool horiz = orient_ == Horizontal;
    int total = horiz ? content.w : content.h;
    int gap = (n - 1) * effSpacing();
    int fixed = gap, totalStretch = 0;
    for (auto& it : items_) {
        if (it.stretch > 0) totalStretch += it.stretch;
        else fixed += horiz ? it.w->sizeHint().w : it.w->sizeHint().h;
    }
    int flex = std::max(0, total - fixed);
    int pos = horiz ? content.x : content.y;
    for (auto& it : items_) {
        int len = it.stretch > 0 && totalStretch > 0
                      ? flex * it.stretch / totalStretch
                      : (horiz ? it.w->sizeHint().w : it.w->sizeHint().h);
        Size hint = it.w->sizeHint();
        Rect r = horiz ? Rect{pos, content.y, len, content.h}
                       : Rect{content.x, pos, content.w, len};
        if (horiz) {
            int hh = std::min(hint.h > 0 ? hint.h : content.h, content.h);
            if (it.stretch == 0) r.h = content.h; // fill cross axis
            (void)hh;
        } else {
            if (it.stretch == 0) r.w = content.w;
        }
        it.w->setGeometry(r);
        pos += len + effSpacing();
    }
}

inline Size GridLayout::sizeHint() const {
    int cw = 0, ch = 0;
    for (auto& it : items_) {
        Size h = it.w->sizeHint();
        cw = std::max(cw, h.w); ch = std::max(ch, h.h);
    }
    return {cols_ * cw + (cols_ - 1) * effSpacing() + margins_.horizontal(),
            rows_ * ch + (rows_ - 1) * effSpacing() + margins_.vertical()};
}

inline void GridLayout::apply(Rect content) {
    int cw = (content.w - (cols_ - 1) * effSpacing()) / std::max(1, cols_);
    int ch = (content.h - (rows_ - 1) * effSpacing()) / std::max(1, rows_);
    int i = 0;
    for (auto& it : items_) {
        int r = i / cols_, c = i % cols_;
        if (r >= rows_) break;
        it.w->setGeometry({content.x + c * (cw + effSpacing()),
                           content.y + r * (ch + effSpacing()), cw, ch});
        ++i;
    }
}

} // namespace aui
