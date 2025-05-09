/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sprite_atlas.h"

#include "cybel/gfx/sprite.h"
#include "cybel/types/cybel_error.h"

namespace cybel {

SpriteAtlas::SpriteAtlas(const Builder& builder)
  : tex_(builder.tex_),
    grid_size_(builder.grid_size_),
    index_to_src_(static_cast<std::size_t>(grid_size_.area()),Pos4f{}) {
  if(!tex_) { throw CybelError{"Texture is null on SpriteAtlas."}; }

  const int p2 = builder.cell_padding_ * 2;

  cell_size_.w = builder.cell_size_.w - p2;
  cell_size_.h = builder.cell_size_.h - p2;

  const int x_offset = builder.offset_.x + builder.cell_padding_;
  const int y_offset = builder.offset_.y + builder.cell_padding_;

  for(std::size_t i = 0; i < index_to_src_.size(); ++i) {
    const int col = static_cast<int>(i) % grid_size_.w;
    const int row = static_cast<int>(i) / grid_size_.w;
    const Pos2i offset{
      x_offset + (builder.cell_size_.w * col),
      y_offset + (builder.cell_size_.h * row),
    };

    index_to_src_[i] = Sprite::build_src(*tex_,offset,cell_size_);
  }
}

void SpriteAtlas::zombify() { tex_->zombify(); }

const Texture& SpriteAtlas::tex() const { return *tex_; }

const Pos4f* SpriteAtlas::src(std::size_t index) const {
  if(index >= index_to_src_.size()) { return nullptr; }

  return &index_to_src_[index];
}

const Pos4f* SpriteAtlas::src(const Pos2i& cell) const {
  return src(static_cast<std::size_t>(cell.x + (cell.y * grid_size_.w)));
}

const Size2i& SpriteAtlas::cell_size() const { return cell_size_; }

const Size2i& SpriteAtlas::grid_size() const { return grid_size_; }

std::size_t SpriteAtlas::cell_count() const { return index_to_src_.size(); }

SpriteAtlas SpriteAtlas::Builder::build() { return SpriteAtlas{*this}; }

SpriteAtlas::Builder& SpriteAtlas::Builder::tex(Texture&& tex) {
  tex_ = std::make_shared<Texture>(std::move(tex));

  return *this;
}

SpriteAtlas::Builder& SpriteAtlas::Builder::tex(std::unique_ptr<Texture> tex) {
  tex_ = std::move(tex);

  return *this;
}

SpriteAtlas::Builder& SpriteAtlas::Builder::tex(std::shared_ptr<Texture> tex) {
  tex_ = std::move(tex);

  return *this;
}

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

} // namespace cybel
