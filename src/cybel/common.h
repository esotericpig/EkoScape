/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_COMMON_H_
#define CYBEL_COMMON_H_

#if defined(CYBEL_MACOS)
  #ifndef GL_SILENCE_DEPRECATION
  #define GL_SILENCE_DEPRECATION
  #endif

  #include <GL/glew.h>
  #include <OpenGL/glu.h>
#elif defined(CYBEL_WINDOWS)
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef NOMINMAX
  #define NOMINMAX
  #endif

  #include <windows.h>
  #include <GL/glew.h>
  #include <GL/glu.h>
#else
  #include <GL/glew.h>
  #include <GL/glu.h>
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

namespace cybel {
  namespace utf8 {}
}

#endif
