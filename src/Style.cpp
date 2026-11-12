#include "aurora/style/Style.hpp"

namespace aui {

Style Style::light() {
    return {Palette{
        Color::rgb(0xF5F6F8), // window
        Color::rgb(0x1B1F24), // windowText
        Color::rgb(0xFFFFFF), // base
        Color::rgb(0xF0F2F5), // alternateBase
        Color::rgb(0x1B1F24), // text
        Color::rgb(0x0D6EFD), // button
        Color::rgb(0xFFFFFF), // buttonText
        Color::rgb(0x0D6EFD), // highlight
        Color::rgb(0xFFFFFF), // highlightText
        Color::rgb(0x9AA0A6), // disabled
        Color::rgb(0xC9CDD3), // border
        Color::rgb(0x8A9199), // placeholder
    }};
}

Style Style::dark() {
    return {Palette{
        Color::rgb(0x1E2126),
        Color::rgb(0xE6E8EB),
        Color::rgb(0x26292F),
        Color::rgb(0x2D3138),
        Color::rgb(0xE6E8EB),
        Color::rgb(0x3B82F6),
        Color::rgb(0xFFFFFF),
        Color::rgb(0x3B82F6),
        Color::rgb(0xFFFFFF),
        Color::rgb(0x6B7076),
        Color::rgb(0x3A3F46),
        Color::rgb(0x7C828A),
    }};
}

static Style g_style = Style::light();
const Style& Style::current() { return g_style; }
void Style::set(const Style& s) { g_style = s; }

} // namespace aui
