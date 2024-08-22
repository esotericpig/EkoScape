/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_SCENE_SCENE_TYPES_H_
#define EKOSCAPE_CORE_SCENE_SCENE_TYPES_H_

#include "core/common.h"

#include "core/util/duration.h"

namespace cybel {

struct FrameStep {
  const Duration& dpf;
  double delta_time = 0.016;
};

} // Namespace.
#endif
