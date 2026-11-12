#pragma once
// Aurora UI - ARGB32 image surface with BMP export
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "Color.hpp"

namespace aui {

class Image {
public:
    Image() = default;
    Image(int w, int h, Color fill = colors::White) : w_(w), h_(h), px_(size_t(w) * h) {
        std::uint32_t v = pack(fill);
        for (auto& p : px_) p = v;
    }
    int width() const { return w_; }
    int height() const { return h_; }

    void setPixel(int x, int y, Color c) {
        if (unsigned(x) < unsigned(w_) && unsigned(y) < unsigned(h_))
            px_[size_t(y) * w_ + x] = pack(c);
    }
    void blendPixel(int x, int y, Color c) {
        if (unsigned(x) >= unsigned(w_) || unsigned(y) >= unsigned(h_) || c.a == 0) return;
        if (c.a == 255) { setPixel(x, y, c); return; }
        Color d = unpack(px_[size_t(y) * w_ + x]);
        auto mix = [&](std::uint8_t s, std::uint8_t dd) {
            return std::uint8_t((s * c.a + dd * (255 - c.a)) / 255);
        };
        setPixel(x, y, {mix(c.r, d.r), mix(c.g, d.g), mix(c.b, d.b)});
    }
    Color pixel(int x, int y) const {
        if (unsigned(x) < unsigned(w_) && unsigned(y) < unsigned(h_))
            return unpack(px_[size_t(y) * w_ + x]);
        return {};
    }

    /// Save as 24-bit BMP (universally viewable, no dependencies).
    bool save(const std::string& path) const {
        FILE* f = std::fopen(path.c_str(), "wb");
        if (!f) return false;
        int rowBytes = w_ * 3, pad = (4 - rowBytes % 4) % 4;
        std::uint32_t imgSize = (rowBytes + pad) * h_, fileSize = 54 + imgSize;
        std::uint8_t hdr[54] = {'B', 'M'};
        auto put32 = [&](int off, std::uint32_t v) {
            for (int i = 0; i < 4; ++i) hdr[off + i] = std::uint8_t(v >> (8 * i));
        };
        put32(2, fileSize); put32(10, 54); put32(14, 40);
        put32(18, w_); put32(22, h_);          // bottom-up
        hdr[26] = 1; hdr[28] = 24;             // planes, bpp
        put32(34, imgSize);
        std::fwrite(hdr, 1, 54, f);
        std::vector<std::uint8_t> row(rowBytes + pad);
        for (int y = h_ - 1; y >= 0; --y) {
            for (int x = 0; x < w_; ++x) {
                Color c = unpack(px_[size_t(y) * w_ + x]);
                row[x * 3 + 0] = c.b; row[x * 3 + 1] = c.g; row[x * 3 + 2] = c.r;
            }
            std::fwrite(row.data(), 1, row.size(), f);
        }
        std::fclose(f);
        return true;
    }

private:
    static std::uint32_t pack(Color c) {
        return (std::uint32_t(c.a) << 24) | (std::uint32_t(c.r) << 16) |
               (std::uint32_t(c.g) << 8) | c.b;
    }
    static Color unpack(std::uint32_t v) {
        return {std::uint8_t(v >> 16), std::uint8_t(v >> 8), std::uint8_t(v), std::uint8_t(v >> 24)};
    }
    int w_ = 0, h_ = 0;
    std::vector<std::uint32_t> px_;
};

} // namespace aui
