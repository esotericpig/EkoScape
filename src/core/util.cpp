/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "util.h"

namespace ekoscape {

Color4f::Color4f() {}

Color4f::Color4f(float r,float g,float b,float a) { set_f(r,g,b,a); }

Color4f::Color4f(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) { set_i(r,g,b,a); }

Color4f& Color4f::set_f(float r,float g,float b,float a) {
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;

  return *this;
}

Color4f& Color4f::set_i(std::uint8_t r,std::uint8_t g,std::uint8_t b,std::uint8_t a) {
  this->r = r / 255.0f;
  this->g = g / 255.0f;
  this->b = b / 255.0f;
  this->a = a / 255.0f;

  return *this;
}

Rando::Rando() {}

Rando& Rando::it() {
  static Rando it_{};

  return it_;
}

bool Rando::rand_bool() { return bool_distr_(generator_); }

std::uint32_t Rando::rand_uint(std::uint32_t min,std::uint32_t max) {
  return (uint_distr_(generator_) % (max + 1)) + min;
}

std::string Util::strip(const std::string& str) {
  if(str.empty()) { return str; }

  std::size_t begin = 0;

  for(; begin < str.size(); ++begin) {
    if(!std::isspace(static_cast<unsigned char>(str[begin]))) { break; }
  }

  if(begin >= str.size()) { return ""; }

  std::size_t end = str.size() - 1;

  for(; end >= 0; --end) {
    if(!std::isspace(static_cast<unsigned char>(str[end]))) { break; }
  }

  return str.substr(begin,end - begin + 1);
}

std::string Util::get_sdl_error() {
  // This should technically never return null, but Justin Case.
  const char* str = SDL_GetError();

  return (str == NULL) ? "" : str;
}

std::string Util::get_sdl_img_error() {
  const char* str = IMG_GetError();

  return (str == NULL) ? "" : str;
}

std::string Util::get_sdl_mix_error() {
  const char* str = Mix_GetError();

  return (str == NULL) ? "" : str;
}

std::string Util::get_gl_error(GLenum error) {
  const GLubyte* str = gluErrorString(error);

  return (str == NULL) ? "" : reinterpret_cast<const char*>(str);
}

std::string Util::get_glew_error(GLenum error) {
  const GLubyte* str = glewGetErrorString(error);

  return (str == NULL) ? "" : reinterpret_cast<const char*>(str);
}

} // Namespace.
