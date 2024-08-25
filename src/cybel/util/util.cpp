/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "util.h"

namespace cybel {

std::string Util::strip_str(const std::string& str) {
  if(str.empty()) { return str; }

  std::size_t begin = 0;

  for(; begin < str.length(); ++begin) {
    if(!std::isspace(static_cast<unsigned char>(str[begin]))) { break; }
  }

  if(begin >= str.length()) { return ""; }

  std::size_t end = str.length();

  for(; end > begin; --end) { // Unsigned loop.
    if(!std::isspace(static_cast<unsigned char>(str[end - 1]))) { break; }
  }

  return str.substr(begin,end - begin);
}

std::string Util::ellips_str(const std::string& str,std::size_t max_len) {
  if(str.length() > max_len) {
    if(max_len == 0) { return ""; }
    if(max_len == 1) { return str.substr(0,max_len); }

    return str.substr(0,max_len - 1) + "…";
  }

  return str;
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
