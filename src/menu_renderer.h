/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MENU_RENDERER_H_
#define EKOSCAPE_MENU_RENDERER_H_

#include "cybel/common.h"

#include "cybel/gfx/font_atlas.h"
#include "cybel/render/renderer.h"
#include "cybel/types.h"

#include <functional>

namespace ekoscape {

using namespace cybel;

class MenuRenderer {
public:
  class Wrapper {
  public:
    Renderer::FontAtlasWrapper& font;

    explicit Wrapper(Renderer::FontAtlasWrapper& font);

    Wrapper& draw_opt(const tiny_utf8::string& text,int styles = 0);
    Wrapper& draw_up_arrow();
    Wrapper& draw_down_arrow();
  };

  using WrapCallback = std::function<void(Renderer::FontAtlasWrapper&,Wrapper&)>;

  static const int kStyleSelected = 1 << 0;
  static const int kStyleCycle = 1 << 1;

  static const Size2i kCharSize;
  static const Color4f kArrowColor;
  static const Color4f kCycleArrowColor;
  static const Color4f kTextColor;
  static const tiny_utf8::string kUpArrowText;
  static const tiny_utf8::string kDownArrowText;
  static const tiny_utf8::string kLeftArrowText;
  static const tiny_utf8::string kRightArrowText;
  static const int kSmallSpaceSize;

  explicit MenuRenderer(FontAtlas& font_atlas);

  void wrap(Renderer& ren,const Pos3i& pos,const WrapCallback& callback);
  void wrap(Renderer& ren,const Pos3i& pos,float scale,const WrapCallback& callback);

private:
  FontAtlas& font_atlas_;
};

} // Namespace.
#endif
