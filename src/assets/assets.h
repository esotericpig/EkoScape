/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_ASSETS_H_
#define EKOSCAPE_ASSETS_ASSETS_H_

#include "common.h"

#include "cybel/audio/music.h"
#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/texture.h"
#include "cybel/util/cybel_error.h"

#include "font_renderer.h"
#include "styled_graphics.h"

#include <cstdlib>
#include <filesystem>

namespace ekoscape {

class Assets {
private:
  static inline const std::string kAssetsDirname = "assets";

  /**
   * This is an expensive operation that should only be called once.
   */
  static std::filesystem::path fetch_assets_path();

public:
  const std::filesystem::path kAssetsDir{fetch_assets_path()};
  const std::filesystem::path kIconsDir{kAssetsDir / "icons"};
  const std::filesystem::path kImagesDir{kAssetsDir / "images"};
  const std::filesystem::path kMapsDir{kAssetsDir / "maps"};
  const std::filesystem::path kMusicDir{kAssetsDir / "music"};
  const std::filesystem::path kTexturesDir{kAssetsDir / "textures"};

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
  const Texture& fruit_texture() const;
  const Texture& portal_texture() const;
  const Texture& robot_texture() const;
  const Texture& wall_texture() const;
  const Texture& white_texture() const;
  const Texture& star_texture() const;

  const Image& icon_image() const;
  const Sprite& logo_sprite() const;
  const Sprite& keys_sprite() const;
  const Sprite& dantares_sprite() const;
  const Sprite& boring_work_sprite() const;
  const Sprite& goodnight_sprite() const;
  const Sprite& corngrits_sprite() const;

  FontRenderer& font_renderer() const;
  const FontAtlas& font_atlas() const;

  const Color4f& eko_color() const;
  const Color4f& end_color() const;
  const Color4f& fruit_color() const;
  const Color4f& portal_color() const;
  const Color4f& robot_color() const;
  const Color4f& wall_color() const;

  const Music* music() const;

private:
  bool is_weird_ = false;
  StyledGraphics styled_graphics_;
  bool has_music_player_ = false;

  Texture* star_texture_ = nullptr;
  std::unique_ptr<Texture> star1_texture_{};
  std::unique_ptr<Texture> star2_texture_{};

  std::unique_ptr<Image> icon_image_{};
  std::unique_ptr<Sprite> logo_sprite_{};
  std::unique_ptr<Sprite> keys_sprite_{};
  std::unique_ptr<Sprite> dantares_sprite_{};
  std::unique_ptr<Sprite> boring_work_sprite_{};
  std::unique_ptr<Sprite> goodnight_sprite_{};
  std::unique_ptr<Sprite> corngrits_sprite_{};

  std::unique_ptr<FontAtlas> font_atlas_{};
  std::unique_ptr<FontRenderer> font_renderer_{};

  Color4f eko_color_{}; // Cell & Player.
  Color4f end_color_{};
  Color4f fruit_color_{};
  Color4f portal_color_{};
  Color4f robot_color_{};
  Color4f wall_color_{};

  std::unique_ptr<Music> music_{};
};

} // Namespace.
#endif
