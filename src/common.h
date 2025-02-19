/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_COMMON_H_
#define EKOSCAPE_COMMON_H_

#include "cybel/common.h"

#if defined(CYBEL_PLATFORM_MACOS)
  #ifndef DANTARES_PLATFORM_MACOS
  #define DANTARES_PLATFORM_MACOS
  #endif
#elif defined(CYBEL_PLATFORM_WINDOWS)
  #ifndef DANTARES_PLATFORM_WINDOWS
  #define DANTARES_PLATFORM_WINDOWS
  #endif
#else // CYBEL_PLATFORM_LINUX.
  #ifndef DANTARES_PLATFORM_LINUX
  #define DANTARES_PLATFORM_LINUX
  #endif
#endif

#if defined(CYBEL_RENDERER_GLES)
#else // CYBEL_RENDERER_GL.
  #ifndef DANTARES_RENDERER_GL
  #define DANTARES_RENDERER_GL
  #endif
#endif

#include "Dantares/Dantares2.h"

namespace ekoscape {
  using namespace cybel;
}

#endif
