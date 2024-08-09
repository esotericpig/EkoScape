/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sprite.h"

namespace ekoscape {

Pos4f Sprite::build_src(const Texture& texture,const Pos2i& offset,const Size2i& size,int padding) {
  Pos4f src{};

  if(texture.width() == 0) { // Avoid divides by 0.
    src.x1 = 0.0f;
    src.x2 = 0.0f;
  } else {
    const GLfloat tex_w = static_cast<GLfloat>(texture.width());
    const GLfloat src_x = static_cast<GLfloat>(offset.x + padding);

    // Clamp between 0 & 1.
    src.x1 = src_x / tex_w;
    src.x2 = (src_x + static_cast<GLfloat>(size.w)) / tex_w;
  }
  if(texture.height() == 0) { // Avoid divides by 0.
    src.y1 = 0.0f;
    src.y2 = 0.0f;
  } else {
    const GLfloat tex_h = static_cast<GLfloat>(texture.height());
    const GLfloat src_y = static_cast<GLfloat>(offset.y + padding);

    // Clamp between 0 & 1.
    src.y1 = src_y / tex_h;
    src.y2 = (src_y + static_cast<GLfloat>(size.h)) / tex_h;
  }

  return src;
}

Sprite::Sprite(const Texture& texture,int padding)
    : Sprite(texture,{0,0},{0,0},padding) {}

Sprite::Sprite(const Texture& texture,const Pos2i& offset,const Size2i& size,int padding) {
  const int p2 = padding * 2;

  size_.w = (size.w > 0) ? size.w : (texture.width() - p2 - offset.x);
  size_.h = (size.h > 0) ? size.h : (texture.height() - p2 - offset.y);
  src_ = build_src(texture,offset,size_,padding);
}

const Pos4f& Sprite::src() const { return src_; }

const Size2i& Sprite::size() const { return size_; }

} // Namespace.
