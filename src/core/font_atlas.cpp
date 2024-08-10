/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "font_atlas.h"

namespace ekoscape {

FontAtlas::Builder::Builder(const Texture& texture)
    : Base(texture) {}

FontAtlas FontAtlas::Builder::build() { return FontAtlas{*this}; }

FontAtlas::Builder& FontAtlas::Builder::offset(int x,int y) {
  Base::offset(x,y);
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::cell_size(int width,int height) {
  Base::cell_size(width,height);
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::cell_padding(int padding) {
  Base::cell_padding(padding);
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::grid_size(int columns,int rows) {
  Base::grid_size(columns,rows);
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

FontAtlas::Builder& FontAtlas::Builder::default_index(int column,int row) {
  default_index_ = 0;
  default_cell_.x = column;
  default_cell_.y = row;
  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::index_to_char(const tiny_utf8::string& str) {
  int index = 0;

  for(auto c: str) {
    char_to_index_[c] = index++;
  }

  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::index_to_char(const std::vector<tiny_utf8::string>& lines) {
  int index = 0;
  int column_count = 0;

  for(auto& line: lines) {
    int len = static_cast<int>(line.length());
    if(len > column_count) { column_count = len; }

    for(auto c: line) {
      char_to_index_[c] = index++;
    }
  }

  if(grid_size_.w == 0) { grid_size_.w = column_count; }
  if(grid_size_.h == 0) { grid_size_.h = static_cast<int>(lines.size()); }

  return *this;
}

// We don't do std::move() on char_to_index_ so that the Builder can be reused.
FontAtlas::FontAtlas(const Builder& builder)
    : SpriteAtlas(builder)
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
