/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_GFX_SPRITE_H_
#define EKOSCAPE_CORE_GFX_SPRITE_H_

#include "core/common.h"

#include "core/render/render_data.h"
#include "texture.h"

namespace cybel {

class Renderer;

class Sprite {
public:
  static Pos4f build_src(const Texture& texture,const Pos2i& offset,const Size2i& size,int padding = 0);

  explicit Sprite(Texture&& texture,int padding = 0);
  Sprite(Texture&& texture,const Pos2i& offset,const Size2i& size,int padding = 0);
  explicit Sprite(std::shared_ptr<Texture> texture,int padding = 0);
  Sprite(std::shared_ptr<Texture> texture,const Pos2i& offset,const Size2i& size,int padding = 0);
  virtual ~Sprite() noexcept = default;

  const Texture& texture() const;
  const Pos4f& src() const;
  const Size2i& size() const;

protected:
  std::shared_ptr<Texture> texture_{};
  Pos4f src_{};
  Size2i size_{};
};

} // Namespace.
#endif