/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ui_quad.h"

namespace cybel {

UiQuad::UiQuad(const Color4f& color) noexcept
  : color_(color) {}

void UiQuad::draw(Renderer& ren) {
  ren.wrap_color(color_,[&] { ren.draw_quad(pos_,size_); });
}

void UiQuad::set_color(const Color4f& color) { color_ = color; }

} // namespace cybel
