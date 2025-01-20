/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "util.h"

namespace cybel {

std::string Util::wrap_str(const std::string& str,std::size_t max_len) {
  if(str.length() <= max_len) { return str; }

  std::ostringstream result{};
  std::istringstream lines{str};
  std::string line{};

  while(std::getline(lines,line)) {
    if(line.length() <= max_len) {
      result << line << '\n';
      continue;
    }

    std::ostringstream wrapped_line{};
    std::istringstream words{line};
    std::string word{};

    while(words >> word) {
      const auto wrapped_len = static_cast<std::size_t>(wrapped_line.tellp());

      // If empty, then always add the word, else might get `\n<long.word>\n` instead of `<long.word>\n`.
      if(wrapped_len > 0) {
        // +1 for space.
        if((wrapped_len + 1 + word.length()) > max_len) {
          result << wrapped_line.str() << '\n';

          wrapped_line.str("");
          wrapped_line.clear();
        } else {
          wrapped_line << ' '; // Not the first word on this line.
        }
      }

      wrapped_line << word;
    }

    // Not empty?
    if(static_cast<std::size_t>(wrapped_line.tellp()) > 0) {
      result << wrapped_line.str() << '\n';
    }
  }

  return result.str();
}

std::string Util::get_sdl_error() {
  // This should technically never return null, but Justin Case.
  const auto* str = SDL_GetError();

  return (str != NULL) ? str : "";
}

std::string Util::get_sdl_img_error() {
  const auto* str = IMG_GetError();

  return (str != NULL) ? str : "";
}

std::string Util::get_sdl_mix_error() {
  const auto* str = Mix_GetError();

  return (str != NULL) ? str : "";
}

std::string Util::get_gl_error(GLenum error) {
  const auto* str = gluErrorString(error);

  return (str != NULL) ? reinterpret_cast<const char*>(str) : "";
}

std::string Util::get_glew_error(GLenum error) {
  const auto* str = glewGetErrorString(error);

  return (str != NULL) ? reinterpret_cast<const char*>(str) : "";
}

} // Namespace.
