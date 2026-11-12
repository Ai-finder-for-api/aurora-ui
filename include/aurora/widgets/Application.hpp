#pragma once
// Aurora UI - Application: headless harness, event injection, screenshot render
#include "Widget.hpp"

namespace aui {

class Application {
public:
    /// Lay out and paint the whole tree into an image (headless "screenshot").
    static Image renderToImage(Widget& root, Color bg = colors::White) {
        root.doLayout();
        Image img(std::max(1, root.width()), std::max(1, root.height()), bg);
        Painter p(img);
        root.render(p, {0, 0});
        return img;
    }
    static bool sendMouse(Widget& root, EventType t, Point pos,
                          MouseButton btn = MouseButton::Left,
                          MouseButton btns = MouseButton::Left) {
        MouseEvent e(t, pos, btn, btns);
        return root.dispatchInput(e);
    }
    static bool click(Widget& root, Point pos) {
        sendMouse(root, EventType::MousePress, pos);
        return sendMouse(root, EventType::MouseRelease, pos, MouseButton::Left, MouseButton::None);
    }
    static bool sendKey(Widget& root, int key, std::string text = {}, int mods = 0) {
        KeyEvent e(EventType::KeyPress, key, std::move(text), mods);
        return root.dispatchInput(e);
    }
    static bool typeText(Widget& root, const std::string& text) {
        for (char c : text) sendKey(root, c, std::string(1, c));
        return true;
    }
    static int exec() { return EventLoop::instance().exec(); }
    static void quit(int code = 0) { EventLoop::instance().exit(code); }
};

} // namespace aui
