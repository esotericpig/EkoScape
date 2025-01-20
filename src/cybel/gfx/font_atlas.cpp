/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "font_atlas.h"

#include "cybel/str/utf8/rune_range.h"

namespace cybel {

FontAtlas::Builder::Builder(Texture&& tex)
    : sprite_atlas_(std::move(tex)) {}

FontAtlas::Builder::Builder(std::unique_ptr<Texture> tex)
    : sprite_atlas_(std::move(tex)) {}

FontAtlas::Builder::Builder(std::shared_ptr<Texture> tex)
    : sprite_atlas_(std::move(tex)) {}

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

FontAtlas::Builder& FontAtlas::Builder::spacing(int rune_spacing,int line_spacing) {
  spacing_.w = rune_spacing;
  spacing_.h = line_spacing;

  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::default_index(int index) {
  default_index_ = index;
  default_cell_.x = 0;
  default_cell_.y = 0;
  default_rune_ = 0;

  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::default_cell(int col,int row) {
  default_index_ = 0;
  default_cell_.x = col;
  default_cell_.y = row;
  default_rune_ = 0;

  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::default_rune(char32_t rune) {
  default_index_ = 0;
  default_cell_.x = 0;
  default_cell_.y = 0;
  default_rune_ = rune;

  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::index_to_rune(std::string_view str) {
  int index = 0;

  for(auto rune: utf8::RuneRange{str}) {
    rune_to_index_[rune] = index;
    ++index;
  }

  return *this;
}

FontAtlas::Builder& FontAtlas::Builder::index_to_rune(std::initializer_list<std::string_view> lines) {
  int index = 0;
  int col_count = 0;

  for(const auto& line: lines) {
    int len = 0;

    for(auto rune: utf8::RuneRange{line}) {
      rune_to_index_[rune] = index;
      ++index;
      ++len;
    }

    if(len > col_count) { col_count = len; }
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
      ,rune_to_index_(builder.rune_to_index_) {
  if(builder.default_index_ > 0) {
    default_index_ = builder.default_index_;
  } else if(builder.default_cell_.x > 0 || builder.default_cell_.y > 0) {
    default_index_ = builder.default_cell_.x + (builder.default_cell_.y * grid_size_.w);
  } else if(builder.default_rune_ != 0) {
    for(auto [rune,index]: rune_to_index_) {
      if(rune == builder.default_rune_) {
        default_index_ = index;
        break;
      }
    }
  }

  if(default_index_ >= rune_to_index_.size()) {
    default_index_ = !rune_to_index_.empty() ? (rune_to_index_.size() - 1) : 0;
  }
}

const Size2i& FontAtlas::spacing() const { return spacing_; }

int FontAtlas::rune_index(char32_t rune) const {
  auto it = rune_to_index_.find(rune);

  return (it != rune_to_index_.end()) ? it->second : default_index_;
}

} // Namespace.
