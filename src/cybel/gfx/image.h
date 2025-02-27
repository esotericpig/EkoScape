/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_IMAGE_H_
#define CYBEL_GFX_IMAGE_H_

#include "cybel/common.h"

#include "cybel/types/color.h"
#include "cybel/types/size.h"

#include <filesystem>
#include <functional>

namespace cybel {

class CybelEngine;
class Texture;

class Image {
public:
  using EditPixel = std::function<void(Color4f&)>;

  explicit Image(const std::filesystem::path& file,bool make_weird = false);

  Image(const Image& other) = delete;
  Image(Image&& other) noexcept;
  virtual ~Image() noexcept;

  Image& operator=(const Image& other) = delete;
  Image& operator=(Image&& other) noexcept;

  void make_weird();
  void colorize(const Color4f& to_color);
  void edit_pixels(const EditPixel& edit_pixel);

  Image& lock();
  Image& unlock() noexcept;

  const std::string& id() const;
  const Size2i& size() const;
  std::uint8_t bytes_per_pixel() const;
  bool is_red_first() const;

  friend class CybelEngine;
  friend class Texture;

private:
  std::string id_{};
  SDL_Surface* surface_ = NULL;
  Size2i size_{};
  bool is_locked_ = false;

  void move_from(Image&& other) noexcept;
  void destroy() noexcept;

  const void* pixels() const;
  GLenum gl_type() const;
};

} // Namespace.
#endif
