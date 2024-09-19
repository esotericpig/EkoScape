/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "font_renderer.h"

namespace ekoscape {

FontRenderer::Wrapper::Wrapper(FontRenderer& font_ren,Renderer::FontAtlasWrapper& font
    ,const Color4f& font_color)
    : font_ren(font_ren),font(font),font_color(font_color) {}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_bg(const Color4f& color,const Size2i& str_size) {
  font.draw_bg(color,str_size);
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_bg(const Color4f& color,const Size2i& str_size
    ,const Size2i& padding) {
  font.draw_bg(color,str_size,padding);
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_menu_opt(const tiny_utf8::string& text,int styles) {
  const bool is_selected = styles & kMenuStyleSelected;
  const bool is_cycle = styles & kMenuStyleCycle;

  // Check kMenuStyleCycle first, since it can be combined with kMenuStyleSelected.
  if(is_cycle) {
    font.ren.wrap_color(font_ren.cycle_arrow_color_,[&]() { font.print(kLeftArrowText); });
  } else if(is_selected) {
    font.ren.wrap_color(font_ren.arrow_color_,[&]() { font.print(kRightArrowText); });
  } else {
    font.print(); // Space.
  }

  font.pos.x += kSmallSpaceSize;
  font.ren.wrap_color(font_color,[&]() { font.print(text); });

  if(is_cycle) {
    font.pos.x += kSmallSpaceSize;
    font.ren.wrap_color(font_ren.cycle_arrow_color_,[&]() { font.print(kRightArrowText); });
  }

  font.puts();

  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_menu_up_arrow() {
  font.ren.wrap_color(font_ren.cycle_arrow_color_,[&]() { font.puts(kUpArrowText); });
  return *this;
}

FontRenderer::Wrapper& FontRenderer::Wrapper::draw_menu_down_arrow() {
  font.ren.wrap_color(font_ren.cycle_arrow_color_,[&]() { font.puts(kDownArrowText); });
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

FontRenderer::FontRenderer(const FontAtlas& font_atlas,bool make_weird)
    : font_atlas_(font_atlas) {
  font_color_ = make_weird ? Color4f::bytes(255,192,203) : Color4f::bytes(214,214,214);
  arrow_color_ = make_weird ? Color4f::bytes(0,252,252) : Color4f::bytes(0,252,0);
  cycle_arrow_color_ = make_weird ? Color4f::bytes(0,0,254) : Color4f::bytes(254,0,0);
}

void FontRenderer::wrap(Renderer& ren,const Pos3i& pos,const WrapCallback& callback) {
  wrap(ren,pos,1.0f,callback);
}

void FontRenderer::wrap(Renderer& ren,const Pos3i& pos,float scale,const WrapCallback& callback) {
  ren.wrap_font_atlas(font_atlas_,pos,scale_size(scale),[&](auto& font) {
    Wrapper wrapper{*this,font,font_color_};
    callback(wrapper);
  });
}

const Size2i& FontRenderer::font_size() const { return kFontSize; }

const Size2i& FontRenderer::font_spacing() const { return font_atlas_.spacing(); }

Size2i FontRenderer::scale_size(float scale) const {
  if(scale == 1.0f) { return kFontSize; }

  Size2i size = kFontSize;

  size.w = static_cast<int>(std::round(static_cast<float>(size.w) * scale));
  size.h = static_cast<int>(std::round(static_cast<float>(size.h) * scale));

  return size;
}

} // Namespace.
