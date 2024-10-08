/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_SPRITE_ATLAS_H_
#define CYBEL_GFX_SPRITE_ATLAS_H_

#include "cybel/common.h"

#include "cybel/types.h"
#include "sprite.h"
#include "texture.h"

#include <vector>

namespace cybel {

class SpriteAtlas {
public:
  class Builder {
  public:
    explicit Builder(Texture&& texture);
    explicit Builder(const std::shared_ptr<Texture>& texture);
    virtual ~Builder() noexcept = default;

    SpriteAtlas build();

    Builder& offset(int x,int y);
    Builder& cell_size(int width,int height);
    Builder& cell_padding(int padding);
    Builder& grid_size(int cols,int rows);

    friend class SpriteAtlas;

  protected:
    std::shared_ptr<Texture> texture_{};
    Pos2i offset_{};
    Size2i cell_size_{};
    int cell_padding_ = 0;
    Size2i grid_size_{};
  };

  virtual ~SpriteAtlas() noexcept = default;

  const Texture& texture() const;
  const Pos4f* src(int index) const;
  const Pos4f* src(const Pos2i& cell) const;
  const Size2i& cell_size() const;
  const Size2i& grid_size() const;
  int cell_count() const;

protected:
  std::shared_ptr<Texture> texture_{};
  Size2i cell_size_{};
  Size2i grid_size_{};
  int cell_count_ = 0;
  std::vector<Pos4f> index_to_src_{};

  explicit SpriteAtlas(const Builder& builder);
};

} // Namespace.
#endif
