/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_SCENE_ACTION_H_
#define EKOSCAPE_SCENES_SCENE_ACTION_H_

#include "common.h"

#include "cybel/scene/scene_types.h"

namespace ekoscape {

enum class SceneAction : scene_id_t {
  kNone,
  kQuit,

  kGoToMenu,
  kGoToMenuPlay,
  kGoToMenuCredits,

  kGoToGame,
  kGoToBoringWork,
};

namespace SceneActions {
  template <typename T>
  bool is_menu(T action);
}

template <typename T>
bool SceneActions::is_menu(T action) {
  switch(static_cast<SceneAction>(action)) {
    // This should not include kGoToBoringWork.
    case SceneAction::kGoToMenu:
    case SceneAction::kGoToMenuPlay:
    case SceneAction::kGoToMenuCredits:
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
