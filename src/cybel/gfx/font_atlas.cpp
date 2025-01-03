/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "font_atlas.h"

namespace cybel {

FontAtlas::Builder::Builder(Texture&& tex)
    : sprite_atlas_(std::move(tex)) {}

FontAtlas::Builder::Builder(std::unique_ptr<Texture> tex)
    : sprite_atlas_(std::move(tex)) {}

FontAtlas::Builder::Builder(const std::shared_ptr<Texture>& tex)
    : sprite_atlas_(tex) {}

FontAtlas FontAtlas::Builder::build() { return FontAtlas{*this}; }

FontAtlas::Builder& FontAtlas::Builder::offset(int x,int y) {
  sprite_atlas_.offset(x,y);
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::cell_size(int width,int height) {
  sprite_atlas_.cell_size(width,height);
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::cell_padding(int padding) {
  sprite_atlas_.cell_padding(padding);
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::grid_size(int cols,int rows) {
  sprite_atlas_.grid_size(cols,rows);
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::spacing(int char_spacing,int line_spacing) {
  spacing_.w = char_spacing;
  spacing_.h = line_spacing;
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::default_index(int index) {
  default_index_ = index;
  default_cell_.x = 0;
  default_cell_.y = 0;
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::default_index(int col,int row) {
  default_index_ = 0;
  default_cell_.x = col;
  default_cell_.y = row;
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::index_to_char(const StrUtf8& str) {
  int index = 0;

  for(auto c: str) {
    char_to_index_[c] = index;
    ++index;
  }

  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::index_to_char(const std::vector<StrUtf8>& lines) {
  int index = 0;
  int col_count = 0;

  for(const auto& line: lines) {
    int len = static_cast<int>(line.length());
    if(len > col_count) { col_count = len; }

    for(auto c: line) {
      char_to_index_[c] = index;
      ++index;
    }
  }

  auto grid_size = sprite_atlas_.grid_size();

  if(grid_size.w <= 0) { grid_size.w = col_count; }
  if(grid_size.h <= 0) { grid_size.h = static_cast<int>(lines.size()); }

  sprite_atlas_.grid_size(grid_size.w,grid_size.h);

  return *this;
}

FontAtlas::FontAtlas(const Builder& builder)
    : SpriteAtlas(builder.sprite_atlas_)
      ,spacing_(builder.spacing_)
      ,char_to_index_(builder.char_to_index_) {
  if(builder.default_cell_.x > 0 || builder.default_cell_.y > 0) {
    default_index_ = builder.default_cell_.x + (builder.default_cell_.y * grid_size_.w);
  } else {
    default_index_ = builder.default_index_;
  }
}

const Size2i& FontAtlas::spacing() const { return spacing_; }

int FontAtlas::char_index(char32_t c) const {
  auto it = char_to_index_.find(c);

  if(it == char_to_index_.end()) { return default_index_; }
  return it->second;
}

} // Namespace.
