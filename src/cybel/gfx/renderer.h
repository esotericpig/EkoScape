/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_RENDERER_H_
#define CYBEL_GFX_RENDERER_H_

#include "cybel/common.h"

#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/color.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"
#include "cybel/types/view_dimens.h"

#include <functional>

namespace cybel {

/**
 * The begin_/end_*() functions do not preserve the previous states when nesting.
 * This is by design.
 *
 * Alternatively, the wrap_*() functions do preserve the previous states when nesting.
 *
 * Nesting Example with Wrap:
 *   @code
 *   Pos2i pos{100,0};
 *   const Size2i size{100,100};
 *
 *   ren.wrap_color({0.0f,0.0f,1.0f},[&]() {
 *     ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Blue.
 *
 *     ren.wrap_color({0.0f,1.0f,0.0f},[&]() {
 *       ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Green.
 *
 *       ren.wrap_color({1.0f,1.0f,0.0f},[&]() {
 *         ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Yellow.
 *       });
 *
 *       ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Back to Green.
 *     });
 *
 *     ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Back to Blue.
 *   });
 *
 *   ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Back to White (default).
 *   @endcode
 *
 * Nesting Example with Begin/End:
 *   @code
 *   Pos2i pos{100,0};
 *   const Size2i size{100,100};
 *
 *   ren.begin_color({0.0f,0.0f,1.0f});
 *     ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Blue.
 *
 *     ren.begin_color({0.0f,1.0f,0.0f});
 *       ren.draw_quad({pos.x,(++pos.y) * size.h,0},size); // Green.
 *     ren.end_color(); // NOT back to Blue, but White (default).
 *
 *     // NOT Blue or Green, but now White (default).
 *     ren.draw_quad({pos.x,(++pos.y) * size.h,0},size);
 *   ren.end_color();
 *   @endcode
 */
class Renderer {
public:
  class TextureWrapper {
  public:
    Renderer& ren;
    const Texture& tex;
    Pos4f src{};

    explicit TextureWrapper(Renderer& ren,const Texture& tex,const Pos4f& src);

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
    Size2i rune_size{};
    Size2i spacing{};

    explicit FontAtlasWrapper(Renderer& ren,const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                              const Size2i& spacing);

    FontAtlasWrapper& draw_bg(const Color4f& color,const Size2i& str_size);
    FontAtlasWrapper& draw_bg(const Color4f& color,const Size2i& str_size,const Size2i& padding);

    FontAtlasWrapper& print();
    FontAtlasWrapper& print(char32_t rune);
    FontAtlasWrapper& print(std::string_view str);
    FontAtlasWrapper& print_blanks(int count);
    FontAtlasWrapper& puts();
    FontAtlasWrapper& puts(char32_t rune);
    FontAtlasWrapper& puts(std::string_view str);
    FontAtlasWrapper& puts_blanks(int count);

    Size2i calc_total_size(const Size2i& str_size);
    Size2i calc_total_size(const Size2i& str_size,const Size2i& padding);
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

  Renderer& begin_tex(const Texture& tex);
  Renderer& end_tex();

  Renderer& wrap_color(const Color4f& color,const WrapCallback& callback);
  Renderer& wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback);
  Renderer& wrap_add_blend(const WrapCallback& callback);

  Renderer& wrap_tex(const Texture& tex,const WrapTextureCallback& callback);
  Renderer& wrap_tex(const Texture& tex,const Pos4f& src,const WrapTextureCallback& callback);

  Renderer& wrap_sprite(const Sprite& sprite,const WrapSpriteCallback& callback);
  Renderer& wrap_sprite_atlas(const SpriteAtlas& atlas,const WrapSpriteAtlasCallback& callback);

  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                            const WrapFontAtlasCallback& callback);
  Renderer& wrap_font_atlas(const FontAtlas& font,const Pos3i& pos,const Size2i& rune_size,
                            const Size2i& spacing,const WrapFontAtlasCallback& callback);

  Renderer& draw_quad(const Pos3i& pos,const Size2i& size);
  Renderer& draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size);

  Pos5f build_dest_pos5f(const Pos3i& pos,const Size2i& size);

  const ViewDimens& dimens() const;
  Color4f& clear_color();

private:
  struct BlendMode {
    GLenum src_factor{};
    GLenum dst_factor{};
  };

  static inline const BlendMode kDefaultBlendMode{GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA};
  static inline const BlendMode kAddBlendMode{GL_ONE,GL_ONE};

  ViewDimens dimens_{};
  Pos2f scale_{1.0f,1.0f};
  float aspect_scale_ = 1.0f;
  Pos2f offset_{0.0f,0.0f};
  Color4f clear_color_{};

  Color4f curr_color_{1.0f};
  BlendMode curr_blend_mode_ = kDefaultBlendMode;
  // This would be safer as a shared_ptr, but I think fine,
  //     as it would require refactoring all of the methods & callers.
  const Texture* curr_tex_ = nullptr;

  void init_gl();

  Renderer& begin_blend(const BlendMode& mode);

  Renderer& wrap_tex(const Texture& tex,const WrapCallback& callback);
};

} // Namespace.
#endif
