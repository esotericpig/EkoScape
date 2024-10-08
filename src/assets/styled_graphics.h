/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_STYLED_GRAPHICS_H_
#define EKOSCAPE_ASSETS_STYLED_GRAPHICS_H_

#include "common.h"

#include "cybel/gfx/texture.h"
#include "cybel/util/cybel_error.h"
#include "cybel/util/util.h"

#include <filesystem>
#include <vector>

namespace ekoscape {

/**
 * This can contain Textures, Sprites, Fonts, etc.
 */
class StyledGraphics {
public:
  // If add a new one, update reload().
  enum class Style : int {
    kClassic = 0,
    kRealistic,
  };

  struct Graphics {
    Style style{};
    std::string name{};

    std::unique_ptr<Texture> ceiling_texture{};
    std::unique_ptr<Texture> cell_texture{};
    std::unique_ptr<Texture> dead_space_texture{};
    std::unique_ptr<Texture> dead_space_ghost_texture{};
    std::unique_ptr<Texture> end_texture{};
    std::unique_ptr<Texture> end_wall_texture{};
    std::unique_ptr<Texture> floor_texture{};
    std::unique_ptr<Texture> fruit_texture{};
    std::unique_ptr<Texture> portal_texture{};
    std::unique_ptr<Texture> robot_texture{};
    std::unique_ptr<Texture> wall_texture{};
    std::unique_ptr<Texture> wall_ghost_texture{};
    std::unique_ptr<Texture> white_texture{};
    std::unique_ptr<Texture> white_ghost_texture{};
  };

  explicit StyledGraphics(const std::filesystem::path& textures_dir,Style style,bool make_weird = false);

  void reload(bool make_weird);

  const std::string& prev_style();
  const std::string& next_style();

  Style style() const;
  const std::string& style_name() const;
  const Graphics& graphics() const;

private:
  const std::filesystem::path textures_dir_;

  std::vector<Graphics> graphics_bag_{};
  int graphics_bag_index_ = 0;
  Graphics* graphics_ = nullptr;

  void reload(Style style,bool make_weird);
  Graphics load_style(Style style,const std::string& dirname,bool make_weird);
};

} // Namespace.
#endif
