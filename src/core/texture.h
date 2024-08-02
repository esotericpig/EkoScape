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
  Texture(const Texture&) = delete;
  Texture(Texture&&) noexcept = delete;
  virtual ~Texture() noexcept;

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&&) noexcept = delete;

  GLuint id() const;

private:
  GLuint id_ = 0;
};

} // Namespace.
#endif
