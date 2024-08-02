/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "scene_result.h"

namespace ekoscape {

SceneResult SceneResults::to_scene_result(int value) { return static_cast<SceneResult>(value); }

int SceneResults::value_of(SceneResult result) { return static_cast<int>(result); }

} // Namespace.
