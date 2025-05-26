/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UI_UI_TEXTURE_H_
#define CYBEL_UI_UI_TEXTURE_H_

#include "cybel/common.h"

#include "cybel/asset/texture_ref.h"
#include "cybel/ui/ui_box_node.h"

namespace cybel {

class UiTexture : public UiBoxNode {
public:
  TextureRef tex_ref;

  explicit UiTexture(const TextureRef& tex_ref) noexcept;

  void draw(Renderer& ren) override;
};

} // namespace cybel
#endif
