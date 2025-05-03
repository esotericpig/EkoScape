/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_FONT_ATLAS_ID_H_
#define EKOSCAPE_ASSETS_FONT_ATLAS_ID_H_

#include "common.h"

#include "cybel/asset/asset_types.h"

namespace ekoscape {

enum class FontAtlasId : asset_id_t {
  kMonogram,

  kMax
};

} // namespace ekoscape
#endif
