/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_GFX_IMAGE_H_
#define EKOSCAPE_CORE_GFX_IMAGE_H_

#include "core/common.h"

#include "core/util/util.h"
#include "core/cybel_error.h"

#include <filesystem>

namespace cybel {

class Image {
public:
  explicit Image(const std::filesystem::path& file);
  Image(const Image& other) = delete;
  Image(Image&& other) noexcept;
  virtual ~Image() noexcept;

  Image& operator=(const Image& other) = delete;
  Image& operator=(Image&& other) noexcept;

  Image& lock();
  Image& unlock() noexcept;

  const std::string& id() const;
  int width() const;
  int height() const;
  std::uint8_t bpp() const;
  bool is_red_first() const;
  const void* pixels() const;
  GLenum gl_type() const;

private:
  std::string id_;
  SDL_Surface* surface_ = NULL;
  bool is_locked_ = false;

  void move_from(Image&& other) noexcept;
  void destroy() noexcept;
};

} // Namespace.
#endif
