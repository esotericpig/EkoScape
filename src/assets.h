/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_H_
#define EKOSCAPE_ASSETS_H_

#include "core/common.h"

#include "core/ekoscape_error.h"
#include "core/font_atlas.h"
#include "core/image.h"
#include "core/music.h"
#include "core/texture.h"

namespace ekoscape {

class Assets {
public:
  enum class TexturesType {
    kClassic,
    kRealistic,
  };

  static const std::string kAssetsDir;

  Assets(TexturesType textures_type,bool has_music_player);

  void reload_textures();
  void reload_textures(TexturesType type);
  void reload_music();

  std::string build_textures_dir() const;

  const Texture& ceiling_texture() const;
  const Texture& cell_texture() const;
  const Texture& end_texture() const;
  const Texture& floor_texture() const;
  const Texture& robot_texture() const;
  const Texture& wall_texture() const;
  const Texture& white_texture() const;

  const Texture& font_texture() const;
  const FontAtlas& font_atlas() const;

  const Music* music() const;

private:
  TexturesType textures_type_ = TexturesType::kClassic;
  std::unique_ptr<Texture> ceiling_texture_{};
  std::unique_ptr<Texture> cell_texture_{};
  std::unique_ptr<Texture> end_texture_{};
  std::unique_ptr<Texture> floor_texture_{};
  std::unique_ptr<Texture> robot_texture_{};
  std::unique_ptr<Texture> wall_texture_{};
  std::unique_ptr<Texture> white_texture_{};

  std::unique_ptr<Texture> font_texture_{};
  std::unique_ptr<FontAtlas> font_atlas_{};

  const bool has_music_player_;
  std::unique_ptr<Music> music_{};
};

} // Namespace.
#endif
