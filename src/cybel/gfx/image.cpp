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

Image::Image(const std::filesystem::path& file,bool make_weird,const Color4f& weird_color)
  : id_(file.string()) {
  const auto file_str = file.u8string();
  const auto* file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  handle_ = IMG_Load(file_cstr);

  if(handle_ == NULL) {
    throw CybelError{"Failed to load image [",file_cstr,"]: ",Util::get_sdl_img_error(),'.'};
  }

  size_.w = handle_->w;
  size_.h = handle_->h;

  if(make_weird) {
    if(weird_color == Color4f::kBlack) {
      this->make_weird();
    } else {
      colorize(weird_color);
    }
  }
}

Image::Image(Image&& other) noexcept {
  move_from(std::move(other));
}

void Image::move_from(Image&& other) noexcept {
  destroy();

  handle_ = other.handle_;
  other.handle_ = NULL;

  id_ = std::exchange(other.id_,"");
  size_ = std::exchange(other.size_,Size2i{});
  is_locked_ = std::exchange(other.is_locked_,false);
}

Image::~Image() noexcept {
  destroy();
}

void Image::destroy() noexcept {
  if(handle_ != NULL) {
    unlock();
    SDL_FreeSurface(handle_);
    handle_ = NULL;
  }
}

Image& Image::operator=(Image&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

void Image::make_weird() {
  edit_pixels([](Color4f& c) {
    std::swap(c.r,c.b);
  });
}

void Image::colorize(const Color4f& to_color) {
  edit_pixels([&](Color4f& c) {
    c.r = std::clamp(c.r * (to_color.r / 0.5f),0.0f,1.0f);
    c.g = std::clamp(c.g * (to_color.g / 0.5f),0.0f,1.0f);
    c.b = std::clamp(c.b * (to_color.b / 0.5f),0.0f,1.0f);
    c.a = to_color.a;
  });
}

void Image::edit_pixels(const EditPixel& edit_pixel) {
  if(SDL_PIXELTYPE(handle_->format->format) != SDL_PIXELTYPE_PACKED32) {
    // Convert to a surface we can work with.
    SDL_Surface* new_handle = SDL_ConvertSurfaceFormat(handle_,SDL_PIXELFORMAT_RGBA32,0);

    if(new_handle == NULL) {
      std::cerr << "[WARN] Failed to convert surface of image [" << id_ << "] for editing pixels: "
                << Util::get_sdl_error() << '.' << std::endl;
      return;
    }

    destroy();
    handle_ = new_handle;
    size_.w = handle_->w;
    size_.h = handle_->h;
  }

  try {
    lock();
  } catch(const CybelError& e) {
    std::cerr << "[WARN] For editing pixels: " << e.what() << std::endl;
    return;
  }

  const int area = size_.w * size_.h;
  auto* pixels = static_cast<Uint32*>(handle_->pixels);

  for(int i = 0; i < area; ++i) {
    const auto pixel = pixels[i];
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
    Uint8 a = 0;

    SDL_GetRGBA(pixel,handle_->format,&r,&g,&b,&a);
    auto color = Color4f::bytes(r,g,b,a);

    edit_pixel(color);

    pixels[i] = SDL_MapRGBA(
      handle_->format,
      static_cast<Uint8>(std::clamp(255.0f * color.r,0.0f,255.0f)),
      static_cast<Uint8>(std::clamp(255.0f * color.g,0.0f,255.0f)),
      static_cast<Uint8>(std::clamp(255.0f * color.b,0.0f,255.0f)),
      static_cast<Uint8>(std::clamp(255.0f * color.a,0.0f,255.0f))
    );
  }

  unlock();
}

Image& Image::lock() {
  if(is_locked_ || !SDL_MUSTLOCK(handle_)) { return *this; }

  if(SDL_LockSurface(handle_) != 0) {
    throw CybelError{"Failed to lock image [",id_,"]: ",Util::get_sdl_error(),'.'};
  }

  is_locked_ = true;

  return *this;
}

Image& Image::unlock() noexcept {
  if(is_locked_) {
    SDL_UnlockSurface(handle_);
    is_locked_ = false;
  }

  return *this;
}

const std::string& Image::id() const { return id_; }

const Size2i& Image::size() const { return size_; }

std::uint8_t Image::bytes_per_pixel() const { return handle_->format->BytesPerPixel; }

bool Image::is_red_first() const { return(handle_->format->Rmask == 0x000000ff); }

const void* Image::pixels() const { return handle_->pixels; }

GLenum Image::gl_type() const { return GL_UNSIGNED_BYTE; }

} // namespace cybel
