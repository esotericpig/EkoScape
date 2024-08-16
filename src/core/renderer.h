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
#include "sprite_atlas.h"
#include "texture.h"
#include "texture_bag.h"
#include "util.h"

#include <functional>
#include <vector>

namespace ekoscape {

class Renderer {
public:
  class TextureWrapper {
  public:
    Renderer& ren;
    const Texture& texture;
    Pos4f src;

    TextureWrapper(Renderer& ren,const Texture& texture,const Pos4f& src);

    TextureWrapper& draw_quad(int x,int y);
    TextureWrapper& draw_quad(int x,int y,int width,int height);
  };

  class SpriteAtlasWrapper {
  public:
    Renderer& ren;
    const SpriteAtlas& atlas;

    SpriteAtlasWrapper(Renderer& ren,const SpriteAtlas& atlas);

    SpriteAtlasWrapper& draw_quad(int index,int x,int y);
    SpriteAtlasWrapper& draw_quad(int index,int x,int y,int width,int height);
    SpriteAtlasWrapper& draw_quad(int column,int row,int x,int y);
    SpriteAtlasWrapper& draw_quad(int column,int row,int x,int y,int width,int height);
  };

  class FontAtlasWrapper {
  public:
    Renderer& ren;
    const FontAtlas& font;
    Pos2i init_pos;
    Pos2i pos;
    Size2i char_size;
    Size2i spacing;

    FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos2i& pos,const Size2i& char_size
        ,const Size2i& spacing);

    FontAtlasWrapper& print();
    FontAtlasWrapper& print(const tiny_utf8::string& str);
    FontAtlasWrapper& print(const std::vector<tiny_utf8::string>& strs);
    FontAtlasWrapper& puts();
    FontAtlasWrapper& puts(const tiny_utf8::string& str);
    FontAtlasWrapper& puts(const std::vector<tiny_utf8::string>& lines);
  };

  using WrapCallback = std::function<void()>;
  using WrapTextureCallback = std::function<void(TextureWrapper&)>;
  using WrapSpriteAtlasCallback = std::function<void(SpriteAtlasWrapper&)>;
  using WrapFontAtlasCallback = std::function<void(FontAtlasWrapper&)>;

  static const Pos4f kDefaultSrc;

  Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  void resize(const Size2i& size);
  void clear_view();

  Renderer& begin_2d_scene();
  Renderer& begin_3d_scene();

  Renderer& begin_auto_center();
  Renderer& end_auto_center();

  Renderer& begin_auto_scale();
  Renderer& begin_scale(float scale);
  Renderer& end_scale();

  /**
   * This uses the scale, so if you want to change the scale, you must change it first
   * before calling this function.
   * - Note that this is done for you automatically in begin_auto_center().
   */
  Renderer& begin_auto_center_offset();
  Renderer& begin_offset(float x_offset,float y_offset);
  Renderer& end_offset();

  Renderer& begin_color(const Color4f& color);
  Renderer& end_color();

  Renderer& begin_texture(const Texture& texture);
  Renderer& end_texture();

  Renderer& wrap_color(const Color4f& color,const WrapCallback& callback);

  Renderer& wrap_texture(const Texture& texture,const WrapTextureCallback& callback);
  Renderer& wrap_texture(const Texture& texture,const Pos4f& src,const WrapTextureCallback& callback);
  Renderer& wrap_texture(const TextureBag& tex_bag,const WrapTextureCallback& callback);
  Renderer& wrap_texture(const TextureBag& tex_bag,const Pos4f& src,const WrapTextureCallback& callback);

  Renderer& wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback);

  Renderer& wrap_font_atlas(const FontAtlas& font,int x,int y,const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,int x,int y,int char_width,int char_height
      ,const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,int x,int y,int char_width,int char_height
      ,const Size2i& spacing,const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,int x,int y,const Size2i& spacing
      ,const WrapFontAtlasCallback& callback);

  Renderer& draw_quad(int x,int y,int width,int height);
  Renderer& draw_quad(const Pos4f& src,int x,int y,int width,int height);

  Pos4f build_dest_pos4f(int x,int y,int width,int height);

  const ViewDimens& dimens() const;
  Color4f& clear_color();

private:
  ViewDimens dimens_{};
  float scale_ = 1.0f;
  Pos2f offset_{0.0f,0.0f};
  Color4f clear_color_{};

  void init_gl();
};

} // Namespace.
#endif
