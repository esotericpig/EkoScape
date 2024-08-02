/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "image.h"

namespace ekoscape {

Image::Image(const std::string& file)
    : id_(file) {
  surface_ = IMG_Load(file.c_str());

  if(surface_ == NULL) {
    throw EkoScapeError{Util::build_string("Failed to load image [",file,"]: "
        ,Util::get_sdl_img_error(),'.')};
  }
}

Image::~Image() noexcept {
  if(surface_ != NULL) {
    unlock();
    SDL_FreeSurface(surface_);
    surface_ = NULL;
  }
}

Image& Image::lock() {
  if(is_locked_) { return *this; }
  if(!SDL_MUSTLOCK(surface_)) { return *this; }

  if(SDL_LockSurface(surface_) != 0) {
    throw EkoScapeError{Util::build_string("Failed to lock image [",id_,"]: "
        ,Util::get_sdl_error(),'.')};
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

int Image::width() const { return surface_->w; }

int Image::height() const { return surface_->h; }

Uint8 Image::bpp() const { return surface_->format->BytesPerPixel; }

bool Image::is_red_first() const { return(surface_->format->Rmask == 0x000000ff); }

const void* Image::pixels() const { return surface_->pixels; }

GLenum Image::gl_type() const { return GL_UNSIGNED_BYTE; }

} // Namespace.
