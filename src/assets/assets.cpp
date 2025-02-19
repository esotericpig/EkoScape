/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "assets.h"

#include "cybel/str/utf8/str_util.h"
#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace ekoscape {

std::vector<std::filesystem::path> Assets::fetch_base_dirs() {
  std::vector<std::filesystem::path> dirs{};
  std::filesystem::path dir{};
  std::error_code err_code{};

  // First, try current dir, so that the user can easily overwrite the assets.
  dirs.emplace_back(".");

  // Try our Linux AppImage's path.
  const char* appimg_path = std::getenv("APPIMAGE");

  if(appimg_path != nullptr) {
    // parent_path() can throw exceptions.
    try {
      dir = appimg_path;
      dir = dir.parent_path();

      if(is_directory(dir,err_code)) {
        dirs.push_back(dir);
      } else {
        std::cerr << "[WARN] AppImage path [" << dir << ',' << appimg_path << "] isn't a folder."
                  << std::endl;
      }
    } catch(const std::exception& e) {
      std::cerr << "[WARN] Failed to get parent path of AppImage path [" << appimg_path << "]: "
                << e.what() << '.' << std::endl;
    }
  }

  // Try our game's base dir.
  // - NOTE: SDL_GetBasePath() is an expensive operation so should only be called once.
  // - On macOS, this is `Contents/Resources`.
  char* base_path = SDL_GetBasePath();

  if(base_path != NULL) {
    dir = base_path;

    SDL_free(base_path);
    base_path = NULL;

    if(is_directory(dir,err_code)) {
      dirs.push_back(dir);
    } else {
      std::cerr << "[WARN] Base path of game [" << dir << "] isn't a folder." << std::endl;
    }
  } else {
    std::cerr << "[WARN] Failed to get base path of game: " + Util::get_sdl_error() + '.' << std::endl;
  }

  // Make all paths absolute for Util::unique().
  for(auto& d : dirs) { d = canonical(d,err_code); }

  if(dirs.empty()) {
    std::cerr << "[ERROR] All base dirs failed for some reason." << std::endl;
    dirs.emplace_back(".");

    return dirs;
  }

  return (dirs.size() == 1) ? dirs : Util::unique(dirs);
}

Assets::Assets(std::string_view tex_style,bool has_audio_player,bool make_weird)
  : has_audio_player_(has_audio_player) {
  reload_gfx(tex_style,make_weird);
  reload_music();
}

void Assets::reload_gfx() { reload_gfx(is_weird_); }

void Assets::reload_gfx(bool make_weird) { reload_gfx(styled_texs_bag_it_->dirname,make_weird); }

void Assets::reload_gfx(std::string_view tex_style,bool make_weird) {
  is_weird_ = make_weird;

  reload_styled_texs_bag(tex_style);

  star1_tex_ = load_tex(kTexsSubdir / "star.png");
  star2_tex_ = load_tex(kTexsSubdir / "star2.png");
  star_tex_ = is_weird_ ? star2_tex_.get() : star1_tex_.get();

  icon_img_ = load_img(kIconsSubdir / "io.github.esotericpig.ekoscape.png");
  logo_sprite_ = load_sprite(kImgsSubdir / "EkoScape.png");
  keys_sprite_ = load_sprite(kImgsSubdir / "keys.png");
  dantares_sprite_ = load_sprite(kImgsSubdir / "Dantares.png");
  boring_work_sprite_ = load_sprite(kImgsSubdir / "boring_work.png");
  goodnight_sprite_ = load_sprite(kImgsSubdir / "goodnight.png");
  corngrits_sprite_ = load_sprite(kImgsSubdir / "corngrits.png");

  load_asset([&](const auto& base_dir) {
    font_atlas_ = std::make_unique<FontAtlas>(
      FontAtlas::Builder{Texture{Image{base_dir / kImgsSubdir / "font_monogram.png"}}}
        .offset(0,0)
        .cell_size(9,14)
        .cell_padding(2)
        .spacing(5,5)
        .default_rune(U'¿')
        .index_to_rune({
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
  });
  font_renderer_ = std::make_unique<FontRenderer>(*font_atlas_,is_weird_);

  eko_color_.set_hex(0xff0000);
  end_color_.set_hex(0xb87333); // Copper.
  fruit_color_.set_hex(0xff69b4); // Hot Pink.
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

void Assets::reload_styled_texs_bag(std::string_view tex_style) {
  styled_texs_bag_.clear();
  styled_texs_bag_it_ = styled_texs_bag_.cbegin();

  std::unordered_set<std::filesystem::path> loaded_dirnames{};
  std::ostringstream errors{};
  std::error_code err_code{};

  for(const auto& base_dir : kBaseDirs) {
    const auto texs_dir = base_dir / kTexsSubdir;

    if(!is_directory(texs_dir,err_code)) { continue; } // ADL (Argument-Dependent Lookup).

    try {
      for(const auto& style_entry : std::filesystem::directory_iterator(texs_dir)) {
        if(!style_entry.is_directory()) { continue; }

        const auto style_dir = style_entry.path();
        const auto style_dirname = style_dir.filename();

        if(loaded_dirnames.contains(style_dirname)) { continue; }

        styled_texs_bag_.push_back(load_styled_texs(style_dir));
        loaded_dirnames.insert(style_dirname);
      }
    } catch(const CybelError& e) {
      std::cerr << "[WARN] " << e.what() << std::endl;
      errors << "\n\n- " << e.what();
    } catch(const std::filesystem::filesystem_error& e) {
      std::string msg = "Failed to crawl textures folder [" + texs_dir.string() + "]: " + e.what() + '.';
      std::cerr << "[WARN] " << msg << std::endl;
      errors << "\n\n- " << msg;
    }
  }

  styled_texs_bag_.shrink_to_fit();
  styled_texs_bag_it_ = styled_texs_bag_.cbegin();

  if(styled_texs_bag_.empty()) {
    throw CybelError{
      "Failed to find/load any graphics styles in textures folder [",kTexsSubdir.string(),"].",
      errors.str()
    };
  }

  // Sort the styles alphabetically, ignoring case.
  std::ranges::sort(styled_texs_bag_,[](const auto& style1,const auto& style2) {
    return utf8::StrUtil::casecmp_ascii(style1.dirname,style2.dirname) < 0;
  });

  // Auto-select the style that matches `tex_style`, ignoring case.
  for(styled_texs_bag_it_ = styled_texs_bag_.cbegin(); styled_texs_bag_it_ < styled_texs_bag_.cend();
      ++styled_texs_bag_it_) {
    if(utf8::StrUtil::casecmp_ascii(styled_texs_bag_it_->dirname,tex_style) == 0) { break; }
  }

  if(styled_texs_bag_it_ >= styled_texs_bag_.cend()) {
    std::cerr << "[WARN] Failed to find/load graphics style [" << tex_style << "]." << std::endl;
    styled_texs_bag_it_ = styled_texs_bag_.cbegin();
  }
}

Assets::StyledTextures Assets::load_styled_texs(const std::filesystem::path& dir) const {
  StyledTextures st{};

  st.dirname = dir.filename().string();
  st.name = utf8::StrUtil::ellipsize(st.dirname,18);

  st.ceiling_tex = std::make_unique<Texture>(Image{dir / "ceiling.png"},is_weird_);
  st.cell_tex = std::make_unique<Texture>(Image{dir / "cell.png"},is_weird_);
  st.dead_space_tex = std::make_unique<Texture>(Image{dir / "dead_space.png"},is_weird_);
  st.dead_space_ghost_tex = std::make_unique<Texture>(Image{dir / "dead_space_ghost.png"},is_weird_);
  st.end_tex = std::make_unique<Texture>(Image{dir / "end.png"},is_weird_);
  st.end_wall_tex = std::make_unique<Texture>(Image{dir / "end_wall.png"},is_weird_);
  st.floor_tex = std::make_unique<Texture>(Image{dir / "floor.png"},is_weird_);
  st.fruit_tex = std::make_unique<Texture>(Image{dir / "fruit.png"},is_weird_);
  st.portal_tex = std::make_unique<Texture>(Image{dir / "portal.png"},is_weird_);
  st.robot_tex = std::make_unique<Texture>(Image{dir / "robot.png"},is_weird_);
  st.wall_tex = std::make_unique<Texture>(Image{dir / "wall.png"},is_weird_);
  st.wall_ghost_tex = std::make_unique<Texture>(Image{dir / "wall_ghost.png"},is_weird_);
  st.white_tex = std::make_unique<Texture>(Image{dir / "white.png"},is_weird_);
  st.white_ghost_tex = std::make_unique<Texture>(Image{dir / "white_ghost.png"},is_weird_);

  return st; // NRVO (Named Return Value Optimization).
}

void Assets::reload_music() {
  if(!has_audio_player_) { return; }

  try {
    load_asset([&](const auto& base_dir) {
      music_ = std::make_unique<Music>(base_dir / kMusicSubdir / "ekoscape.ogg");
    });
  } catch(const CybelError& e) {
    std::cerr << "[WARN] " << e.what() << std::endl;
    // Don't fail, since music is optional.
  }
}

void Assets::load_asset(const AssetLoader& load_from) const {
  std::string error{};

  for(const auto& base_dir : kBaseDirs) {
    try {
      load_from(base_dir);
      return; // Success.
    } catch(const CybelError& e) {
      if(error.empty()) { error = e.what(); }
    }
  }

  if(error.empty()) {
    error = "Failed to find/load assets in assets folder [" + kAssetsSubdir.string() + "].";
  }
  throw CybelError{error};
}

std::unique_ptr<Image> Assets::load_img(const std::filesystem::path& subfile) const {
  std::unique_ptr<Image> img{};

  load_asset([&](const auto& base_dir) {
    img = std::make_unique<Image>(base_dir / subfile);
  });

  return img;
}

std::unique_ptr<Sprite> Assets::load_sprite(const std::filesystem::path& subfile) const {
  std::unique_ptr<Sprite> sprite{};

  load_asset([&](const auto& base_dir) {
    sprite = std::make_unique<Sprite>(Texture{Image{base_dir / subfile},is_weird_});
  });

  return sprite;
}

std::unique_ptr<Texture> Assets::load_tex(const std::filesystem::path& subfile) const {
  std::unique_ptr<Texture> tex{};

  load_asset([&](const auto& base_dir) {
    tex = std::make_unique<Texture>(Image{base_dir / subfile},is_weird_);
  });

  return tex;
}

void Assets::make_weird() {
  if(!is_weird_) { reload_gfx(true); }
}

void Assets::glob_maps_meta(const MapCallback& on_map) const {
  std::unordered_set<std::string> loaded_maps{};

  for(const auto& base_dir : kBaseDirs) {
    const auto maps_dir = base_dir / kMapsSubdir;
    std::error_code err_code{}; // For noexcept overload.

    if(!is_directory(maps_dir,err_code)) { continue; }

    try {
      for(const auto& group_entry : std::filesystem::directory_iterator(maps_dir)) {
        if(!group_entry.is_directory()) { continue; }

        const std::string group = group_entry.path().filename().string();

        for(const auto& map_entry : std::filesystem::directory_iterator(group_entry)) {
          const auto map_file = map_entry.path();
          const auto map_key = group + '/' + map_file.filename().string();

          if(loaded_maps.contains(map_key)) { continue; }
          if(!map_entry.is_regular_file() || !Map::is_map_file(map_file)) { continue; }

          Map map{};

          try {
            map.load_file_meta(map_file);
          } catch(const CybelError& e) {
            std::cerr << "[WARN] " << e.what() << std::endl;
            continue;
          }

          on_map(group,map_file,map);
          loaded_maps.insert(map_key);
        }
      }
    } catch(const std::filesystem::filesystem_error& e) {
      std::cerr << "[WARN] Failed to crawl maps folder [" << maps_dir.string() << "]: " << e.what() << '.'
                << std::endl;
    }
  }
}

const std::string& Assets::prev_tex_style() {
  if(styled_texs_bag_it_ <= styled_texs_bag_.cbegin()) {
    styled_texs_bag_it_ = styled_texs_bag_.cend(); // Wrap to end.
  }
  --styled_texs_bag_it_;

  return styled_texs_bag_it_->name;
}

const std::string& Assets::next_tex_style() {
  ++styled_texs_bag_it_;
  if(styled_texs_bag_it_ >= styled_texs_bag_.cend()) {
    styled_texs_bag_it_ = styled_texs_bag_.cbegin(); // Wrap to beginning.
  }

  return styled_texs_bag_it_->name;
}

bool Assets::is_weird() const { return is_weird_; }

const std::string& Assets::tex_style() const { return styled_texs_bag_it_->name; }

const Texture& Assets::ceiling_tex() const { return *styled_texs_bag_it_->ceiling_tex; }

const Texture& Assets::cell_tex() const { return *styled_texs_bag_it_->cell_tex; }

const Texture& Assets::dead_space_tex() const { return *styled_texs_bag_it_->dead_space_tex; }

const Texture& Assets::dead_space_ghost_tex() const { return *styled_texs_bag_it_->dead_space_ghost_tex; }

const Texture& Assets::end_tex() const { return *styled_texs_bag_it_->end_tex; }

const Texture& Assets::end_wall_tex() const { return *styled_texs_bag_it_->end_wall_tex; }

const Texture& Assets::floor_tex() const { return *styled_texs_bag_it_->floor_tex; }

const Texture& Assets::fruit_tex() const { return *styled_texs_bag_it_->fruit_tex; }

const Texture& Assets::portal_tex() const { return *styled_texs_bag_it_->portal_tex; }

const Texture& Assets::robot_tex() const { return *styled_texs_bag_it_->robot_tex; }

const Texture& Assets::wall_tex() const { return *styled_texs_bag_it_->wall_tex; }

const Texture& Assets::wall_ghost_tex() const { return *styled_texs_bag_it_->wall_ghost_tex; }

const Texture& Assets::white_tex() const { return *styled_texs_bag_it_->white_tex; }

const Texture& Assets::white_ghost_tex() const { return *styled_texs_bag_it_->white_ghost_tex; }

const Texture& Assets::star_tex() const { return *star_tex_; }

const Image& Assets::icon_img() const { return *icon_img_; }

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
