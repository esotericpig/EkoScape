/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UI_UI_SPRITE_H_
#define CYBEL_UI_UI_SPRITE_H_

#include "cybel/common.h"

#include "cybel/asset/sprite_ref.h"
#include "cybel/ui/ui_box_node.h"

namespace cybel {

class UiSprite : public UiBoxNode {
public:
  explicit UiSprite(const SpriteRef& sprite_ref) noexcept;

  void draw(Renderer& ren) override;

private:
  SpriteRef sprite_ref_;
};

} // namespace cybel
#endif
