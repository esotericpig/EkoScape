/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_SPRITE_H_
#define CYBEL_GFX_SPRITE_H_

#include "cybel/common.h"

#include "cybel/types.h"
#include "texture.h"

namespace cybel {

class Renderer;

class Sprite {
public:
  static Pos4f build_src(const Texture& texture,const Pos2i& offset,const Size2i& size,int padding = 0);

  explicit Sprite(Texture&& texture,int padding = 0);
  explicit Sprite(Texture&& texture,const Pos2i& offset,const Size2i& size,int padding = 0);
  explicit Sprite(const std::shared_ptr<Texture>& texture,int padding = 0);
  explicit Sprite(const std::shared_ptr<Texture>& texture,const Pos2i& offset,const Size2i& size
      ,int padding = 0);
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
