/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "util.h"

namespace cybel {

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

std::string Util::get_emscripten_result([[maybe_unused]] int result) {
  #if defined(__EMSCRIPTEN__)
    switch(result) {
      case EMSCRIPTEN_RESULT_SUCCESS: return "EMSCRIPTEN_RESULT_SUCCESS";
      case EMSCRIPTEN_RESULT_DEFERRED: return "EMSCRIPTEN_RESULT_DEFERRED";
      case EMSCRIPTEN_RESULT_NOT_SUPPORTED: return "EMSCRIPTEN_RESULT_NOT_SUPPORTED";
      case EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED: return "EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED";
      case EMSCRIPTEN_RESULT_INVALID_TARGET: return "EMSCRIPTEN_RESULT_INVALID_TARGET";
      case EMSCRIPTEN_RESULT_UNKNOWN_TARGET: return "EMSCRIPTEN_RESULT_UNKNOWN_TARGET";
      case EMSCRIPTEN_RESULT_INVALID_PARAM: return "EMSCRIPTEN_RESULT_INVALID_PARAM";
      case EMSCRIPTEN_RESULT_FAILED: return "EMSCRIPTEN_RESULT_FAILED";
      case EMSCRIPTEN_RESULT_NO_DATA: return "EMSCRIPTEN_RESULT_NO_DATA";
    }

    return std::string{"unknown result ["} + std::to_string(result) + ']';
  #else
    return "";
  #endif
}

} // Namespace.
