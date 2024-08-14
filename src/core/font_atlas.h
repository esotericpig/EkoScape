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
    explicit Builder(Texture&& texture);
    explicit Builder(std::shared_ptr<Texture> texture);

    FontAtlas build();

    // Re-define base funcs to return derived Builder, instead of base Builder.
    Builder& offset(int x,int y);
    Builder& cell_size(int width,int height);
    Builder& cell_padding(int padding);
    Builder& grid_size(int columns,int rows);

    Builder& spacing(int char_spacing,int line_spacing);
    Builder& default_index(int index);
    Builder& default_index(int column,int row);
    Builder& index_to_char(const tiny_utf8::string& str);
    Builder& index_to_char(const std::vector<tiny_utf8::string>& lines);

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
  Size2i spacing_;
  int default_index_ = 0;
  std::unordered_map<char32_t,int> char_to_index_;

  explicit FontAtlas(const Builder& builder);
};

} // Namespace.
#endif
