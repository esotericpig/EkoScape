/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UI_UI_BOX_NODE_H_
#define CYBEL_UI_UI_BOX_NODE_H_

#include "cybel/common.h"

#include "cybel/ui/ui_node.h"

namespace cybel {

class UiBoxNode : public UiNode {
public:
  void resize(const Pos3i& pos,const Size2i& size) override;

protected:
  Pos3i pos_{};
  Size2i size_{};
};

} // namespace cybel
#endif
