/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sprite_atlas.h"

#include "cybel/gfx/sprite.h"

namespace cybel {

SpriteAtlas::Builder::Builder(Texture&& tex)
  : Builder(std::make_shared<Texture>(std::move(tex))) {}

SpriteAtlas::Builder::Builder(std::unique_ptr<Texture> tex)
  : Builder(std::shared_ptr{std::move(tex)}) {}

SpriteAtlas::Builder::Builder(std::shared_ptr<Texture> tex)
  : tex_(std::move(tex)) {}

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

SpriteAtlas::Builder& SpriteAtlas::Builder::grid_size(int cols,int rows) {
  grid_size_.w = cols;
  grid_size_.h = rows;

  return *this;
}

std::shared_ptr<Texture> SpriteAtlas::Builder::tex() const { return tex_; }

const Pos2i& SpriteAtlas::Builder::offset() const { return offset_; }

const Size2i& SpriteAtlas::Builder::cell_size() const { return cell_size_; }

int SpriteAtlas::Builder::cell_padding() const { return cell_padding_; }

const Size2i& SpriteAtlas::Builder::grid_size() const { return grid_size_; }

SpriteAtlas::SpriteAtlas(const Builder& builder)
  : tex_(builder.tex_),
    grid_size_(builder.grid_size_),
    cell_count_(grid_size_.w * grid_size_.h),
    index_to_src_(cell_count_,Pos4f{}) {
  const int p2 = builder.cell_padding_ * 2;

  cell_size_.w = builder.cell_size_.w - p2;
  cell_size_.h = builder.cell_size_.h - p2;

  const int x_offset = builder.offset_.x + builder.cell_padding_;
  const int y_offset = builder.offset_.y + builder.cell_padding_;

  for(int i = 0; i < cell_count_; ++i) {
    const int col = i % grid_size_.w;
    const int row = i / grid_size_.w;
    const Pos2i offset{
      x_offset + (builder.cell_size_.w * col),
      y_offset + (builder.cell_size_.h * row),
    };

    index_to_src_[i] = Sprite::build_src(*tex_,offset,cell_size_);
  }
}

void SpriteAtlas::zombify() { tex_->zombify(); }

const Texture& SpriteAtlas::tex() const { return *tex_; }

const Pos4f* SpriteAtlas::src(int index) const {
  if(index < 0 || index >= cell_count_) { return nullptr; }

  return &index_to_src_.at(index);
}

const Pos4f* SpriteAtlas::src(const Pos2i& cell) const {
  return src(cell.x + (cell.y * grid_size_.w));
}

const Size2i& SpriteAtlas::cell_size() const { return cell_size_; }

const Size2i& SpriteAtlas::grid_size() const { return grid_size_; }

int SpriteAtlas::cell_count() const { return cell_count_; }

} // namespace cybel
