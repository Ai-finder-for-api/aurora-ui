#pragma once
// Aurora UI - Theme/palette (QPalette + QStyle analogue)
#include "../paint/Color.hpp"

namespace aui {

struct Palette {
    Color window;        // background
    Color windowText;
    Color base;          // input background
    Color alternateBase;
    Color text;
    Color button;
    Color buttonText;
    Color highlight;     // accent
    Color highlightText;
    Color disabled;      // disabled text
    Color border;
    Color placeholder;
};

struct Style {
    Palette palette;
    int radius = 6;          // corner radius
    int paddingH = 12;       // horizontal content padding
    int paddingV = 7;
    int spacing = 8;         // layout spacing
    int borderWidth = 1;
    int fontSize = 14;

    static const Style& current();
    static void set(const Style& s);
    static Style light();
    static Style dark();
};

} // namespace aui
