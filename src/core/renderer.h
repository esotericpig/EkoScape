/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_RENDERER_H_
#define EKOSCAPE_CORE_RENDERER_H_

#include "common.h"

#include "ekoscape_error.h"
#include "font_atlas.h"
#include "render_data.h"
#include "sprite.h"
#include "sprite_atlas.h"
#include "texture.h"
#include "texture_bag.h"
#include "util.h"

#include <vector>

namespace ekoscape {

class Renderer {
public:
  Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  void resize(const Size2i& size);

  void begin_2d_scene();
  void begin_3d_scene();
  void clear_view();

  void begin_texture(const Texture& texture);
  void begin_texture(const TextureBag& bag);
  void end_texture();

  void begin_color(const Color4f& color);
  void end_color();

  void draw_quad(int x,int y,int width,int height);
  void draw_quad(const Pos4f& src,int x,int y,int width,int height);

  void draw_quad(const Sprite& sprite,int x,int y);
  void draw_quad(const Sprite& sprite,int x,int y,int width,int height);

  void draw_quad(const SpriteAtlas& atlas,int index,int x,int y);
  void draw_quad(const SpriteAtlas& atlas,int index,int x,int y,int width,int height);
  void draw_quad(const SpriteAtlas& atlas,int column,int row,int x,int y);
  void draw_quad(const SpriteAtlas& atlas,int column,int row,int x,int y,int width,int height);

  /**
   * `str` is at end in case have a multi-line string.
   */
  void draw_str(const FontAtlas& font,int x,int y,const tiny_utf8::string& str);

  /**
   * Using Size2i for `spacing` to prevent ambiguous overloads.
   */
  void draw_str(const FontAtlas& font,int x,int y,const Size2i& spacing,const tiny_utf8::string& str);

  void draw_str(const FontAtlas& font,int x,int y,int char_width,int char_height
      ,const tiny_utf8::string& str);
  void draw_str(const FontAtlas& font,int x,int y,int char_width,int char_height,const Size2i& spacing
      ,const tiny_utf8::string& str);

  void draw_strs(const FontAtlas& font,int x,int y,const std::vector<tiny_utf8::string>& lines);
  void draw_strs(const FontAtlas& font,int x,int y,const Size2i& spacing
      ,const std::vector<tiny_utf8::string>& lines);
  void draw_strs(const FontAtlas& font,int x,int y,int char_width,int char_height
      ,const std::vector<tiny_utf8::string>& lines);
  void draw_strs(const FontAtlas& font,int x,int y,int char_width,int char_height,const Size2i& spacing
      ,const std::vector<tiny_utf8::string>& lines);

  const ViewDimens& dimens() const;
  Color4f& clear_color();

private:
  ViewDimens dimens_{};
  Color4f clear_color_{};

  void init_gl();
};

} // Namespace.
#endif
