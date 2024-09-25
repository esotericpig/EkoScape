/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_COMMON_H_
#define CYBEL_COMMON_H_

#if defined(CYBEL_MACOS)
  #include <OpenGL/glew.h>
  #include <OpenGL/glu.h>
#elif defined(CYBEL_WINDOWS)
  #include <GL/glew.h>
  #include <GL/glu.h>
#else
  #include <GL/glew.h>
  #include <GL/glu.h>
#endif

#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

// Disable warnings for the next includes.
// - It's annoying to see warnings from third-party libraries,
//   as it makes it difficult to determine if it's my code or theirs.
#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#include <tinyutf8/tinyutf8.h>

// Re-enable warnings.
#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic pop
#endif

#endif
