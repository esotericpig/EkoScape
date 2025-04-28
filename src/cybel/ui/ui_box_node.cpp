/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ui_box_node.h"

namespace cybel {

void UiBoxNode::resize(const Pos3i& pos,const Size2i& size) {
  pos_ = pos;
  size_ = size;
}

} // namespace cybel
