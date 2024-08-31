/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_renderer.h"

namespace ekoscape {

const Size2i MenuRenderer::kCharSize = {40,90};
const Color4f MenuRenderer::kArrowColor = {0,252,0};
const Color4f MenuRenderer::kCycleArrowColor = {254,0,0};
const Color4f MenuRenderer::kTextColor = {214,214,214};
const tiny_utf8::string MenuRenderer::kUpArrowText = "↑";
const tiny_utf8::string MenuRenderer::kDownArrowText = "↓";
const tiny_utf8::string MenuRenderer::kLeftArrowText = "←";
const tiny_utf8::string MenuRenderer::kRightArrowText = "→";
const int MenuRenderer::kSmallSpaceSize = 24;

MenuRenderer::Wrapper::Wrapper(Renderer::FontAtlasWrapper& font)
    : font(font) {}

MenuRenderer::Wrapper& MenuRenderer::Wrapper::draw_opt(const tiny_utf8::string& text,int styles) {
  const bool is_selected = styles & kStyleSelected;
  const bool is_cycle = styles & kStyleCycle;

  if(is_cycle) {
    font.ren.wrap_color(kCycleArrowColor,[&]() { font.print(kLeftArrowText); });
  } else if(is_selected) {
    font.ren.wrap_color(kArrowColor,[&]() { font.print(kRightArrowText); });
  } else {
    font.print(); // Space.
  }

  font.pos.x += kSmallSpaceSize;
  font.ren.wrap_color(kTextColor,[&]() { font.print(text); });

  if(is_cycle) {
    font.pos.x += kSmallSpaceSize;
    font.ren.wrap_color(kCycleArrowColor,[&]() { font.print(kRightArrowText); });
  }

  font.puts();

  return *this;
}

MenuRenderer::Wrapper& MenuRenderer::Wrapper::draw_up_arrow() {
  font.ren.wrap_color(kCycleArrowColor,[&]() { font.puts(kUpArrowText); });
  return *this;
}

MenuRenderer::Wrapper& MenuRenderer::Wrapper::draw_down_arrow() {
  font.ren.wrap_color(kCycleArrowColor,[&]() { font.puts(kDownArrowText); });
  return *this;
}

MenuRenderer::MenuRenderer(FontAtlas& font_atlas)
    : font_atlas_(font_atlas) {}

void MenuRenderer::wrap(Renderer& ren,const Pos3i& pos,const WrapCallback& callback) {
  wrap(ren,pos,1.0f,callback);
}

void MenuRenderer::wrap(Renderer& ren,const Pos3i& pos,float scale,const WrapCallback& callback) {
  Size2i char_size = kCharSize;

  if(scale != 1.0f) {
    char_size.w = static_cast<int>(std::round(static_cast<float>(char_size.w) * scale));
    char_size.h = static_cast<int>(std::round(static_cast<float>(char_size.h) * scale));
  }

  ren.wrap_font_atlas(font_atlas_,pos,char_size,[&](auto& font) {
    Wrapper wrapper{font};
    callback(font,wrapper);
  });
}

} // Namespace.