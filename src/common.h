/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_COMMON_H_
#define EKOSCAPE_COMMON_H_

#if defined(CYBEL_MACOS)
  #ifndef DANTARES_MACOS
  #define DANTARES_MACOS
  #endif
#elif defined(CYBEL_WINDOWS)
  #ifndef DANTARES_WINDOWS
  #define DANTARES_WINDOWS
  #endif
#else
  #ifndef DANTARES_LINUX
  #define DANTARES_LINUX
  #endif
#endif

#include "cybel/common.h"

#include "cybel/types.h"

#include "Dantares/Dantares.h"

namespace ekoscape {
  using namespace cybel;
}

#endif
