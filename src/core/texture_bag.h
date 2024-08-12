/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_TEXTURE_BAG_H_
#define EKOSCAPE_CORE_TEXTURE_BAG_H_

#include "common.h"

#include "texture.h"

namespace ekoscape {

class TextureBag {
public:
  virtual ~TextureBag() noexcept = default;

  virtual const Texture& texture() const = 0;
};

} // Namespace.
#endif
