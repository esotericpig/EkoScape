/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_ASSET_IDS_H_
#define EKOSCAPE_ASSETS_ASSET_IDS_H_

#include "common.h"

#include "cybel/asset/asset_types.h"

namespace ekoscape {

enum class ImageId : asset_id_t {
  kEkoScapeIcon,
};

enum class TextureId : asset_id_t {
  kStar,
  kStarWeird,

  kStyleBegin,
};

enum class SpriteId : asset_id_t {
  kEkoScapeLogo,
  kDantaresLogo,
  kKeys,
  kBoringWork,
  kGoodnight,
  kCorngrits,
};

enum class FontAtlasId : asset_id_t {
  kMonogram,
};

enum class MusicId : asset_id_t {
  kEkoScape,
};

} // namespace ekoscape
#endif
