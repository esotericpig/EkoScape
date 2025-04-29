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

using joypad_input_t = std::uint8_t;

enum class JoypadInput : joypad_input_t {
  kNone = 0,

  kUp,
  kDown,
  kLeft,
  kRight,

  kA,
  kB,

  kMax
};

enum class FakeJoypadInputType {
  kAxis, // Joystick & Game controller.
  kHat,  // Joystick only.
  kDpad, // Game controller only.
};

} // namespace cybel
#endif
