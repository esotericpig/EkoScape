/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "assets.h"

namespace ekoscape {

std::filesystem::path Assets::fetch_assets_path() {
  // First, try current dir, so that the user can easily overwrite the assets (and it's fast).
  std::filesystem::path assets_path = ".";
  assets_path /= kAssetsDirname;

  if(is_directory(assets_path)) { return assets_path; }

  // Try our AppImage's path.
  const char* appimg_path = std::getenv("APPIMAGE");

  if(appimg_path != nullptr) {
    assets_path = appimg_path;
    assets_path = assets_path.parent_path() / kAssetsDirname;

    if(is_directory(assets_path)) { return assets_path; }
  }

  // Lastly, try our app's base dir (slowest).
  char* base_path = SDL_GetBasePath();

  if(base_path == NULL) {
    throw CybelError{"Failed to get base path of app: " + Util::get_sdl_error() + "."};
  }

  assets_path = base_path;
  assets_path /= kAssetsDirname;

  SDL_free(base_path);
  base_path = NULL;

  if(!is_directory(assets_path)) {
    throw CybelError{"Failed to find [" + kAssetsDirname + "] folder."};
  }

  return assets_path;
}

Assets::Assets(StyledGraphics::Style graphics_style,bool has_audio_player,bool make_weird)
    : styled_graphics_(kTexturesDir,graphics_style,make_weird),has_audio_player_(has_audio_player) {
  reload_graphics(make_weird);
  reload_music();
}

void Assets::reload_graphics() { reload_graphics(is_weird_); }

void Assets::reload_graphics(bool make_weird) {
  is_weird_ = make_weird;

  styled_graphics_.reload(is_weird_);

  star1_texture_ = std::make_unique<Texture>(Image{kTexturesDir / "star.png"});
  star2_texture_ = std::make_unique<Texture>(Image{kTexturesDir / "star2.png"});
  star_texture_ = is_weird_ ? star2_texture_.get() : star1_texture_.get();

  icon_image_ = std::make_unique<Image>(kIconsDir / "com.github.esotericpig.EkoScape.png");
  logo_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "EkoScape.png"},is_weird_});
  keys_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "keys.png"},is_weird_});
  dantares_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "Dantares.png"},is_weird_});
  boring_work_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "boring_work.png"},is_weird_});
  goodnight_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "goodnight.png"},is_weird_});
  corngrits_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "corngrits.png"},is_weird_});

  font_atlas_ = std::make_unique<FontAtlas>(
    FontAtlas::Builder{Texture{Image{kImagesDir / "font_monogram.png"}}}
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
  font_renderer_ = std::make_unique<FontRenderer>(*font_atlas_,is_weird_);

  eko_color_.set_hex(0xff0000);
  end_color_.set_hex(0xb87333); // Copper.
  fruit_color_.set_hex(0xffc0cb); // Pink.
  portal_color_.set_hex(0x00ffff); // Cyan.
  robot_color_.set_bytes(214);
  wall_color_.set_hex(0x00ff00);

  if(is_weird_) {
    std::swap(eko_color_.r,eko_color_.b);
    std::swap(end_color_.r,end_color_.b);
    std::swap(fruit_color_.r,fruit_color_.b);
    std::swap(portal_color_.r,portal_color_.b);
    std::swap(robot_color_.r,robot_color_.b);
    std::swap(wall_color_.r,wall_color_.b);
  }
}

void Assets::reload_music() {
  if(!has_audio_player_) { return; }

  try {
    music_ = std::make_unique<Music>(kMusicDir / "ekoscape.ogg");
  } catch(const CybelError& e) {
    std::cerr << "[WARN] " << e.what() << std::endl;
    // Don't fail, since music is optional.
  }
}

const std::string& Assets::prev_graphics_style() { return styled_graphics_.prev_style(); }

const std::string& Assets::next_graphics_style() { return styled_graphics_.next_style(); }

bool Assets::is_weird() const { return is_weird_; }

StyledGraphics::Style Assets::graphics_style() const { return styled_graphics_.style(); }

const std::string& Assets::graphics_style_name() const { return styled_graphics_.style_name(); }

const Texture& Assets::ceiling_texture() const { return *styled_graphics_.graphics().ceiling_texture; }

const Texture& Assets::cell_texture() const { return *styled_graphics_.graphics().cell_texture; }

const Texture& Assets::dead_space_texture() const { return *styled_graphics_.graphics().dead_space_texture; }

const Texture& Assets::dead_space_ghost_texture() const {
  return *styled_graphics_.graphics().dead_space_ghost_texture;
}

const Texture& Assets::end_texture() const { return *styled_graphics_.graphics().end_texture; }

const Texture& Assets::end_wall_texture() const { return *styled_graphics_.graphics().end_wall_texture; }

const Texture& Assets::floor_texture() const { return *styled_graphics_.graphics().floor_texture; }

const Texture& Assets::fruit_texture() const { return *styled_graphics_.graphics().fruit_texture; }

const Texture& Assets::portal_texture() const { return *styled_graphics_.graphics().portal_texture; }

const Texture& Assets::robot_texture() const { return *styled_graphics_.graphics().robot_texture; }

const Texture& Assets::wall_texture() const { return *styled_graphics_.graphics().wall_texture; }

const Texture& Assets::wall_ghost_texture() const { return *styled_graphics_.graphics().wall_ghost_texture; }

const Texture& Assets::white_texture() const { return *styled_graphics_.graphics().white_texture; }

const Texture& Assets::white_ghost_texture() const {
  return *styled_graphics_.graphics().white_ghost_texture;
}

const Texture& Assets::star_texture() const { return *star_texture_; }

const Image& Assets::icon_image() const { return *icon_image_; }

const Sprite& Assets::logo_sprite() const { return *logo_sprite_; }

const Sprite& Assets::keys_sprite() const { return *keys_sprite_; }

const Sprite& Assets::dantares_sprite() const { return *dantares_sprite_; }

const Sprite& Assets::boring_work_sprite() const { return *boring_work_sprite_; }

const Sprite& Assets::goodnight_sprite() const { return *goodnight_sprite_; }

const Sprite& Assets::corngrits_sprite() const { return *corngrits_sprite_; }

FontRenderer& Assets::font_renderer() const { return *font_renderer_; }

const FontAtlas& Assets::font_atlas() const { return *font_atlas_; }

const Color4f& Assets::eko_color() const { return eko_color_; }

const Color4f& Assets::end_color() const { return end_color_; }

const Color4f& Assets::fruit_color() const { return fruit_color_; }

const Color4f& Assets::portal_color() const { return portal_color_; }

const Color4f& Assets::robot_color() const { return robot_color_; }

const Color4f& Assets::wall_color() const { return wall_color_; }

const Music* Assets::music() const { return music_.get(); }

} // Namespace.
