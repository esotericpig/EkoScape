/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sprite.h"

namespace cybel {

Pos4f Sprite::build_src(const Texture& texture,const Pos2i& offset,const Size2i& size,int padding) {
  Pos4f src{};

  if(texture.size().w == 0) { // Avoid divides by 0.
    src.x1 = 0.0f;
    src.x2 = 0.0f;
  } else {
    const float tex_w = static_cast<float>(texture.size().w);
    const float src_x = static_cast<float>(offset.x + padding);

    // Clamp between 0 & 1.
    src.x1 = src_x / tex_w;
    src.x2 = (src_x + static_cast<float>(size.w)) / tex_w;
  }
  if(texture.size().h == 0) { // Avoid divides by 0.
    src.y1 = 0.0f;
    src.y2 = 0.0f;
  } else {
    const float tex_h = static_cast<float>(texture.size().h);
    const float src_y = static_cast<float>(offset.y + padding);

    // Clamp between 0 & 1.
    src.y1 = src_y / tex_h;
    src.y2 = (src_y + static_cast<float>(size.h)) / tex_h;
  }

  return src;
}

Sprite::Sprite(Texture&& texture,int padding)
    : Sprite(std::make_shared<Texture>(std::move(texture)),padding) {}

Sprite::Sprite(Texture&& texture,const Pos2i& offset,const Size2i& size,int padding)
    : Sprite(std::make_shared<Texture>(std::move(texture)),offset,size,padding) {}

Sprite::Sprite(const std::shared_ptr<Texture>& texture,int padding)
    : Sprite(texture,{0,0},{0,0},padding) {}

Sprite::Sprite(const std::shared_ptr<Texture>& texture,const Pos2i& offset,const Size2i& size,int padding)
    : texture_(texture) {
  const int p2 = padding * 2;

  size_.w = (size.w > 0) ? size.w : (texture_->size().w - p2 - offset.x);
  size_.h = (size.h > 0) ? size.h : (texture_->size().h - p2 - offset.y);
  src_ = build_src(*texture_,offset,size_,padding);
}

const Texture& Sprite::texture() const { return *texture_; }

const Pos4f& Sprite::src() const { return src_; }

const Size2i& Sprite::size() const { return size_; }

} // Namespace.
