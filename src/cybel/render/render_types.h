/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_RENDER_RENDER_TYPES_H_
#define CYBEL_RENDER_RENDER_TYPES_H_

#include "cybel/common.h"

#include "cybel/types.h"

namespace cybel {

struct ViewDimens {
  Size2i size{};
  Size2i target_size{};
  Size2i init_size{};
  float scale = 1.0f;
};

} // Namespace.
#endif
