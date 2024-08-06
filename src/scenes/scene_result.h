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

namespace SceneResult {
  enum {
    kNil = 0,
    kQuit,
    kMenuScene,
    kNextScene,
  };
}

} // Namespace.
#endif
