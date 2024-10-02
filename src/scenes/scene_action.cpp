/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "scene_action.h"

namespace ekoscape {

bool SceneActions::is_menu(int action) {
  // This should not include kGoToBoringWork.
  switch(action) {
    case SceneAction::kGoToMenu:
    case SceneAction::kGoToMenuPlay:
    case SceneAction::kGoToMenuCredits:
      return true;

    default: return false;
  }
}

} // Namespace.
