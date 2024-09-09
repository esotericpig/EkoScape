/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_ASSETS_H_
#define EKOSCAPE_ASSETS_ASSETS_H_

#include "cybel/common.h"

#include "cybel/audio/music.h"
#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/texture.h"
#include "cybel/util/cybel_error.h"
#include "cybel/types.h"

#include "font_renderer.h"
#include "styled_graphics.h"

#include <filesystem>

namespace ekoscape {

class Assets {
public:
  static const std::filesystem::path kAssetsDir;
  static const std::filesystem::path kImagesDir;
  static const std::filesystem::path kMapsDir;
  static const std::filesystem::path kTexturesDir;

  explicit Assets(StyledGraphics::Style graphics_style,bool has_music_player,bool make_weird = false);

  void reload_graphics();
  void reload_graphics(bool make_weird);
  void reload_music();

  const std::string& prev_graphics_style();
  const std::string& next_graphics_style();

  bool is_weird() const;
  StyledGraphics::Style graphics_style() const;
  const std::string& graphics_style_name() const;

  const Texture& ceiling_texture() const;
  const Texture& cell_texture() const;
  const Texture& end_texture() const;
  const Texture& floor_texture() const;
  const Texture& robot_texture() const;
  const Texture& wall_texture() const;
  const Texture& white_texture() const;

  const Texture& star_texture() const;

  const Sprite& logo_sprite() const;
  const Sprite& keys_sprite() const;
  const Sprite& dantares_sprite() const;
  const Sprite& boring_work_sprite() const;

  FontRenderer& font_renderer() const;

  const Music* music() const;

private:
  static const Size2i kFontSize;
  static const Color4f kFontColor1;
  static const Color4f kFontColor2;

  bool is_weird_ = false;
  StyledGraphics styled_graphics_;
  bool has_music_player_ = false;

  Texture* star_texture_ = nullptr;
  std::unique_ptr<Texture> star1_texture_{};
  std::unique_ptr<Texture> star2_texture_{};

  std::unique_ptr<Sprite> logo_sprite_{};
  std::unique_ptr<Sprite> keys_sprite_{};
  std::unique_ptr<Sprite> dantares_sprite_{};
  std::unique_ptr<Sprite> boring_work_sprite_{};

  std::unique_ptr<FontAtlas> font_atlas_{};
  const Color4f* font_color_ = &kFontColor1;
  std::unique_ptr<FontRenderer> font_renderer_{};

  std::unique_ptr<Music> music_{};

  static std::filesystem::path fetch_base_path();
};

} // Namespace.
#endif
