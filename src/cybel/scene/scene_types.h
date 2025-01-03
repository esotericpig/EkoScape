/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_SCENE_SCENE_TYPES_H_
#define CYBEL_SCENE_SCENE_TYPES_H_

#include "cybel/common.h"

#include "cybel/util/duration.h"

namespace cybel {

struct FrameStep {
  Duration dpf{};
  double delta_time{};
};

} // Namespace.
#endif
