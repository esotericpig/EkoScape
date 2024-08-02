/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "assets.h"

namespace ekoscape {

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

  std::string dir = "assets/textures/";

  switch(textures_type_) {
    case TexturesType::kRealistic:
      dir += "realistic";
      break;

    default:
      dir += "classic";
      break;
  }
  dir += '/';

  Image ceiling_img = Image{dir + "ceiling.png"};
  Image cell_img = Image{dir + "cell.png"};
  Image end_img = Image{dir + "end.png"};
  Image floor_img = Image{dir + "floor.png"};
  Image robot_img = Image{dir + "robot.png"};
  Image wall_img = Image{dir + "wall.png"};

  ceiling_texture_ = std::make_unique<Texture>(ceiling_img);
  cell_texture_ = std::make_unique<Texture>(cell_img);
  end_texture_ = std::make_unique<Texture>(end_img);
  floor_texture_ = std::make_unique<Texture>(floor_img);
  robot_texture_ = std::make_unique<Texture>(robot_img);
  wall_texture_ = std::make_unique<Texture>(wall_img);
  white_texture_ = std::make_unique<Texture>(255,255,255,255);
}

void Assets::reload_music() {
  if(!has_music_player_) { return; }

  try {
    //music_ = std::make_unique<Music>("assets/music/matrix.mid");
  } catch(const EkoScapeError& e) {
    std::cerr << "[WARN] " << e.what() << std::endl;
    // Don't fail, since music is optional.
  }
}

const Texture& Assets::ceiling_texture() const { return *ceiling_texture_.get(); }

const Texture& Assets::cell_texture() const { return *cell_texture_.get(); }

const Texture& Assets::end_texture() const { return *end_texture_.get(); }

const Texture& Assets::floor_texture() const { return *floor_texture_.get(); }

const Texture& Assets::robot_texture() const { return *robot_texture_.get(); }

const Texture& Assets::wall_texture() const { return *wall_texture_.get(); }

const Texture& Assets::white_texture() const { return *white_texture_.get(); }

const Music* Assets::music() const { return music_.get(); }

} // Namespace.
