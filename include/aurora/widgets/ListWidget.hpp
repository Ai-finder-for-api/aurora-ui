#pragma once
// Aurora UI - ListWidget (QListWidget analogue)
#include "Widget.hpp"

namespace aui {

class ListWidget : public Widget {
public:
    explicit ListWidget(Widget* parent = nullptr) : Widget(parent) {}
    void addItem(std::string text) { items_.push_back(std::move(text)); }
    void clear() { items_.clear(); current_ = -1; }
    int count() const { return int(items_.size()); }
    const std::string& item(int i) const { return items_[i]; }
    void setCurrentRow(int r) {
        if (r != current_ && r >= -1 && r < int(items_.size())) {
            current_ = r;
            if (r >= 0) currentRowChanged.emit(r);
        }
    }
    int currentRow() const { return current_; }
    void setRowHeight(int h) { rowH_ = h; }
    Size sizeHint() const override { return {180, 5 * rowH_}; }
    Signal<int> currentRowChanged;
    Signal<int> itemActivated;

protected:
    void onPaint(Painter& p, Rect abs) override {
        const auto& st = Style::current();
        const auto& pal = st.palette;
        p.fillRect(abs, pal.base);
        for (int i = 0; i < int(items_.size()); ++i) {
            Rect row{abs.x + 1, abs.y + 1 + i * rowH_ - scroll_, abs.w - 2, rowH_};
            if (row.y > abs.bottom() || row.bottom() < abs.y) continue;
            if (i == current_) p.fillRect(row, pal.highlight);
            else if (i == hovered_) p.fillRect(row, pal.alternateBase);
            p.setPen(i == current_ ? pal.highlightText : pal.text);
            p.drawText(row.adjusted(8, 0, 0, 0), Align::Left | Align::VCenter, items_[i]);
        }
        p.setPen(pal.border);
        p.drawRect(abs);
    }
    void onMousePress(MouseEvent& e) override {
        int r = (e.pos().y + scroll_ - 1) / rowH_;
        if (r >= 0 && r < int(items_.size())) { setCurrentRow(r); e.accept(); }
    }
    void onMouseMove(MouseEvent& e) override {
        hovered_ = (e.pos().y + scroll_ - 1) / rowH_;
    }
    void onWheel(WheelEvent& e) override {
        int maxScroll = std::max(0, int(items_.size()) * rowH_ - rect_.h);
        scroll_ = std::clamp(scroll_ - e.delta() / 4, 0, maxScroll);
        e.accept();
    }
    void onKeyPress(KeyEvent& e) override {
        if (e.key() == int(Key::Up)) { setCurrentRow(std::max(0, current_ - 1)); e.accept(); }
        else if (e.key() == int(Key::Down)) {
            setCurrentRow(std::min(int(items_.size()) - 1, current_ + 1));
            e.accept();
        } else if (e.key() == int(Key::Return) && current_ >= 0) {
            itemActivated.emit(current_);
            e.accept();
        }
    }
private:
    std::vector<std::string> items_;
    int current_ = -1, hovered_ = -1, scroll_ = 0, rowH_ = Painter::fontHeight() + 10;
};

} // namespace aui
