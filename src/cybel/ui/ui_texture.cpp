/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ui_texture.h"

namespace cybel {

void UiTexture::draw(Renderer& ren) {
  if(tex_ == nullptr) { return; }

  ren.wrap_tex(*tex_,[&](auto& t) { t.draw_quad(pos_,size_); });
}

void UiTexture::set_tex(const Texture& tex) { tex_ = &tex; }

} // namespace cybel
