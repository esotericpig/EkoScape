/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_COMMON_H_
#define CYBEL_COMMON_H_

#if !(defined(CYBEL_RENDERER_GL) || defined(CYBEL_RENDERER_GLES))
  #if defined(__EMSCRIPTEN__)
    #define CYBEL_RENDERER_GLES
  #else
    #define CYBEL_RENDERER_GL
  #endif
#endif

#if defined(__EMSCRIPTEN__)
  #include <emscripten.h>
  #include <emscripten/html5.h>
#endif

#if defined(CYBEL_RENDERER_GL)
  #if defined(CYBEL_PLATFORM_MACOS)
    #ifndef GL_SILENCE_DEPRECATION
    #define GL_SILENCE_DEPRECATION
    #endif

    #include <GL/glew.h>
    #include <OpenGL/glu.h>
  #elif defined(CYBEL_PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif

    #include <windows.h>
    #include <GL/glew.h>
    #include <GL/glu.h>
  #else // CYBEL_PLATFORM_LINUX
    #include <GL/glew.h>
    #include <GL/glu.h>
  #endif
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#if defined(CYBEL_RENDERER_GLES)
  #if defined(CYBEL_PLATFORM_MACOS) || defined(__IPHONEOS__)
    #include <OpenGLES/ES3/gl.h>
  #else
    #include <GLES3/gl3.h>
  #endif

  // Can't use GLEW in GLES, so using a dummy stub.
  #include "cybel/stubs/glew_stub.h"
#endif

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace cybel {
  namespace utf8 {}
}

#endif
