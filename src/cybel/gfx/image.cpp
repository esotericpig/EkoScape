/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "image.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Image::Image(const std::filesystem::path& file)
    : id_(file.string()) {
  const auto file_str = file.u8string();
  auto file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  surface_ = IMG_Load(file_cstr);

  if(surface_ == NULL) {
    throw CybelError{"Failed to load image [",file_cstr,"]: ",Util::get_sdl_img_error(),'.'};
  }

  size_.w = surface_->w;
  size_.h = surface_->h;
}

Image::Image(Image&& other) noexcept {
  move_from(std::move(other));
}

void Image::move_from(Image&& other) noexcept {
  destroy();

  surface_ = other.surface_;
  other.surface_ = NULL;

  id_ = std::exchange(other.id_,"");
  size_ = std::exchange(other.size_,Size2i{});
  is_locked_ = std::exchange(other.is_locked_,false);
}

Image::~Image() noexcept {
  destroy();
}

void Image::destroy() noexcept {
  if(surface_ != NULL) {
    unlock();
    SDL_FreeSurface(surface_);
    surface_ = NULL;
  }
}

Image& Image::operator=(Image&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

Image& Image::lock() {
  if(is_locked_ || !SDL_MUSTLOCK(surface_)) { return *this; }

  if(SDL_LockSurface(surface_) != 0) {
    throw CybelError{"Failed to lock image [",id_,"]: ",Util::get_sdl_error(),'.'};
  }

  is_locked_ = true;

  return *this;
}

Image& Image::unlock() noexcept {
  if(is_locked_) {
    SDL_UnlockSurface(surface_);
    is_locked_ = false;
  }

  return *this;
}

const std::string& Image::id() const { return id_; }

const Size2i& Image::size() const { return size_; }

std::uint8_t Image::bpp() const { return surface_->format->BytesPerPixel; }

bool Image::is_red_first() const { return(surface_->format->Rmask == 0x000000ff); }

const void* Image::pixels() const { return surface_->pixels; }

GLenum Image::gl_type() const { return GL_UNSIGNED_BYTE; }

} // Namespace.
