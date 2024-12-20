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

StrUtf8 Util::ellips_str(const StrUtf8& str,std::size_t max_len) {
  if(str.length() > max_len) {
    if(max_len == 0) { return ""; }
    if(max_len == 1) { return str.substr(0,max_len); }

    return str.substr(0,max_len - 1) + "…";
  }

  return str;
}

StrUtf8 Util::pad_str(const StrUtf8& str,std::size_t len) {
  if(str.length() >= len) { return str; }

  StrUtf8 result = str;

  while(result.length() < len) { result += " "; }

  return result;
}

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

int Util::comparei_str(const StrUtf8& str1,const StrUtf8& str2) {
  const std::size_t len1 = str1.length();
  const std::size_t len2 = str2.length();
  const std::size_t min_len = std::min(len1,len2);

  for(std::size_t i = 0; i < min_len; ++i) {
    // tolower() only works w/ ASCII, but that's fine for now.
    char32_t c1 = std::tolower(str1.at(i));
    char32_t c2 = std::tolower(str2.at(i));

    if(c1 < c2) { return -1; }
    if(c1 > c2) { return 1; }
  }

  if(len1 < len2) { return -1; }
  if(len1 > len2) { return 1; }
  return 0;
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
