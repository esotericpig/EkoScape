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

  ceiling_texture_ = std::make_unique<Texture>(Image{dir + "ceiling.png"});
  cell_texture_ = std::make_unique<Texture>(Image{dir + "cell.png"});
  end_texture_ = std::make_unique<Texture>(Image{dir + "end.png"});
  floor_texture_ = std::make_unique<Texture>(Image{dir + "floor.png"});
  robot_texture_ = std::make_unique<Texture>(Image{dir + "robot.png"});
  wall_texture_ = std::make_unique<Texture>(Image{dir + "wall.png"});
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
