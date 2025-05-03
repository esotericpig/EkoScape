/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_ASSET_SPRITE_REF_H_
#define CYBEL_ASSET_SPRITE_REF_H_

#include "cybel/common.h"

#include "cybel/asset/asset_types.h"
#include "cybel/gfx/sprite.h"

namespace cybel {

class AssetMan;

class SpriteRef {
public:
  explicit SpriteRef(AssetMan& asset_man,asset_id_t id) noexcept;

  Sprite* get();

private:
  AssetMan& asset_man_;
  asset_id_t id_ = 0;
};

} // namespace cybel
#endif
