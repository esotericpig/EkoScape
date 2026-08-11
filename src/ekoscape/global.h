/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_GLOBAL_H_
#define EKOSCAPE_GLOBAL_H_

// IWYU pragma: always_keep

#include <cybel/global.h>

#if defined(CYBEL_PLATFORM_MACOS)
  #ifndef DANTARES_PLATFORM_MACOS
  #define DANTARES_PLATFORM_MACOS
  #endif
#elif defined(CYBEL_PLATFORM_WINDOWS)
  #ifndef DANTARES_PLATFORM_WINDOWS
  #define DANTARES_PLATFORM_WINDOWS
  #endif
#else // CYBEL_PLATFORM_LINUX
  #ifndef DANTARES_PLATFORM_LINUX
  #define DANTARES_PLATFORM_LINUX
  #endif
#endif

namespace ekoscape {
  using namespace cybel;
}

#endif
