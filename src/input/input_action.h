/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_INPUT_INPUT_ACTION_H_
#define EKOSCAPE_INPUT_INPUT_ACTION_H_

#include "common.h"

namespace ekoscape {

namespace InputAction {
  enum : int {
    kUp = 1,
    kDown,
    kLeft,
    kRight,

    kPageUp,
    kPageDown,
    kSelect,
    kGoBack,
    // kQuit,

    kToggleMusic,
    // kPlayMusic,
    // kStopMusic,
    kToggleFullscreen,
    kToggleMiniMap,
    kToggleSpeedrun,
    kToggleBossOma,

    kRefresh,
    kMakeWeird,
    kToggleFps,

    kMaxId // Must be defined last.
  };
}

} // Namespace.
#endif
