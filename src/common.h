/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_COMMON_H_
#define EKOSCAPE_COMMON_H_

#if defined(CYBEL_MACOS)
  #define DANTARES_MACOS
#elif defined(CYBEL_WINDOWS)
  #define DANTARES_WINDOWS
#else
  #define DANTARES_LINUX
#endif

#include "cybel/common.h"

#include "cybel/types.h"

#include "Dantares/Dantares.h"

namespace ekoscape {
  using namespace cybel;
}

#endif
