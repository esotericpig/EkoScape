/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_COMMON_H_
#define CYBEL_COMMON_H_

// TODO: Define in build.
#if defined(CYBEL_MACOS)
  #define DANTARES_MACOS

  #include <OpenGL/glew.h>
  #include <OpenGL/glu.h>
#elif defined(CYBEL_WINDOWS)
  #define DANTARES_WINDOWS

  #include <GL/glew.h>
  #include <GL/glu.h>
#else
  #define DANTARES_LINUX

  #include <GL/glew.h>
  #include <GL/glu.h>
#endif

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <tinyutf8/tinyutf8.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "Dantares/Dantares.h"

#endif
