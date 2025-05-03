/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "texture_ref.h"

#include "cybel/asset/asset_man.h"

namespace cybel {

TextureRef::TextureRef(AssetMan& asset_man,asset_id_t id) noexcept
  : asset_man_(asset_man),id_(id) {}

Texture* TextureRef::get() { return asset_man_.tex(id_); }

} // namespace cybel
