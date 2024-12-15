/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// ReSharper disable CppHidingFunction

#ifndef CYBEL_GFX_FONT_ATLAS_H_
#define CYBEL_GFX_FONT_ATLAS_H_

#include "cybel/common.h"

#include "sprite_atlas.h"
#include "texture.h"

#include <unordered_map>
#include <vector>

namespace cybel {

class FontAtlas : public SpriteAtlas {
public:
  class Builder : public SpriteAtlas::Builder {
  public:
    explicit Builder(Texture&& texture);
    explicit Builder(const std::shared_ptr<Texture>& texture);

    FontAtlas build();

    // Re-define base funcs to return derived Builder (instead of base Builder).
    Builder& offset(int x,int y);
    Builder& cell_size(int width,int height);
    Builder& cell_padding(int padding);
    Builder& grid_size(int cols,int rows);

    Builder& spacing(int char_spacing,int line_spacing);
    Builder& default_index(int index);
    Builder& default_index(int col,int row);
    Builder& index_to_char(const StrUtf8& str);
    Builder& index_to_char(const std::vector<StrUtf8>& lines);

    friend class FontAtlas;

  protected:
    Size2i spacing_{};
    int default_index_ = 0;
    Pos2i default_cell_{};
    std::unordered_map<char32_t,int> char_to_index_{};

  private:
    using Base = SpriteAtlas::Builder;
  };

  const Size2i& spacing() const;
  int char_index(char32_t c) const;

protected:
  Size2i spacing_{};
  int default_index_ = 0;
  std::unordered_map<char32_t,int> char_to_index_{};

  explicit FontAtlas(const Builder& builder);
};

} // Namespace.
#endif
