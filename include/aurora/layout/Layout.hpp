#pragma once
// Aurora UI - Layouts: Box & Grid (QBoxLayout/QGridLayout analogues)
#include <algorithm>
#include <vector>
#include "../core/Geometry.hpp"
#include "../style/Style.hpp"

namespace aui {

class Widget;

class Layout {
public:
    enum Direction { LeftToRight, TopToBottom };
    virtual ~Layout() = default;

    void add(Widget* w, int stretch = 0);
    void setSpacing(int s) { spacing_ = s; }
    int spacing() const { return spacing_; }
    void setMargins(Margins m) { margins_ = m; }
    Margins margins() const { return margins_; }

    virtual Size sizeHint() const = 0;
    virtual void apply(Rect content) = 0;

protected:
    struct Item { Widget* w; int stretch; };
    std::vector<Item> items_;
    int spacing_ = -1; // -1 = style default
    Margins margins_{};
    int effSpacing() const { return spacing_ >= 0 ? spacing_ : Style::current().spacing; }
};

class BoxLayout : public Layout {
public:
    enum Orientation { Horizontal, Vertical };
    explicit BoxLayout(Orientation o) : orient_(o) {}
    Size sizeHint() const override;
    void apply(Rect content) override;
private:
    Orientation orient_;
};

class GridLayout : public Layout {
public:
    GridLayout(int rows, int cols) : rows_(rows), cols_(cols) {}
    Size sizeHint() const override;
    void apply(Rect content) override;
private:
    int rows_, cols_;
};

} // namespace aui
