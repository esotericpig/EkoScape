/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_FONT_ATLAS_H_
#define CYBEL_GFX_FONT_ATLAS_H_

#include "cybel/common.h"

#include "cybel/gfx/sprite_atlas.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/pos.h"
#include "cybel/types/size.h"

#include <unordered_map>

namespace cybel {

class FontAtlas : public SpriteAtlas {
public:
  class Builder final {
  public:
    explicit Builder(Texture&& tex);
    explicit Builder(std::unique_ptr<Texture> tex);
    explicit Builder(std::shared_ptr<Texture> tex);

    FontAtlas build();

    Builder& offset(int x,int y);
    Builder& cell_size(int width,int height);
    Builder& cell_padding(int padding);
    Builder& grid_size(int cols,int rows);

    Builder& spacing(int rune_spacing,int line_spacing);
    Builder& default_index(int index);
    Builder& default_cell(int col,int row);
    Builder& default_rune(char32_t rune);
    Builder& index_to_rune(std::string_view str);
    Builder& index_to_rune(std::initializer_list<std::string_view> lines);

    friend class FontAtlas;

  private:
    SpriteAtlas::Builder sprite_atlas_;
    Size2i spacing_{};

    // Since we don't know the final grid width & height until build,
    //     we have to store both `default_index_` & `default_cell_`.
    int default_index_ = 0;
    Pos2i default_cell_{};
    char32_t default_rune_ = 0;

    std::unordered_map<char32_t,int> rune_to_index_{};
  };

  const Size2i& spacing() const;
  int rune_index(char32_t rune) const;

protected:
  Size2i spacing_{};
  int default_index_ = 0;
  std::unordered_map<char32_t,int> rune_to_index_{};

  explicit FontAtlas(const Builder& builder);
};

} // Namespace.
#endif
