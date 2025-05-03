/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_IMAGE_ID_H_
#define EKOSCAPE_ASSETS_IMAGE_ID_H_

#include "common.h"

#include "cybel/asset/asset_types.h"

namespace ekoscape {

enum class ImageId : asset_id_t {
  kEkoScapeIcon,

  kMax
};

} // namespace ekoscape
#endif
