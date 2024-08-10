/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "assets.h"

namespace ekoscape {

const std::string Assets::kAssetsDir = "assets";

Assets::Assets(TexturesType textures_type,bool has_music_player)
    : has_music_player_(has_music_player) {
  reload_textures(textures_type);
  reload_music();
}

void Assets::reload_textures() {
  reload_textures(textures_type_);
}

void Assets::reload_textures(TexturesType type) {
  textures_type_ = type;

  const std::string img_dir = kAssetsDir + "/images";
  const std::string tex_dir = build_textures_dir();

  ceiling_texture_ = std::make_unique<Texture>(Image{tex_dir + "/ceiling.png"});
  cell_texture_ = std::make_unique<Texture>(Image{tex_dir + "/cell.png"});
  end_texture_ = std::make_unique<Texture>(Image{tex_dir + "/end.png"});
  floor_texture_ = std::make_unique<Texture>(Image{tex_dir + "/floor.png"});
  robot_texture_ = std::make_unique<Texture>(Image{tex_dir + "/robot.png"});
  wall_texture_ = std::make_unique<Texture>(Image{tex_dir + "/wall.png"});
  white_texture_ = std::make_unique<Texture>(255,255,255,255);

  font_texture_ = std::make_unique<Texture>(Image{img_dir + "/font_monogram.png"});
  font_atlas_ = std::make_unique<FontAtlas>(
    FontAtlas::Builder{*font_texture_}
      .offset(0,0)
      .cell_size(9,14)
      .cell_padding(2)
      .spacing(5,5)
      .default_index(0)
      .index_to_char({
        R"( !"#$%&'()*+,-./)",
        R"(0123456789:;<=>?)",
        R"(@ABCDEFGHIJKLMNO)",
        R"(PQRSTUVWXYZ[\]^_)",
        R"(`abcdefghijklmno)",
        R"(pqrstuvwxyz{|}~…)",
        R"(¿¡←↑→↓©®×÷±«¤»¬¯)",
        R"(₀₁₂₃₄₅₆₇₈₉°ªº£¥¢)",
      })
      .build()
  );
}

void Assets::reload_music() {
  if(!has_music_player_) { return; }

  try {
    //music_ = std::make_unique<Music>(kAssetsDir + "/music/matrix.mid");
  } catch(const EkoScapeError& e) {
    std::cerr << "[WARN] " << e.what() << std::endl;
    // Don't fail, since music is optional.
  }
}

std::string Assets::build_textures_dir() const {
  std::string dir = kAssetsDir + "/textures";

  switch(textures_type_) {
    case TexturesType::kRealistic:
      dir += "/realistic";
      break;

    default:
      dir += "/classic";
      break;
  }

  return dir;
}

const Texture& Assets::ceiling_texture() const { return *ceiling_texture_; }

const Texture& Assets::cell_texture() const { return *cell_texture_; }

const Texture& Assets::end_texture() const { return *end_texture_; }

const Texture& Assets::floor_texture() const { return *floor_texture_; }

const Texture& Assets::robot_texture() const { return *robot_texture_; }

const Texture& Assets::wall_texture() const { return *wall_texture_; }

const Texture& Assets::white_texture() const { return *white_texture_; }

const Texture& Assets::font_texture() const { return *font_texture_; }

const FontAtlas& Assets::font_atlas() const { return *font_atlas_; }

const Music* Assets::music() const { return music_.get(); }

} // Namespace.
