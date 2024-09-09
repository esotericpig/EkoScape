/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "font_renderer.h"

namespace ekoscape {

const Color4f FontRenderer::kArrowColor = Color4f::bytes(0,252,0);
const Color4f FontRenderer::kCycleArrowColor = Color4f::bytes(254,0,0);
const tiny_utf8::string FontRenderer::kUpArrowText = "↑";
const tiny_utf8::string FontRenderer::kDownArrowText = "↓";
const tiny_utf8::string FontRenderer::kLeftArrowText = "←";
const tiny_utf8::string FontRenderer::kRightArrowText = "→";
const int FontRenderer::kSmallSpaceSize = 24;

FontRenderer::Wrapper::Wrapper(Renderer::FontAtlasWrapper& font,const Color4f& font_color)
    : font(font),font_color(font_color) {}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_menu_opt(const tiny_utf8::string& text,int styles) {
  const bool is_selected = styles & kMenuStyleSelected;
  const bool is_cycle = styles & kMenuStyleCycle;

  // Check kMenuStyleCycle first, since it can be combined with kMenuStyleSelected.
  if(is_cycle) {
    font.ren.wrap_color(kCycleArrowColor,[&]() { font.print(kLeftArrowText); });
  } else if(is_selected) {
    font.ren.wrap_color(kArrowColor,[&]() { font.print(kRightArrowText); });
  } else {
    font.print(); // Space.
  }

  font.pos.x += kSmallSpaceSize;
  font.ren.wrap_color(font_color,[&]() { font.print(text); });

  if(is_cycle) {
    font.pos.x += kSmallSpaceSize;
    font.ren.wrap_color(kCycleArrowColor,[&]() { font.print(kRightArrowText); });
  }

  font.puts();

  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_menu_up_arrow() {
  font.ren.wrap_color(kCycleArrowColor,[&]() { font.puts(kUpArrowText); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_menu_down_arrow() {
  font.ren.wrap_color(kCycleArrowColor,[&]() { font.puts(kDownArrowText); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::print() {
  font.print();
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::print(char32_t c) {
  font.ren.wrap_color(font_color,[&]() { font.print(c); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::print(const tiny_utf8::string& str) {
  font.ren.wrap_color(font_color,[&]() { font.print(str); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::print(const std::vector<tiny_utf8::string>& strs) {
  font.ren.wrap_color(font_color,[&]() { font.print(strs); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::print_blanks(int count) {
  font.print_blanks(count);
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::puts() {
  font.puts();
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::puts(char32_t c) {
  font.ren.wrap_color(font_color,[&]() { font.puts(c); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::puts(const tiny_utf8::string& str) {
  font.ren.wrap_color(font_color,[&]() { font.puts(str); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::puts(const std::vector<tiny_utf8::string>& lines) {
  font.ren.wrap_color(font_color,[&]() { font.puts(lines); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::puts_blanks(int count) {
  font.puts_blanks(count);
  return *this;
}

FontRenderer::FontRenderer(FontAtlas& font_atlas,const Size2i& font_size,const Color4f& font_color)
    : font_atlas_(font_atlas),font_size_(font_size),font_color_(font_color) {}

void FontRenderer::wrap(Renderer& ren,const Pos3i& pos,const WrapCallback& callback) {
  wrap(ren,pos,1.0f,callback);
}

void FontRenderer::wrap(Renderer& ren,const Pos3i& pos,float scale,const WrapCallback& callback) {
  Size2i char_size = font_size_;

  if(scale != 1.0f) {
    char_size.w = static_cast<int>(std::round(static_cast<float>(char_size.w) * scale));
    char_size.h = static_cast<int>(std::round(static_cast<float>(char_size.h) * scale));
  }

  ren.wrap_font_atlas(font_atlas_,pos,char_size,[&](auto& font) {
    Wrapper wrapper{font,font_color_};
    callback(wrapper);
  });
}

const Size2i& FontRenderer::font_size() { return font_size_; }

const Size2i& FontRenderer::font_spacing() { return font_atlas_.spacing(); }

} // Namespace.
