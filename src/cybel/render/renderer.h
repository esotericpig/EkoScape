/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_RENDER_RENDERER_H_
#define CYBEL_RENDER_RENDERER_H_

#include "cybel/common.h"

#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"
#include "cybel/util/cybel_error.h"
#include "cybel/util/util.h"
#include "cybel/types.h"
#include "render_types.h"

#include <functional>
#include <vector>

namespace cybel {

/**
 * Note that wrapping inside of a wrap doesn't work as one might expect.
 * This can be fixed internally by using a stack/vector, but not implemented for now.
 *
 * Example:
 *   ren.wrap_color({0.0f,0.0f,1.0f},[&]() {
 *     // Blue.
 *     ren.draw_quad(0,0,100,100);
 *
 *     // Green.
 *     ren.wrap_color({0.0f,1.0f,0.0f},[&]() {
 *       ren.draw_quad(0,100,100,100);
 *     });
 *
 *     // Not blue (or green), but white now (reset).
 *     ren.draw_quad(0,200,100,100);
 *   });
 */
class Renderer {
public:
  class TextureWrapper {
  public:
    Renderer& ren;
    const Texture& texture;
    Pos4f src{};

    explicit TextureWrapper(Renderer& ren,const Texture& texture,const Pos4f& src);

    TextureWrapper& draw_quad(const Pos3i& pos);
    TextureWrapper& draw_quad(const Pos3i& pos,const Size2i& size);
  };

  class SpriteWrapper {
  public:
    Renderer& ren;
    const Sprite& sprite;

    explicit SpriteWrapper(Renderer& ren,const Sprite& sprite);

    SpriteWrapper& draw_quad(const Pos3i& pos);
    SpriteWrapper& draw_quad(const Pos3i& pos,const Size2i& size);
  };

  class SpriteAtlasWrapper {
  public:
    Renderer& ren;
    const SpriteAtlas& atlas;

    explicit SpriteAtlasWrapper(Renderer& ren,const SpriteAtlas& atlas);

    SpriteAtlasWrapper& draw_quad(int index,const Pos3i& pos);
    SpriteAtlasWrapper& draw_quad(int index,const Pos3i& pos,const Size2i& size);
    SpriteAtlasWrapper& draw_quad(const Pos2i& cell,const Pos3i& pos);
    SpriteAtlasWrapper& draw_quad(const Pos2i& cell,const Pos3i& pos,const Size2i& size);
  };

  class FontAtlasWrapper {
  public:
    Renderer& ren;
    const FontAtlas& font;
    Pos3i init_pos{};
    Pos3i pos{};
    Size2i char_size{};
    Size2i spacing{};

    explicit FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos3i& pos,const Size2i& char_size
        ,const Size2i& spacing);

    FontAtlasWrapper& draw_bg(const Color4f& color,const Size2i& str_size);
    FontAtlasWrapper& draw_bg(const Color4f& color,const Size2i& str_size,const Size2i& padding);

    FontAtlasWrapper& print();
    FontAtlasWrapper& print(char32_t c);
    FontAtlasWrapper& print(const CybelStrUtf8& str);
    FontAtlasWrapper& print(const std::vector<CybelStrUtf8>& strs);
    FontAtlasWrapper& print_blanks(int count);
    FontAtlasWrapper& puts();
    FontAtlasWrapper& puts(char32_t c);
    FontAtlasWrapper& puts(const CybelStrUtf8& str);
    FontAtlasWrapper& puts(const std::vector<CybelStrUtf8>& lines);
    FontAtlasWrapper& puts_blanks(int count);
  };

  using WrapCallback = std::function<void()>;
  using WrapTextureCallback = std::function<void(TextureWrapper&)>;
  using WrapSpriteCallback = std::function<void(SpriteWrapper&)>;
  using WrapSpriteAtlasCallback = std::function<void(SpriteAtlasWrapper&)>;
  using WrapFontAtlasCallback = std::function<void(FontAtlasWrapper&)>;

  static inline const Pos4f kDefaultSrc{0.0f,0.0f,1.0f,1.0f};

  explicit Renderer(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  void resize(const Size2i& size);
  void clear_view();

  Renderer& begin_2d_scene();
  Renderer& begin_3d_scene();

  Renderer& begin_auto_center_scale();
  Renderer& begin_auto_anchor_scale(const Pos2f& anchor);
  Renderer& begin_auto_scale();
  Renderer& end_scale();

  Renderer& begin_color(const Color4f& color);
  Renderer& end_color();

  Renderer& begin_add_blend();
  Renderer& end_blend();

  Renderer& begin_texture(const Texture& texture);
  Renderer& end_texture();

  Renderer& wrap_color(const Color4f& color,const WrapCallback& callback);
  Renderer& wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback);
  Renderer& wrap_add_blend(const WrapCallback& callback);

  Renderer& wrap_texture(const Texture& texture,const WrapTextureCallback& callback);
  Renderer& wrap_texture(const Texture& texture,const Pos4f& src,const WrapTextureCallback& callback);

  Renderer& wrap_sprite(const Sprite& sprite,const WrapSpriteCallback& callback);
  Renderer& wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback);

  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& char_size
      ,const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& char_size
      ,const Size2i& spacing,const WrapFontAtlasCallback& callback);

  Renderer& draw_quad(const Pos3i& pos,const Size2i& size);
  Renderer& draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size);

  Pos5f build_dest_pos5f(const Pos3i& pos,const Size2i& size);

  const ViewDimens& dimens() const;
  Color4f& clear_color();

private:
  ViewDimens dimens_{};
  Pos2f scale_{1.0f,1.0f};
  float aspect_scale_ = 1.0f;
  Pos2f offset_{0.0f,0.0f};
  Color4f clear_color_{};
  GLint blend_src_rgb_ = 0;
  GLint blend_src_alpha_ = 0;
  GLint blend_dst_rgb_ = 0;
  GLint blend_dst_alpha_ = 0;

  void init_gl();
};

} // Namespace.
#endif
