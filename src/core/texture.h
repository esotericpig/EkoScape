/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_TEXTURE_H_
#define EKOSCAPE_CORE_TEXTURE_H_

#include "common.h"

#include "ekoscape_error.h"
#include "image.h"
#include "util.h"

namespace ekoscape {

/**
 * Only supports images with BPP of: RGB, RGBA, BGR, or BGRA.
 */
class Texture {
public:
  Texture(Image& image,bool make_weird = false);
  Texture(GLubyte r,GLubyte g,GLubyte b,GLubyte a,bool make_weird = false);
  Texture(const Texture& other) = delete;
  Texture(Texture&& other) noexcept;
  virtual ~Texture() noexcept;

  Texture& operator=(const Texture& other) = delete;
  Texture& operator=(Texture&& other) noexcept;

  GLuint id() const;
  int width() const;
  int height() const;

private:
  GLuint id_ = 0;
  int width_ = 0;
  int height_ = 0;

  void move_from(Texture&& other) noexcept;
  void destroy() noexcept;
};

} // Namespace.
#endif
