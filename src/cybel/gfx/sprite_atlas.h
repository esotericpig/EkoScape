/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_SPRITE_ATLAS_H_
#define CYBEL_GFX_SPRITE_ATLAS_H_

#include "cybel/common.h"

#include "cybel/gfx/texture.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

#include <vector>

namespace cybel {

class SpriteAtlas {
public:
  class Builder final {
  public:
    SpriteAtlas build();

    Builder& tex(Texture&& tex);
    Builder& tex(std::unique_ptr<Texture> tex);
    Builder& tex(std::shared_ptr<Texture> tex);
    Builder& offset(int x,int y);
    Builder& cell_size(int width,int height);
    Builder& cell_padding(int padding);
    Builder& grid_size(int cols,int rows);

    std::shared_ptr<Texture> tex() const;
    const Pos2i& offset() const;
    const Size2i& cell_size() const;
    int cell_padding() const;
    const Size2i& grid_size() const;

    friend class SpriteAtlas;

  private:
    std::shared_ptr<Texture> tex_{};
    Pos2i offset_{};
    Size2i cell_size_{};
    int cell_padding_ = 0;
    Size2i grid_size_{};
  };

  virtual ~SpriteAtlas() noexcept = default;

  void zombify();

  const Texture& tex() const;
  const Pos4f* src(std::size_t index) const;
  const Pos4f* src(const Pos2i& cell) const;
  const Size2i& cell_size() const;
  const Size2i& grid_size() const;
  std::size_t cell_count() const;

protected:
  std::shared_ptr<Texture> tex_{};
  Size2i cell_size_{};
  Size2i grid_size_{};
  std::vector<Pos4f> index_to_src_{};

  explicit SpriteAtlas(const Builder& builder);
};

} // namespace cybel
#endif
