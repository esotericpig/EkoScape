/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_SCENE_ACTION_H_
#define EKOSCAPE_SCENES_SCENE_ACTION_H_

#include "common.h"

namespace ekoscape {

namespace SceneAction {
  enum : int {
    kNil = 0,
    kQuit,
    kGoBack,
    kRestart,
    kGoToMenu,
    kGoToMenuPlay,
    kGoToMenuCredits,
    kGoToGame,
    kGoToBoringWork,
  };
}

namespace SceneActions {
  bool is_menu(int action);
}

} // namespace ekoscape
#endif
