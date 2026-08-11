/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_FONT_RENDERER_H_
#define EKOSCAPE_ASSETS_FONT_RENDERER_H_

#include "ekoscape/global.h"

#include "ekoscape/assets/asset_ids.h"

#include <cybel/gfx/color.h>
#include <cybel/math/pos.h>
#include <cybel/math/size.h>
#include <cybel/renderer/renderer.h>
#include <cybel/scenes/scene_context.h>

#include <functional>
#include <string>
#include <string_view>

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

  static constexpr int kMenuStyleSelected = 1 << 0;
  static constexpr int kMenuStyleCycle = 1 << 1;

  explicit FontRenderer(FontAtlasId font_atlas_id);

  void make_weird(bool is_weird);

  void wrap(Renderer& ren,const SceneContext& ctx,const Pos3i& pos,const WrapCallback& callback);
  void wrap(Renderer& ren,const SceneContext& ctx,const Pos3i& pos,float scale,const WrapCallback& callback);

  const Size2i& font_size() const;
  const Size2i& font_spacing(const SceneContext& ctx) const;
  const Color4f& arrow_color() const;
  const Color4f& cycle_arrow_color() const;

private:
  static constexpr Size2i kFontSize{40,90};
  static constexpr std::string kUpArrowText = "↑";
  static constexpr std::string kDownArrowText = "↓";
  static constexpr std::string kLeftArrowText = "←";
  static constexpr std::string kRightArrowText = "→";
  static constexpr int kSmallSpaceSize = 24;

  FontAtlasId font_atlas_id_{};
  Color4f font_color_{};
  Color4f arrow_color_{};
  Color4f cycle_arrow_color_{};

  Size2i scale_size(float scale) const;
};

} // namespace ekoscape
#endif
