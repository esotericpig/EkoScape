/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_ASSET_ASSET_MAN_H_
#define CYBEL_ASSET_ASSET_MAN_H_

#include "cybel/common.h"

#include "cybel/asset/asset_types.h"
#include "cybel/asset/font_atlas_ref.h"
#include "cybel/asset/sprite_ref.h"
#include "cybel/asset/texture_ref.h"
#include "cybel/audio/audio.h"
#include "cybel/audio/music.h"
#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"

namespace cybel {

class AssetMan {
public:
  virtual ~AssetMan() noexcept = default;

  TextureRef tex_ref(asset_id_t id);
  SpriteRef sprite_ref(asset_id_t id);
  FontAtlasRef font_atlas_ref(asset_id_t id);

  virtual Image* image([[maybe_unused]] asset_id_t id) { return nullptr; }
  virtual Texture* tex([[maybe_unused]] asset_id_t id) { return nullptr; }
  virtual Sprite* sprite([[maybe_unused]] asset_id_t id) { return nullptr; }
  virtual SpriteAtlas* sprite_atlas([[maybe_unused]] asset_id_t id) { return nullptr; }
  virtual FontAtlas* font_atlas([[maybe_unused]] asset_id_t id) { return nullptr; }

  virtual Audio* audio([[maybe_unused]] asset_id_t id) { return nullptr; }
  virtual Music* music([[maybe_unused]] asset_id_t id) { return nullptr; }
};

} // namespace cybel
#endif
