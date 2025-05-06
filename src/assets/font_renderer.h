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
#include "cybel/types/color.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

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
    Wrapper& draw_menu_opt(std::string_view text,int styles = 0); // Julia Stiles?
    Wrapper& draw_menu_up_arrow();
    Wrapper& draw_menu_down_arrow();

    Wrapper& print();
    Wrapper& print(char32_t rune);
    Wrapper& print(std::string_view str);
    Wrapper& print_blanks(int count);
    Wrapper& puts();
    Wrapper& puts(char32_t rune);
    Wrapper& puts(std::string_view str);
    Wrapper& puts_blanks(int count);

    Wrapper& set_bg_padding(const Size2i& padding);
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
  static inline const std::string kUpArrowText = "↑";
  static inline const std::string kDownArrowText = "↓";
  static inline const std::string kLeftArrowText = "←";
  static inline const std::string kRightArrowText = "→";
  static inline const int kSmallSpaceSize = 24;

  const FontAtlas& font_atlas_;
  Color4f font_color_{};
  Color4f arrow_color_{};
  Color4f cycle_arrow_color_{};

  Size2i scale_size(float scale) const;
};

} // namespace ekoscape
#endif
