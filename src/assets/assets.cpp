/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "assets.h"

namespace ekoscape {

const std::filesystem::path Assets::kAssetsDir{fetch_base_path() / "assets"};
const std::filesystem::path Assets::kImagesDir{kAssetsDir / "images"};
const std::filesystem::path Assets::kMapsDir{kAssetsDir / "maps"};
const std::filesystem::path Assets::kTexturesDir{kAssetsDir / "textures"};

Assets::Assets(StyledGraphics::Style graphics_style,bool has_music_player,bool make_weird)
    : styled_graphics_(kTexturesDir,graphics_style,make_weird),has_music_player_(has_music_player) {
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

  logo_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "ekoscape.png"},is_weird_});
  keys_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "keys.png"},is_weird_});
  dantares_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "dantares.png"},is_weird_});
  boring_work_sprite_ = std::make_unique<Sprite>(Texture{Image{kImagesDir / "boring_work.png"},is_weird_});

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
}

void Assets::reload_music() {
  if(!has_music_player_) { return; }

  try {
    //music_ = std::make_unique<Music>(kAssetsDir / "music/matrix.mid");
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

const Texture& Assets::end_texture() const { return *styled_graphics_.graphics().end_texture; }

const Texture& Assets::floor_texture() const { return *styled_graphics_.graphics().floor_texture; }

const Texture& Assets::robot_texture() const { return *styled_graphics_.graphics().robot_texture; }

const Texture& Assets::wall_texture() const { return *styled_graphics_.graphics().wall_texture; }

const Texture& Assets::white_texture() const { return *styled_graphics_.graphics().white_texture; }

const Texture& Assets::star_texture() const { return *star_texture_; }

const Sprite& Assets::logo_sprite() const { return *logo_sprite_; }

const Sprite& Assets::keys_sprite() const { return *keys_sprite_; }

const Sprite& Assets::dantares_sprite() const { return *dantares_sprite_; }

const Sprite& Assets::boring_work_sprite() const { return *boring_work_sprite_; }

FontRenderer& Assets::font_renderer() const { return *font_renderer_; }

const FontAtlas& Assets::font_atlas() const { return *font_atlas_; }

const Music* Assets::music() const { return music_.get(); }

std::filesystem::path Assets::fetch_base_path() {
  char* cpath = SDL_GetBasePath();

  if(cpath == NULL) {
    throw CybelError{"Failed to get base path of app: " + Util::get_sdl_error() + "."};
  }

  std::filesystem::path path{cpath};

  SDL_free(cpath);
  cpath = NULL;

  return path;
}

} // Namespace.
