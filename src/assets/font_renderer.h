/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_FONT_RENDERER_H_
#define EKOSCAPE_ASSETS_FONT_RENDERER_H_

#include "common.h"

#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/renderer.h"

#include <functional>

namespace ekoscape {

class FontRenderer {
public:
  class Wrapper {
  public:
    FontRenderer& font_ren;
    Renderer::FontAtlasWrapper& font;
    Color4f font_color{};

    explicit Wrapper(FontRenderer& font_ren,Renderer::FontAtlasWrapper& font,const Color4f& font_color);

    Wrapper& draw_bg(const Color4f& color,const Size2i& str_size);
    Wrapper& draw_bg(const Color4f& color,const Size2i& str_size,const Size2i& padding);
    Wrapper& draw_menu_opt(const StrUtf8& text,int styles = 0); // Julia Stiles?
    Wrapper& draw_menu_up_arrow();
    Wrapper& draw_menu_down_arrow();

    Wrapper& print();
    Wrapper& print(char32_t c);
    Wrapper& print(const StrUtf8& str);
    Wrapper& print(const std::vector<StrUtf8>& strs);
    Wrapper& print_blanks(int count);
    Wrapper& puts();
    Wrapper& puts(char32_t c);
    Wrapper& puts(const StrUtf8& str);
    Wrapper& puts(const std::vector<StrUtf8>& lines);
    Wrapper& puts_blanks(int count);
  };

  using WrapCallback = std::function<void(Wrapper&)>;

  static inline const int kMenuStyleSelected = 1 << 0;
  static inline const int kMenuStyleCycle = 1 << 1;

  explicit FontRenderer(const FontAtlas& font_atlas,bool make_weird = false);

  void wrap(Renderer& ren,const Pos3i& pos,const WrapCallback& callback);
  void wrap(Renderer& ren,const Pos3i& pos,float scale,const WrapCallback& callback);

  const Size2i& font_size() const;
  const Size2i& font_spacing() const;
  const Color4f& arrow_color() const;
  const Color4f& cycle_arrow_color() const;

private:
  static inline const Size2i kFontSize{40,90};
  static inline const StrUtf8 kUpArrowText = "↑";
  static inline const StrUtf8 kDownArrowText = "↓";
  static inline const StrUtf8 kLeftArrowText = "←";
  static inline const StrUtf8 kRightArrowText = "→";
  static inline const int kSmallSpaceSize = 24;

  const FontAtlas& font_atlas_;
  Color4f font_color_{};
  Color4f arrow_color_{};
  Color4f cycle_arrow_color_{};

  Size2i scale_size(float scale) const;
};

} // Namespace.
#endif
