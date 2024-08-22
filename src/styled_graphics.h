/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_STYLED_GRAPHICS_H_
#define EKOSCAPE_STYLED_GRAPHICS_H_

#include "core/common.h"

#include "core/gfx/texture.h"
#include "core/util/cybel_error.h"
#include "core/util/util.h"

#include <filesystem>
#include <vector>

namespace ekoscape {

using namespace cybel;

/**
 * This can contain Textures, Sprites, Fonts, etc.
 */
class StyledGraphics {
public:
  /**
   * If add a new one, update reload().
   */
  enum class Style : int {
    kClassic = 0,
    kRealistic,
  };

  struct Graphics {
    Style style{};
    std::string name{};

    std::unique_ptr<Texture> ceiling_texture{};
    std::unique_ptr<Texture> cell_texture{};
    std::unique_ptr<Texture> end_texture{};
    std::unique_ptr<Texture> floor_texture{};
    std::unique_ptr<Texture> robot_texture{};
    std::unique_ptr<Texture> wall_texture{};
    std::unique_ptr<Texture> white_texture{};
  };

  StyledGraphics(const std::filesystem::path& textures_dir,Style style);

  void reload();

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

  void reload(Style style);
  Graphics load_style(Style style,const std::string& dirname);
};

} // Namespace.
#endif
