/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sprite_atlas.h"

namespace cybel {

SpriteAtlas::Builder::Builder(Texture&& texture)
    : Builder(std::make_shared<Texture>(std::move(texture))) {}

SpriteAtlas::Builder::Builder(std::shared_ptr<Texture> texture)
    : texture_(texture) {}

SpriteAtlas SpriteAtlas::Builder::build() { return SpriteAtlas(*this); }

SpriteAtlas::Builder& SpriteAtlas::Builder::offset(int x,int y) {
  offset_.x = x;
  offset_.y = y;
  return *this;
}

SpriteAtlas::Builder& SpriteAtlas::Builder::cell_size(int width,int height) {
  cell_size_.w = width;
  cell_size_.h = height;
  return *this;
}

SpriteAtlas::Builder& SpriteAtlas::Builder::cell_padding(int padding) {
  cell_padding_ = padding;
  return *this;
}

SpriteAtlas::Builder& SpriteAtlas::Builder::grid_size(int columns,int rows) {
  grid_size_.w = columns;
  grid_size_.h = rows;
  return *this;
}

SpriteAtlas::SpriteAtlas(const Builder& builder)
    : texture_(builder.texture_)
      ,grid_size_(builder.grid_size_)
      ,cell_count_(grid_size_.w * grid_size_.h)
      ,index_to_src_(cell_count_) {
  const int p2 = builder.cell_padding_ * 2;

  cell_size_.w = builder.cell_size_.w - p2;
  cell_size_.h = builder.cell_size_.h - p2;

  const int x_offset = builder.offset_.x + builder.cell_padding_;
  const int y_offset = builder.offset_.y + builder.cell_padding_;

  for(int i = 0; i < cell_count_; ++i) {
    const int column = i % grid_size_.w;
    const int row = i / grid_size_.w;
    const Pos2i offset{
      x_offset + (builder.cell_size_.w * column),
      y_offset + (builder.cell_size_.h * row),
    };

    index_to_src_.at(i) = Sprite::build_src(*texture_,offset,cell_size_);
  }
}

const Texture& SpriteAtlas::texture() const { return *texture_; }

const Pos4f* SpriteAtlas::src(int index) const {
  if(index < 0 || index >= cell_count_) { return nullptr; }

  return &index_to_src_.at(index);
}

const Pos4f* SpriteAtlas::src(int column,int row) const {
  return src(column + (row * grid_size_.w));
}

const Size2i& SpriteAtlas::cell_size() const { return cell_size_; }

const Size2i& SpriteAtlas::grid_size() const { return grid_size_; }

int SpriteAtlas::cell_count() const { return cell_count_; }

} // Namespace.
