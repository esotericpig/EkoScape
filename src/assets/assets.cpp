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
  reload_audio();
}

void Assets::on_context_lost() {
  for(auto& st : styled_texs_bag_) { st.zombify(); }
  for(auto& tex : texs_) { tex->zombify(); }
  for(auto& sprite : sprites_) { sprite->zombify(); }
  for(auto& font : font_atlases_) { font->zombify(); }
}

void Assets::on_context_restored() {
  Util::clear_gl_errors();
  reload_gfx();
}

void Assets::reload_gfx() { reload_gfx(is_weird_); }

void Assets::reload_gfx(bool make_weird) { reload_gfx(styled_texs_bag_it_->dirname,make_weird); }

void Assets::reload_gfx(std::string_view tex_style,bool make_weird) {
  is_weird_ = make_weird;

  reload_styled_texs_bag(tex_style);

  load_image(ImageId::kEkoScapeIcon,kIconsSubdir / "io.github.esotericpig.ekoscape.png");

  load_tex(TextureId::kStar1,kTexsSubdir / "star.png");
  load_tex(TextureId::kStar2,kTexsSubdir / "star2.png");
  star_tex_ = is_weird_ ? tex(TextureId::kStar2) : tex(TextureId::kStar1);

  load_sprite(SpriteId::kEkoScapeLogo,kImagesSubdir / "EkoScape.png",kWeirdGrayColor);
  load_sprite(SpriteId::kDantaresLogo,kImagesSubdir / "Dantares.png");
  load_sprite(SpriteId::kBoringWork,kImagesSubdir / "boring_work.png",kWeirdGrayColor);
  load_sprite(SpriteId::kGoodnight,kImagesSubdir / "goodnight.png");
  load_sprite(SpriteId::kCorngrits,kImagesSubdir / "corngrits.png",kWeirdGrayColor);

#if defined(__EMSCRIPTEN__)
  load_sprite(SpriteId::kKeys,kImagesSubdir / "keys_web.png",kWeirdGrayColor);
#else
  load_sprite(SpriteId::kKeys,kImagesSubdir / "keys.png",kWeirdGrayColor);
#endif

  load_font_atlas(
    FontAtlasId::kMonogram,kImagesSubdir / "font_monogram.png",
    FontAtlas::Builder{}
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
  );
  font_renderer_ = std::make_unique<FontRenderer>(font_atlas(),is_weird_);

  eko_color_ = Color4f::kRed;
  end_color_ = Color4f::kCopper;
  fruit_color_ = Color4f::kHotPink;
  portal_color_ = Color4f::kCyan;
  robot_color_.set_bytes(214);
  wall_color_ = Color4f::kGreen;

  if(is_weird_) {
    std::swap(eko_color_.r,eko_color_.b);
    std::swap(end_color_.r,end_color_.b);
    std::swap(fruit_color_.r,fruit_color_.b);
    std::swap(portal_color_.r,portal_color_.b);
    robot_color_ = kWeirdGrayColor;
    std::swap(wall_color_.r,wall_color_.b);
  }

  check_gfx(); // Validate all graphics were loaded.
}

void Assets::reload_styled_texs_bag(std::string_view tex_style) {
  styled_texs_bag_.clear();
  styled_texs_bag_it_ = styled_texs_bag_.begin();

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

        styled_texs_bag_.emplace_back(style_dir,is_weird_);
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
  styled_texs_bag_it_ = styled_texs_bag_.begin();

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
  for(styled_texs_bag_it_ = styled_texs_bag_.begin(); styled_texs_bag_it_ < styled_texs_bag_.end();
      ++styled_texs_bag_it_) {
    if(utf8::StrUtil::casecmp_ascii(styled_texs_bag_it_->dirname,tex_style) == 0) { break; }
  }

  if(styled_texs_bag_it_ >= styled_texs_bag_.end()) {
    std::cerr << "[WARN] Failed to find/load graphics style [" << tex_style << "]." << std::endl;
    styled_texs_bag_it_ = styled_texs_bag_.begin();
  }
}

void Assets::check_gfx() {
  for(auto& st : styled_texs_bag_) { st.check_texs(); }

  for(std::size_t id = 0; id < images_.size(); ++id) {
    if(!images_[id]) { throw CybelError{"Image ID [",id,"] was not loaded."}; }
  }
  for(std::size_t id = 0; id < texs_.size(); ++id) {
    if(!texs_[id]) { throw CybelError{"Texture ID [",id,"] was not loaded."}; }
  }
  for(std::size_t id = 0; id < sprites_.size(); ++id) {
    if(!sprites_[id]) { throw CybelError{"Sprite ID [",id,"] was not loaded."}; }
  }
  for(std::size_t id = 0; id < font_atlases_.size(); ++id) {
    if(!font_atlases_[id]) { throw CybelError{"Font atlas ID [",id,"] was not loaded."}; }
  }
}

void Assets::reload_audio() {
  if(!has_audio_player_) { return; }

  load_music(MusicId::kEkoScape,kMusicSubdir / "ekoscape.ogg");
}

void Assets::load_asset(const AssetLoader& load_from,bool fail_on_error) const {
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
  if(fail_on_error) { throw CybelError{error}; }

  std::cerr << "[WARN] " << error << std::endl;
}

void Assets::load_image(ImageId id,const std::filesystem::path& subfile) {
  const auto i = static_cast<std::size_t>(id);

  if(i >= images_.size()) { throw CybelError{"Invalid image ID [",i,"] on load."}; }

  load_asset([&](const auto& base_dir) {
    images_[i] = std::make_unique<Image>(base_dir / subfile,is_weird_);
  });
}

void Assets::load_tex(TextureId id,const std::filesystem::path& subfile) {
  const auto i = static_cast<std::size_t>(id);

  if(i >= texs_.size()) { throw CybelError{"Invalid texture ID [",i,"] on load."}; }

  load_asset([&](const auto& base_dir) {
    texs_[i] = std::make_unique<Texture>(Image{base_dir / subfile,is_weird_});
  });
}

void Assets::load_sprite(SpriteId id,const std::filesystem::path& subfile,const Color4f& weird_color) {
  const auto i = static_cast<std::size_t>(id);

  if(i >= sprites_.size()) { throw CybelError{"Invalid sprite ID [",i,"] on load."}; }

  load_asset([&](const auto& base_dir) {
    sprites_[i] = std::make_unique<Sprite>(Texture{Image{base_dir / subfile,is_weird_,weird_color}});
  });
}

void Assets::load_font_atlas(FontAtlasId id,const std::filesystem::path& subfile,
                             FontAtlas::Builder& builder) {
  const auto i = static_cast<std::size_t>(id);

  if(i >= font_atlases_.size()) { throw CybelError{"Invalid font atlas ID [",i,"] on load."}; }

  load_asset([&](const auto& base_dir) {
    builder.tex(Texture{Image{base_dir / subfile}});
    font_atlases_[i] = std::make_unique<FontAtlas>(builder.build());
  });
}

void Assets::load_music(MusicId id,const std::filesystem::path& subfile) {
  const auto i = static_cast<std::size_t>(id);

  if(i >= music_bag_.size()) { throw CybelError{"Invalid music ID [",i,"] on load."}; }

  load_asset([&](const auto& base_dir) {
    music_bag_[i] = std::make_unique<Music>(base_dir / subfile);
  },false);
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
  if(styled_texs_bag_it_ <= styled_texs_bag_.begin()) {
    styled_texs_bag_it_ = styled_texs_bag_.end(); // Wrap to end.
  }
  --styled_texs_bag_it_;

  return styled_texs_bag_it_->name;
}

const std::string& Assets::next_tex_style() {
  ++styled_texs_bag_it_;
  if(styled_texs_bag_it_ >= styled_texs_bag_.end()) {
    styled_texs_bag_it_ = styled_texs_bag_.begin(); // Wrap to beginning.
  }

  return styled_texs_bag_it_->name;
}

bool Assets::is_weird() const { return is_weird_; }

const std::string& Assets::tex_style() const { return styled_texs_bag_it_->name; }

const Texture& Assets::star_tex() const { return *star_tex_; }

Texture* Assets::styled_tex(StyledTexId id) {
  return styled_texs_bag_it_->tex(static_cast<asset_id_t>(id));
}

TextureRef Assets::styled_tex_ref(StyledTexId id) {
  return styled_texs_bag_it_->tex_ref(static_cast<asset_id_t>(id));
}

FontRenderer& Assets::font_renderer() const { return *font_renderer_; }

Image* Assets::image(ImageId id) { return image(static_cast<asset_id_t>(id)); }

Image* Assets::image(asset_id_t id) {
  if(id >= images_.size()) { throw CybelError{"Invalid image ID [",id,"] on get."}; }

  return images_[id].get();
}

Texture* Assets::tex(TextureId id) { return tex(static_cast<asset_id_t>(id)); }

Texture* Assets::tex(asset_id_t id) {
  if(id >= texs_.size()) { throw CybelError{"Invalid texture ID [",id,"] on get."}; }

  return texs_[id].get();
}

Sprite* Assets::sprite(SpriteId id) { return sprite(static_cast<asset_id_t>(id)); }

Sprite* Assets::sprite(asset_id_t id) {
  if(id >= sprites_.size()) { throw CybelError{"Invalid sprite ID [",id,"] on get."}; }

  return sprites_[id].get();
}

SpriteRef Assets::sprite_ref(SpriteId id) { return sprite_ref(static_cast<asset_id_t>(id)); }

FontAtlas& Assets::font_atlas() { return *font_atlas(kDefaultFontAtlasId); }

FontAtlas* Assets::font_atlas(FontAtlasId id) { return font_atlas(static_cast<asset_id_t>(id)); }

FontAtlas* Assets::font_atlas(asset_id_t id) {
  if(id >= font_atlases_.size()) { throw CybelError{"Invalid font atlas ID [",id,"] on get."}; }

  return font_atlases_[id].get();
}

FontAtlasRef Assets::font_atlas_ref() { return font_atlas_ref(kDefaultFontAtlasId); }

FontAtlasRef Assets::font_atlas_ref(FontAtlasId id) { return font_atlas_ref(static_cast<asset_id_t>(id)); }

const Color4f& Assets::eko_color() const { return eko_color_; }

const Color4f& Assets::end_color() const { return end_color_; }

const Color4f& Assets::fruit_color() const { return fruit_color_; }

const Color4f& Assets::portal_color() const { return portal_color_; }

const Color4f& Assets::robot_color() const { return robot_color_; }

const Color4f& Assets::wall_color() const { return wall_color_; }

Music* Assets::music(MusicId id) { return music(static_cast<asset_id_t>(id)); }

Music* Assets::music(asset_id_t id) {
  if(id >= music_bag_.size()) { throw CybelError{"Invalid music ID [",id,"] on get."}; }

  return music_bag_[id].get();
}

Assets::StyledTextures::StyledTextures(const std::filesystem::path& dir,bool make_weird)
  : dirname(dir.filename().string()),
    name(utf8::StrUtil::ellipsize(dirname,18)) {
  load_tex(StyledTexId::kCeiling,dir / "ceiling.png",make_weird);
  load_tex(StyledTexId::kCell,dir / "cell.png",make_weird);
  load_tex(StyledTexId::kDeadSpace,dir / "dead_space.png",make_weird,kWeirdBlackColor);
  load_tex(StyledTexId::kDeadSpaceGhost,dir / "dead_space_ghost.png",make_weird,kWeirdBlackColor);
  load_tex(StyledTexId::kEnd,dir / "end.png",make_weird);
  load_tex(StyledTexId::kEndWall,dir / "end_wall.png",make_weird);
  load_tex(StyledTexId::kFloor,dir / "floor.png",make_weird);
  load_tex(StyledTexId::kFruit,dir / "fruit.png",make_weird);
  load_tex(StyledTexId::kPortal,dir / "portal.png",make_weird);
  load_tex(StyledTexId::kRobot,dir / "robot.png",make_weird,kWeirdGrayColor);
  load_tex(StyledTexId::kWall,dir / "wall.png",make_weird);
  load_tex(StyledTexId::kWallGhost,dir / "wall_ghost.png",make_weird);
  load_tex(StyledTexId::kWhite,dir / "white.png",make_weird,kWeirdWhiteColor);
  load_tex(StyledTexId::kWhiteGhost,dir / "white_ghost.png",make_weird,kWeirdWhiteColor);
}

void Assets::StyledTextures::load_tex(StyledTexId id,const std::filesystem::path& file,bool make_weird,
                                      const Color4f& weird_color) {
  const auto i = static_cast<std::size_t>(id);

  if(i >= texs_.size()) { throw CybelError{"Invalid styled texture ID [",i,"] on load."}; }

  texs_[i] = std::make_unique<Texture>(Image{file,make_weird,weird_color});
}

void Assets::StyledTextures::check_texs() {
  for(std::size_t id = 0; id < texs_.size(); ++id) {
    if(!texs_[id]) { throw CybelError{"Styled texture ID [",id,"] was not loaded."}; }
  }
}

void Assets::StyledTextures::zombify() {
  for(auto& tex : texs_) { tex->zombify(); }
}

Texture* Assets::StyledTextures::tex(asset_id_t id) {
  if(id >= texs_.size()) { throw CybelError{"Invalid styled texture ID [",id,"] on get."}; }

  return texs_[id].get();
}

} // namespace ekoscape
