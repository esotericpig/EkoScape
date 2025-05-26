/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ui_sprite.h"

namespace cybel {

UiSprite::UiSprite(const SpriteRef& sprite_ref) noexcept
  : sprite_ref(sprite_ref) {}

void UiSprite::draw(Renderer& ren) {
  const auto* sprite = sprite_ref.get();
  if(sprite == nullptr) { return; }

  ren.wrap_sprite(*sprite,[&](auto& t) { t.draw_quad(pos_,size_); });
}

} // namespace cybel
