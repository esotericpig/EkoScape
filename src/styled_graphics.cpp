/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "styled_graphics.h"

namespace ekoscape {

StyledGraphics::StyledGraphics(const std::filesystem::path& assets_dir,Style style)
    : assets_dir_(assets_dir) {
  reload(style);

  if(graphics_ == nullptr) { throw CybelError{"Graphics pointer was not set."}; }
}

void StyledGraphics::reload() {
  reload(graphics_->style);
}

void StyledGraphics::reload(Style style) {
  graphics_bag_.clear();
  graphics_bag_.emplace_back(load_style(Style::kClassic,"classic"));
  graphics_bag_.emplace_back(load_style(Style::kRealistic,"realistic"));
  graphics_bag_.shrink_to_fit();

  graphics_bag_index_ = -1;

  for(std::size_t i = 0; i < graphics_bag_.size(); ++i) {
    if(graphics_bag_[i].style == style) {
      graphics_bag_index_ = static_cast<int>(i);
      break;
    }
  }

  if(graphics_bag_index_ < 0) {
    throw CybelError{Util::build_string("Graphics style [",static_cast<int>(style),"] was not loaded.")};
  }

  graphics_ = &graphics_bag_.at(graphics_bag_index_);
}

StyledGraphics::Graphics StyledGraphics::load_style(Style style,const std::string& dirname) {
  const auto tex_dir = assets_dir_ / "textures" / dirname;

  Graphics graphics{};

  graphics.style = style;
  graphics.name = dirname;

  graphics.ceiling_texture = std::make_unique<Texture>(Image{tex_dir / "ceiling.png"});
  graphics.cell_texture = std::make_unique<Texture>(Image{tex_dir / "cell.png"});
  graphics.end_texture = std::make_unique<Texture>(Image{tex_dir / "end.png"});
  graphics.floor_texture = std::make_unique<Texture>(Image{tex_dir / "floor.png"});
  graphics.robot_texture = std::make_unique<Texture>(Image{tex_dir / "robot.png"});
  graphics.wall_texture = std::make_unique<Texture>(Image{tex_dir / "wall.png"});
  graphics.white_texture = std::make_unique<Texture>(255,255,255,255);

  return graphics;
}

const std::string& StyledGraphics::prev_style() {
  --graphics_bag_index_;

  if(graphics_bag_index_ < 0) {
    graphics_bag_index_ = static_cast<int>(graphics_bag_.size()) - 1;
  }

  graphics_ = &graphics_bag_.at(graphics_bag_index_);
  return graphics_->name;
}

const std::string& StyledGraphics::next_style() {
  ++graphics_bag_index_;

  if(graphics_bag_index_ >= static_cast<int>(graphics_bag_.size())) {
    graphics_bag_index_ = 0;
  }

  graphics_ = &graphics_bag_.at(graphics_bag_index_);
  return graphics_->name;
}

StyledGraphics::Style StyledGraphics::style() const { return graphics_->style; }

const std::string& StyledGraphics::style_name() const { return graphics_->name; }

const StyledGraphics::Graphics& StyledGraphics::graphics() const { return *graphics_; }

} // Namespace.
