/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_SCENE_ACTION_H_
#define EKOSCAPE_SCENES_SCENE_ACTION_H_

#include "cybel/common.h"

namespace ekoscape {

namespace SceneAction {
  enum {
    kNil = 0,
    kQuit,
    kGoBack,
    kGoToMenu,
    kGoToMenuPlay,
    kGoToMenuCredits,
    kGoToGame,
    kGoToBoringWork,
  };
}

} // Namespace.
#endif
