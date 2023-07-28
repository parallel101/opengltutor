#include "ASCIIFontData.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>
#include "check_gl.hpp"
#define STB_TRUETYPE_IMPLEMENTATION  
#define STB_TRUETYPE_STATIC  
#include "stb_truetype.h" // https://github.com/nothings/stb/blob/master/stb_truetype.h

struct ASCIIFontData::Private {
    stbtt_bakedchar g_cdata[96]; // P-IMPL idiom: don't wanna leak this type out to save compile time
};

ASCIIFontData::ASCIIFontData
    ( const char *font_path
    , float font_height_
    , int font_bitmap_width_
    , int font_bitmap_height_
    )
    : font_bitmap_width(font_bitmap_width_)
    , font_bitmap_height(font_bitmap_height_)
    , font_width(font_height_ * 0.5f)
    , font_height(font_height_)
    , impl(std::make_unique<Private>())
{
    FILE *ttf_file = fopen(font_path, "rb");
    if (!ttf_file) {
        perror(font_path);
        std::terminate();
    }
    fseek(ttf_file, 0, SEEK_END);
    long ttf_size = ftell(ttf_file);
    if (ttf_size < 0) {
        perror(font_path);
        fclose(ttf_file);
        std::terminate();
    }
    fseek(ttf_file, 0, SEEK_SET);
    char* ttf_buffer = (char*)(malloc(ttf_size));
    if (fread(ttf_buffer, 1, ttf_size, ttf_file) != ttf_size) {
        perror(font_path);
        fclose(ttf_file);
        std::terminate();
    }
    fclose(ttf_file);

    unsigned char *font_bitmap = (unsigned char *)malloc(font_bitmap_width_ * font_bitmap_height_);
    stbtt_BakeFontBitmap((unsigned char*)ttf_buffer, 0, 
                         font_height_,                                  // pixel height
                         font_bitmap,
                         font_bitmap_width_, font_bitmap_height_,        // bitmap size
                         32, 96,                                       // ascii characters to bake
                         impl->g_cdata);
    free(ttf_buffer);

    // create an OpenGL texture:
    CHECK_GL(glGenTextures(1, &font_tex));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, font_tex));
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                 font_bitmap_width_, font_bitmap_height_,
                 0, GL_ALPHA, GL_UNSIGNED_BYTE, font_bitmap));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    free(font_bitmap);
}

void ASCIIFontData::draw_ascii(float x, float y, std::string const &text) {
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, font_tex));
    CHECK_GL(glEnable(GL_TEXTURE_2D));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    for (size_t i = 0; i < text.size(); i++) {
        char ch = text[i];
        if (ch < 32 || (unsigned char)ch >= 32 + 96) {
            ch = '?';
        }
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(impl->g_cdata,
                           font_bitmap_width, font_bitmap_height, 
                           ch - 32,
                           &x, &y, &q,
                           1); //1=opengl,0=old d3d
        glBegin(GL_QUADS);
        glColor3f(0.9f, 0.8f, 0.7f);
        glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
        glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
        glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
        glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
        CHECK_GL(glEnd());
    }
}

ASCIIFontData::~ASCIIFontData() = default;
