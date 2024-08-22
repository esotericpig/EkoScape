/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_IMAGE_H_
#define CYBEL_GFX_IMAGE_H_

#include "cybel/common.h"

#include "cybel/util/cybel_error.h"
#include "cybel/util/util.h"
#include "cybel/types.h"

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
  const Size2i& size() const;
  std::uint8_t bpp() const;
  bool is_red_first() const;
  const void* pixels() const;
  GLenum gl_type() const;

private:
  std::string id_{};
  SDL_Surface* surface_ = NULL;
  Size2i size_{};
  bool is_locked_ = false;

  void move_from(Image&& other) noexcept;
  void destroy() noexcept;
};

} // Namespace.
#endif
