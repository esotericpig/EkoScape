/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_ASSET_FONT_ATLAS_REF_H_
#define CYBEL_ASSET_FONT_ATLAS_REF_H_

#include "cybel/common.h"

#include "cybel/asset/asset_types.h"
#include "cybel/gfx/font_atlas.h"

namespace cybel {

class AssetMan;

class FontAtlasRef {
public:
  explicit FontAtlasRef(AssetMan& asset_man,asset_id_t id) noexcept;

  FontAtlas* get();

private:
  AssetMan* asset_man_ = nullptr;
  asset_id_t id_ = 0;
};

} // namespace cybel
#endif
