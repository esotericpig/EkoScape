/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_ASSET_TEXTURE_REF_H_
#define CYBEL_ASSET_TEXTURE_REF_H_

#include "cybel/common.h"

#include "cybel/asset/asset_types.h"
#include "cybel/gfx/texture.h"

namespace cybel {

class AssetMan;

class TextureRef {
public:
  explicit TextureRef(AssetMan& asset_man,asset_id_t id) noexcept;

  Texture* get();

private:
  AssetMan* asset_man_ = nullptr;
  asset_id_t id_ = 0;
};

} // namespace cybel
#endif
