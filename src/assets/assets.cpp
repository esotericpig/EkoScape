/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "assets.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

#include <cassert>
#include <optional>
#include <system_error>
#include <unordered_set>
#include <utility>

namespace ekoscape {

Assets::Assets(const FileSys& file_sys,bool is_audio_alive,std::string_view art_style)
  : is_audio_alive_{is_audio_alive},
    art_styles_{art_style} {
  init_base_dirs(file_sys);
  update_colors();
}

void Assets::init_base_dirs(const FileSys& file_sys) {
  base_dirs_ = {
    // First, try current dir, so that the user can easily overwrite the assets.
    ".",
    // Try our Linux AppImage's path.
    file_sys.app_image_dir(),
    // Try our game's base dir.
    // - On macOS, this is `Contents/Resources`.
    file_sys.base_dir(),
  };

  // Make all paths absolute for Util::unique().
  for(std::error_code ec{}; auto& dir : base_dirs_) {
    dir = canonical(dir,ec);
  }

  base_dirs_ = Util::unique(base_dirs_);
}

void Assets::update_colors() {
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
}

void Assets::make_weird(const SceneContext& ctx,bool is_weird) {
  if(is_weird == is_weird_) { return; }
  is_weird_ = is_weird;

  update_colors();
  ctx.assets.reload_gfx();
  font_renderer_.make_weird(is_weird);

  ctx.engine.set_icon(ctx.assets.image(ImageId::kEkoScapeIcon));
}

void Assets::glob_maps_meta(const OnMapFile& on_map) const {
  std::unordered_set<std::string> loaded_maps{};

  for(const auto& base_dir : base_dirs_) {
    const auto maps_dir = base_dir / kMapsSubDir;
    std::error_code ec{};

    if(!is_directory(maps_dir,ec)) { continue; }

    try {
      for(const auto& group_entry : std::filesystem::directory_iterator(maps_dir)) {
        if(!group_entry.is_directory(ec)) { continue; }

        const std::string group = group_entry.path().filename();

        for(const auto& map_entry : std::filesystem::directory_iterator(group_entry)) {
          const auto map_file = map_entry.path();
          const auto map_key = group + '/' + map_file.filename().string();

          if(loaded_maps.contains(map_key)) { continue; }
          if(!map_entry.is_regular_file(ec) || !Map::is_map_file(map_file)) { continue; }

          Map map{};

          try {
            map.load_file_meta(map_file);
          } catch(const CybelError& e) {
            std::cerr << "[WARN] " << e.what() << std::endl;
            continue;
          }

          on_map(group,map_file,map);
          loaded_maps.insert(map_key); // Success.
        }
      }
    } catch(const std::filesystem::filesystem_error& e) {
      std::cerr << "[WARN] Failed to crawl Maps folder `" << maps_dir << "`: " << e.what() << '.'
                << std::endl;
    }
  }
}

void Assets::load_cpu_gfx([[maybe_unused]] AssetMan& assets,CpuGfxLoader& gfx) {
  load_image(gfx,ImageId::kEkoScapeIcon,kIconsSubDir / "io.github.esotericpig.ekoscape.png");
}

void Assets::load_gpu_gfx(AssetMan& assets,GpuGfxLoader& gfx) {
  art_styles_.load(gfx,base_dirs_,is_weird_);

  // NOTE: Ensure both load successfully.
  load_texture(gfx,TextureId::kStar,kTexturesSubDir / "star.png");
  load_texture(gfx,TextureId::kStarWeird,kTexturesSubDir / "star2.png");

  if(is_weird_) {
    assets.swap_texture(TextureId::kStar,TextureId::kStarWeird);
  }

  load_sprite(gfx,SpriteId::kEkoScapeLogo,kImagesSubDir / "EkoScape.png",kWeirdGrayColor);
  load_sprite(gfx,SpriteId::kDantaresLogo,kImagesSubDir / "Dantares.png");
  load_sprite(gfx,SpriteId::kBoringWork,kImagesSubDir / "boring_work.png",kWeirdGrayColor);
  load_sprite(gfx,SpriteId::kGoodnight,kImagesSubDir / "goodnight.png");
  load_sprite(gfx,SpriteId::kCorngrits,kImagesSubDir / "corngrits.png",kWeirdGrayColor);

#if defined(__EMSCRIPTEN__)
  load_sprite(gfx,SpriteId::kKeys,kImagesSubDir / "keys_web.png",kWeirdGrayColor);
#else // Desktop
  load_sprite(gfx,SpriteId::kKeys,kImagesSubDir / "keys.png",kWeirdGrayColor);
#endif

  load_font_atlas(gfx,FontAtlasId::kMonogram,kImagesSubDir / "font_monogram.png",
    FontAtlas::Config{}
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
}

void Assets::load_audio([[maybe_unused]] AssetMan& assets,AudioLoader& audio) {
  load_music(audio,MusicId::kEkoScape,kMusicSubDir / "ekoscape.ogg");
}

void Assets::load_image(CpuGfxLoader& gfx,ImageId id,const std::filesystem::path& sub_file) {
  load_asset(sub_file,true,[&](const auto& file) {
    auto& image = gfx.load_image(id,file);

    if(is_weird_) {
      image.make_weird();
    }
  });
}

void Assets::load_texture(GpuGfxLoader& gfx,TextureId id,const std::filesystem::path& sub_file) {
  load_asset(sub_file,true,[&](const auto& file) {
    gfx.load_texture(id,Image{file});
  });
}

void Assets::load_sprite(GpuGfxLoader& gfx,SpriteId id,const std::filesystem::path& sub_file,
                         const Color4f& weird_color) {
  load_asset(sub_file,true,[&](const auto& file) {
    Image image{file};

    if(is_weird_) {
      if(weird_color == Color4f::kNone) {
        image.make_weird();
      } else {
        image.colorize(weird_color);
      }
    }

    gfx.load_sprite(id,gfx.load_texture(image));
  });
}

void Assets::load_font_atlas(GpuGfxLoader& gfx,FontAtlasId id,const std::filesystem::path& sub_file,
                             const FontAtlas::Config& config) {
  load_asset(sub_file,true,[&](const auto& file) {
    gfx.load_font_atlas(id,gfx.load_texture(Image{file}),config);
  });
}

void Assets::load_music(AudioLoader& audio,MusicId id,const std::filesystem::path& sub_file) {
  if(!is_audio_alive_) { return; }

  load_asset(sub_file,false,[&](const auto& file) {
    audio.load_music(id,file);
  });
}

void Assets::load_asset(const std::filesystem::path& sub_file,bool fail_on_error,
                        const LoadAssetFile& load_file) {
  std::optional<CybelError> first_error{};

  for(const auto& base_dir : base_dirs_) {
    try {
      load_file(base_dir / sub_file);
      return; // Success.
    } catch(const CybelError& e) {
      if(!first_error) { first_error.emplace(e); }
    }
  }

  assert(first_error);

  if(first_error) {
    if(fail_on_error) { throw *first_error; }

    std::cerr << "[WARN] " << first_error->what() << std::endl;
  }
}

void Assets::prev_art_style() {
  art_styles_.prev_style();
}

void Assets::next_art_style() {
  art_styles_.next_style();
}

bool Assets::is_weird() const { return is_weird_; }

const Color4f& Assets::eko_color() const { return eko_color_; }

const Color4f& Assets::end_color() const { return end_color_; }

const Color4f& Assets::fruit_color() const { return fruit_color_; }

const Color4f& Assets::portal_color() const { return portal_color_; }

const Color4f& Assets::robot_color() const { return robot_color_; }

const Color4f& Assets::wall_color() const { return wall_color_; }

const std::string& Assets::art_style() const { return art_styles_.style_name(); }

const StyledTextureIds& Assets::styled_texture_ids() const { return art_styles_.styled_texture_ids(); }

FontAtlasId Assets::font_atlas_id() const { return FontAtlasId::kMonogram; }

FontRenderer& Assets::font_renderer() { return font_renderer_; }

} // namespace ekoscape
