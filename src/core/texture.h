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
#include "render_data.h"
#include "util.h"

namespace ekoscape {

/**
 * Only supports images with BPP of: RGB, RGBA, BGR, or BGRA.
 */
class Texture {
public:
  explicit Texture(Image& image,bool make_weird = false);
  explicit Texture(Image&& image,bool make_weird = false);
  Texture(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a = 255,bool make_weird = false);
  Texture(const Texture& other) = delete;
  Texture(Texture&& other) noexcept;
  virtual ~Texture() noexcept;

  Texture& operator=(const Texture& other) = delete;
  Texture& operator=(Texture&& other) noexcept;

  GLuint gl_id() const;
  int width() const;
  int height() const;

private:
  GLuint gl_id_ = 0;
  int width_ = 0;
  int height_ = 0;

  void move_from(Texture&& other) noexcept;
  void destroy() noexcept;
};

} // Namespace.
#endif
