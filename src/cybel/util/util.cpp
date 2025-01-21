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

void Util::clear_gl_errors() {
  // Max loop to prevent accidental infinite loop.
  // - https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml
  for(int i = 0; i < 100; ++i) {
    if(glGetError() == GL_NO_ERROR) { break; }
  }
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
  // - https://www.khronos.org/opengl/wiki/OpenGL_Error
  switch(error) {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";

    // Removed in 3.1+.
    case 0x8031: return "GL_TABLE_TOO_LARGE1";

    // 3.0+.
    case 0x0506: return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // 4.5+.
    case 0x0507: return "GL_CONTEXT_LOST";
  }

  return std::string{"unknown error ["} + std::to_string(error) + ']';
}

std::string Util::get_glew_error(GLenum error) {
  const auto* str = glewGetErrorString(error);

  return (str != NULL) ? reinterpret_cast<const char*>(str) : "";
}

} // Namespace.
