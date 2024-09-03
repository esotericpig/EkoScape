/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_FONT_RENDERER_H_
#define EKOSCAPE_FONT_RENDERER_H_

#include "cybel/common.h"

#include "cybel/gfx/font_atlas.h"
#include "cybel/render/renderer.h"
#include "cybel/types.h"

#include <functional>

namespace ekoscape {

using namespace cybel;

class FontRenderer {
public:
  class Wrapper {
  public:
    Renderer::FontAtlasWrapper& font;
    Color4f font_color;

    explicit Wrapper(Renderer::FontAtlasWrapper& font,const Color4f& font_color);

    Wrapper& draw_menu_opt(const tiny_utf8::string& text,int styles = 0); // Julia Stiles?
    Wrapper& draw_menu_up_arrow();
    Wrapper& draw_menu_down_arrow();

    Wrapper& print();
    Wrapper& print(char32_t c);
    Wrapper& print(const tiny_utf8::string& str);
    Wrapper& print(const std::vector<tiny_utf8::string>& strs);
    Wrapper& print_blanks(int count);
    Wrapper& puts();
    Wrapper& puts(char32_t c);
    Wrapper& puts(const tiny_utf8::string& str);
    Wrapper& puts(const std::vector<tiny_utf8::string>& lines);
    Wrapper& puts_blanks(int count);
  };

  using WrapCallback = std::function<void(Wrapper&)>;

  static const int kMenuStyleSelected = 1 << 0;
  static const int kMenuStyleCycle = 1 << 1;

  explicit FontRenderer(FontAtlas& font_atlas,const Size2i& font_size,const Color4f& font_color);

  void wrap(Renderer& ren,const Pos3i& pos,const WrapCallback& callback);
  void wrap(Renderer& ren,const Pos3i& pos,float scale,const WrapCallback& callback);

private:
  static const Color4f kArrowColor;
  static const Color4f kCycleArrowColor;
  static const tiny_utf8::string kUpArrowText;
  static const tiny_utf8::string kDownArrowText;
  static const tiny_utf8::string kLeftArrowText;
  static const tiny_utf8::string kRightArrowText;
  static const int kSmallSpaceSize;

  FontAtlas& font_atlas_;
  Size2i font_size_{};
  Color4f font_color_{};
};

} // Namespace.
#endif
