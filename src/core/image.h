/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_IMAGE_H_
#define EKOSCAPE_CORE_IMAGE_H_

#include "common.h"

#include "ekoscape_error.h"
#include "util.h"

namespace ekoscape {

class Image {
public:
  Image(const std::string& file);
  Image(const Image&) = delete;
  Image(Image&&) noexcept = delete;
  virtual ~Image() noexcept;

  Image& operator=(const Image&) = delete;
  Image& operator=(Image&&) noexcept = delete;

  Image& lock();
  Image& unlock() noexcept;

  const std::string& id() const;
  int width() const;
  int height() const;
  Uint8 bpp() const;
  bool is_red_first() const;
  const void* pixels() const;
  GLenum gl_type() const;

private:
  const std::string id_;
  SDL_Surface* surface_ = NULL;
  bool is_locked_ = false;
};

} // Namespace.
#endif
