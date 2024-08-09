/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_FONT_ATLAS_H_
#define EKOSCAPE_CORE_FONT_ATLAS_H_

#include "common.h"

#include "sprite_atlas.h"

#include <unordered_map>
#include <vector>

namespace ekoscape {

class FontAtlas : public SpriteAtlas {
public:
  class Builder : public SpriteAtlas::Builder {
  public:
    Builder(const Texture& texture);

    FontAtlas build();

    // Re-define base funcs to return our derived Builder, instead of the base Builder.
    Builder& offset(int x,int y);
    Builder& cell_size(int width,int height);
    Builder& cell_padding(int padding);
    Builder& grid_size(int columns,int rows);

    Builder& spacing(int char_spacing,int line_spacing);
    Builder& default_index(int index);
    Builder& default_index(int column,int row);
    Builder& index_to_char(const std::string& str);
    Builder& index_to_char(const std::vector<std::string>& lines);

    friend class FontAtlas;

  protected:
    Size2i spacing_{};
    int default_index_ = 0;
    Pos2i default_cell_{};
    std::unordered_map<char,int> char_to_index_{};

  private:
    using Base = SpriteAtlas::Builder;
  };

  const Size2i& spacing() const;
  int char_index(char c) const;

protected:
  Size2i spacing_;
  int default_index_ = 0;
  std::unordered_map<char,int> char_to_index_;

  FontAtlas(const Builder& builder);
};

} // Namespace.
#endif
