/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_INPUT_TYPES_H_
#define CYBEL_INPUT_INPUT_TYPES_H_

#include "cybel/common.h"

#include <unordered_set>

namespace cybel {

using input_id_t = std::size_t;
using InputIds = std::unordered_set<input_id_t>;

} // namespace cybel
#endif
