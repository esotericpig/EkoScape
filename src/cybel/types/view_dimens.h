/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_TYPES_VIEW_DIMENS_H_
#define CYBEL_TYPES_VIEW_DIMENS_H_

#include "cybel/common.h"

#include "cybel/types/pos.h"
#include "cybel/types/size.h"

namespace cybel {

struct ViewDimens {
  Size2i size{};
  Size2i target_size{};
  Size2i init_size{};
  Pos2f scale{1.0f,1.0f};
  float aspect_scale = 1.0f;
};

} // Namespace.
#endif
