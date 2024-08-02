/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_SCENE_RESULT_H_
#define EKOSCAPE_SCENES_SCENE_RESULT_H_

#include "core/common.h"

namespace ekoscape {

enum class SceneResult : int {
  kNil = 0,
  kQuit,
  kMenuScene,
  kNextScene,
};

namespace SceneResults {
  SceneResult to_scene_result(int value);
  int value_of(SceneResult result);
}

} // Namespace.
#endif
