#pragma once
// Aurora UI - RGBA color
#include <cstdint>

namespace aui {

struct Color {
    std::uint8_t r = 0, g = 0, b = 0, a = 255;
    constexpr Color() = default;
    constexpr Color(std::uint8_t r_, std::uint8_t g_, std::uint8_t b_, std::uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}
    static constexpr Color rgb(std::uint32_t hex) {
        return {static_cast<std::uint8_t>(hex >> 16), static_cast<std::uint8_t>(hex >> 8),
                static_cast<std::uint8_t>(hex)};
    }
    Color withAlpha(std::uint8_t na) const { return {r, g, b, na}; }
    Color lighter(int f = 120) const {
        auto l = [f](std::uint8_t c) {
            return static_cast<std::uint8_t>(c + (255 - c) * (f - 100) / 100);
        };
        return {l(r), l(g), l(b), a};
    }
    Color darker(int f = 120) const {
        auto d = [f](std::uint8_t c) {
            return static_cast<std::uint8_t>(c * 100 / f);
        };
        return {d(r), d(g), d(b), a};
    }
    bool operator==(const Color&) const = default;
};

namespace colors {
inline constexpr Color Transparent{0, 0, 0, 0};
inline constexpr Color Black{0, 0, 0};
inline constexpr Color White{255, 255, 255};
inline constexpr Color Red{220, 50, 47};
inline constexpr Color Green{40, 167, 69};
inline constexpr Color Blue{13, 110, 253};
} // namespace colors

} // namespace aui
