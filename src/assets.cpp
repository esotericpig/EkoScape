/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "assets.h"

namespace ekoscape {

const std::filesystem::path Assets::kAssetsDir = "assets";

Assets::Assets(StyledGraphics::Style graphics_style,bool has_music_player)
    : styled_graphics_(kAssetsDir,graphics_style),has_music_player_(has_music_player) {
  reload_graphics();
  reload_music();
}

void Assets::reload_graphics() {
  styled_graphics_.reload();
  reload_images();
}

void Assets::reload_images() {
  const std::filesystem::path img_dir = kAssetsDir / "images";

  logo_sprite_ = std::make_unique<Sprite>(Texture{Image{img_dir / "ekoscape.png"}});
  font_atlas_ = std::make_unique<FontAtlas>(
    FontAtlas::Builder{Texture{Image{img_dir / "font_monogram.png"}}}
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
  keys_sprite_ = std::make_unique<Sprite>(Texture{Image{img_dir / "keys.png"}});
  boring_work_sprite_ = std::make_unique<Sprite>(Texture{Image{img_dir / "boring_work.png"}});
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

StyledGraphics::Style Assets::graphics_style() const { return styled_graphics_.style(); }

const std::string& Assets::graphics_style_name() const { return styled_graphics_.style_name(); }

const Texture& Assets::ceiling_texture() const { return *styled_graphics_.graphics().ceiling_texture; }

const Texture& Assets::cell_texture() const { return *styled_graphics_.graphics().cell_texture; }

const Texture& Assets::end_texture() const { return *styled_graphics_.graphics().end_texture; }

const Texture& Assets::floor_texture() const { return *styled_graphics_.graphics().floor_texture; }

const Texture& Assets::robot_texture() const { return *styled_graphics_.graphics().robot_texture; }

const Texture& Assets::wall_texture() const { return *styled_graphics_.graphics().wall_texture; }

const Texture& Assets::white_texture() const { return *styled_graphics_.graphics().white_texture; }

const Sprite& Assets::logo_sprite() const { return *logo_sprite_; }

const FontAtlas& Assets::font_atlas() const { return *font_atlas_; }

const Sprite& Assets::keys_sprite() const { return *keys_sprite_; }

const Sprite& Assets::boring_work_sprite() const { return *boring_work_sprite_; }

const Music* Assets::music() const { return music_.get(); }

} // Namespace.
