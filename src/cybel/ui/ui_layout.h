/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UI_UI_LAYOUT_H_
#define CYBEL_UI_UI_LAYOUT_H_

#include "cybel/common.h"

namespace cybel {

enum class UiLayout : std::uint8_t {
  kUnknown,
  kWide,
  kTall,
};

} // namespace cybel
#endif
