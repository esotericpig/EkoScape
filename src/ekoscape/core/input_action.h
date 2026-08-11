/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_INPUT_ACTION_H_
#define EKOSCAPE_CORE_INPUT_ACTION_H_

#include "ekoscape/global.h"

#include <cybel/inputs/input_types.h>

namespace ekoscape {

enum class InputAction : input_id_t {
  // Movement.
  kUp,
  kDown,
  kLeft,
  kRight,

  // Menu Navigation.
  kPageUp,
  kPageDown,
  kSelect,
  kGoBack,
  kQuit,

  // Options/Features.
  kToggleMusic,
  //kPlayMusic, // SDL_SCANCODE_AUDIOPLAY
  //kStopMusic, // SDL_SCANCODE_AUDIOSTOP
  kToggleFullscreen,
  kToggleMiniMap,
  kToggleSpeedrun,
  kToggleBoringWork,

  // Dev/Secrets.
  kRefresh,
  kMakeWeird,
  kToggleFps,
  kToggleFrozen,
};

} // namespace ekoscape
#endif
