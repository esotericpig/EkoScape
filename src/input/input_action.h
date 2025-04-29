/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_INPUT_INPUT_ACTION_H_
#define EKOSCAPE_INPUT_INPUT_ACTION_H_

#include "common.h"

#include "cybel/input/input_man.h"

namespace ekoscape {

namespace InputAction {
  enum : input_id_t {
    kUp = 1,
    kDown,
    kLeft,
    kRight,

    kPageUp,
    kPageDown,
    kSelect,
    kGoBack,
    kQuit,

    kToggleMusic,
    // kPlayMusic, // SDL_SCANCODE_AUDIOPLAY.
    // kStopMusic, // SDL_SCANCODE_AUDIOSTOP.
    kToggleFullscreen,
    kToggleMiniMap,
    kToggleSpeedrun,
    kToggleBossOma,

    kRefresh,
    kMakeWeird,
    kToggleFps,

    kMaxId
  };
}

} // namespace ekoscape
#endif
