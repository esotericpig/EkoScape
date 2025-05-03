/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "asset_man.h"

namespace cybel {

TextureRef AssetMan::tex_ref(asset_id_t id) { return TextureRef{*this,id}; }

SpriteRef AssetMan::sprite_ref(asset_id_t id) { return SpriteRef{*this,id}; }

FontAtlasRef AssetMan::font_atlas_ref(asset_id_t id) { return FontAtlasRef{*this,id}; }

} // namespace cybel
