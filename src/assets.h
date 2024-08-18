/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_H_
#define EKOSCAPE_ASSETS_H_

#include "core/common.h"

#include "core/audio/music.h"
#include "core/gfx/font_atlas.h"
#include "core/gfx/image.h"
#include "core/gfx/sprite.h"
#include "core/gfx/texture.h"
#include "core/cybel_error.h"
#include "styled_graphics.h"

#include <filesystem>

namespace ekoscape {

class Assets {
public:
  static const std::filesystem::path kAssetsDir;

  Assets(StyledGraphics::Style graphics_style,bool has_music_player);

  void reload_graphics();
  void reload_music();

  const std::string& prev_graphics_style();
  const std::string& next_graphics_style();

  StyledGraphics::Style graphics_style() const;
  const std::string& graphics_style_name() const;

  const Texture& ceiling_texture() const;
  const Texture& cell_texture() const;
  const Texture& end_texture() const;
  const Texture& floor_texture() const;
  const Texture& robot_texture() const;
  const Texture& wall_texture() const;
  const Texture& white_texture() const;

  const Sprite& logo_sprite() const;
  const FontAtlas& font_atlas() const;
  const Sprite& keys_sprite() const;
  const Sprite& boring_work_sprite() const;

  const Music* music() const;

private:
  StyledGraphics styled_graphics_;
  const bool has_music_player_;

  std::unique_ptr<Sprite> logo_sprite_{};
  std::unique_ptr<FontAtlas> font_atlas_{};
  std::unique_ptr<Sprite> keys_sprite_{};
  std::unique_ptr<Sprite> boring_work_sprite_{};

  std::unique_ptr<Music> music_{};

  void reload_images();
};

} // Namespace.
#endif
