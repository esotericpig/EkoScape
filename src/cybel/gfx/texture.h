/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_TEXTURE_H_
#define CYBEL_GFX_TEXTURE_H_

#include "cybel/common.h"

#include "cybel/util/cybel_error.h"
#include "cybel/util/util.h"
#include "cybel/types.h"
#include "image.h"

namespace cybel {

/**
 * Only supports images with BPP of: RGB, RGBA, BGR, or BGRA.
 */
class Texture {
public:
  explicit Texture(Image& img,bool make_weird = false);
  explicit Texture(Image&& img,bool make_weird = false);
  explicit Texture(const Color4f& color,bool make_weird = false);
  Texture(const Texture& other) = delete;
  Texture(Texture&& other) noexcept;
  virtual ~Texture() noexcept;

  Texture& operator=(const Texture& other) = delete;
  Texture& operator=(Texture&& other) noexcept;

  GLuint gl_id() const;
  const Size2i& size() const;

private:
  GLuint gl_id_ = 0;
  Size2i size_{};

  void move_from(Texture&& other) noexcept;
  void destroy() noexcept;
};

} // Namespace.
#endif
