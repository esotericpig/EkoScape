/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_SPRITE_H_
#define CYBEL_GFX_SPRITE_H_

#include "cybel/common.h"

#include "cybel/gfx/texture.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

namespace cybel {

class Sprite {
public:
  static Pos4f build_src(const Texture& tex,const Pos2i& offset,const Size2i& size,int padding = 0);

  explicit Sprite(Texture&& tex,int padding = 0);
  explicit Sprite(Texture&& tex,const Pos2i& offset,const Size2i& size,int padding = 0);
  explicit Sprite(std::unique_ptr<Texture> tex,int padding = 0);
  explicit Sprite(std::unique_ptr<Texture> tex,const Pos2i& offset,const Size2i& size,int padding = 0);
  explicit Sprite(std::shared_ptr<Texture> tex,int padding = 0);
  explicit Sprite(std::shared_ptr<Texture> tex,const Pos2i& offset,const Size2i& size,int padding = 0);

  virtual ~Sprite() noexcept = default;

  const Texture& tex() const;
  const Pos4f& src() const;
  const Size2i& size() const;

protected:
  std::shared_ptr<Texture> tex_{};
  Pos4f src_{};
  Size2i size_{};
};

} // Namespace.
#endif
