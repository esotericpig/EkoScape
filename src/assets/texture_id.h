/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_TEXTURE_ID_H_
#define EKOSCAPE_ASSETS_TEXTURE_ID_H_

#include "common.h"

#include "cybel/asset/asset_types.h"

namespace ekoscape {

enum class TextureId : asset_id_t {
  kStar1,
  kStar2,

  kMax
};

} // namespace ekoscape
#endif
