/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UI_UI_COMP_H_
#define CYBEL_UI_UI_COMP_H_

#include "cybel/common.h"

#include "cybel/gfx/renderer.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

namespace cybel {

class UiComp {
public:
  virtual ~UiComp() noexcept = default;

  virtual void resize([[maybe_unused]] const Pos3i& pos,[[maybe_unused]] const Size2i& size) {}
  void resize(const Size2i& size) { resize(Pos3i{},size); }
  virtual void draw([[maybe_unused]] Renderer& ren) {}
};

inline auto kUiSpacer = std::make_shared<UiComp>();

} // namespace cybel
#endif
