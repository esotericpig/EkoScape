/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_JOYPAD_INPUT_H_
#define CYBEL_INPUT_JOYPAD_INPUT_H_

#include "cybel/common.h"

namespace cybel {

enum class JoypadInput {
  kNone,

  kUp,
  kDown,
  kLeft,
  kRight,

  kA,
  kB,
};

} // namespace cybel
#endif
