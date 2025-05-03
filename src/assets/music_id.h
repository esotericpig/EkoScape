/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_MUSIC_ID_H_
#define EKOSCAPE_ASSETS_MUSIC_ID_H_

#include "common.h"

#include "cybel/asset/asset_types.h"

namespace ekoscape {

enum class MusicId : asset_id_t {
  kEkoScape,

  kMax
};

} // namespace ekoscape
#endif
