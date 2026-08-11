/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_SCENE_ACTION_H_
#define EKOSCAPE_CORE_SCENE_ACTION_H_

#include "ekoscape/global.h"

#include <cybel/scenes/scene_types.h>

namespace ekoscape {

enum class SceneAction : scene_id_t {
  kNone,
  kQuit,

  kGoToMainMenu,
  kGoToPlayMenu,
  kGoToCredits,

  kGoToGame,
  kGoToBoringWork,
};

namespace SceneActions {
  template <typename T>
  bool has_stars(T action);
}

template <typename T>
bool SceneActions::has_stars(T action) {
  switch(static_cast<SceneAction>(action)) {
    case SceneAction::kGoToMainMenu:
    case SceneAction::kGoToPlayMenu:
    case SceneAction::kGoToCredits:
      return true;

    case SceneAction::kNone:
    case SceneAction::kQuit:
    case SceneAction::kGoToGame:
    case SceneAction::kGoToBoringWork:
      break;
  }

  return false;
}

} // namespace ekoscape
#endif
