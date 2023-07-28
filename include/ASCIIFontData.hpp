#pragma once

#include <memory>
#include <string>
#include "check_gl.hpp"

struct ASCIIFontData {
    ASCIIFontData(ASCIIFontData &&) = delete;

    const int font_bitmap_width, font_bitmap_height;
    const float font_width, font_height;
    GLuint font_tex;

    struct Private;
    std::unique_ptr<Private> impl;

    explicit ASCIIFontData
    ( const char *font_path
    , float font_height_ = 64.0f
    , int font_bitmap_width_ = 512
    , int font_bitmap_height_ = 512
    );
    ~ASCIIFontData();

    void draw_ascii(float x, float y, std::string const &text);
};

/* static std::unique_ptr<FontData> g_font; */
