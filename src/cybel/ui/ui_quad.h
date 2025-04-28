/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UI_UI_QUAD_H_
#define CYBEL_UI_UI_QUAD_H_

#include "cybel/common.h"

#include "cybel/types/color.h"
#include "cybel/ui/ui_node.h"

namespace cybel {

/**
 * Mainly used for testing/debugging purposes.
 */
class UiQuad : public UiNode {
public:
  explicit UiQuad(const Color4f& color) noexcept;

  void resize(const Pos3i& pos,const Size2i& size) override;
  void draw(Renderer& ren) override;

private:
  Color4f color_{};
  Pos3i pos_{};
  Size2i size_{};
};

} // namespace cybel
#endif
