/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_ASSETS_H_
#define EKOSCAPE_ASSETS_ASSETS_H_

#include "common.h"

#include "cybel/asset/asset_loader.h"
#include "cybel/scene/scene_context.h"
#include "cybel/types/color.h"
#include "cybel/util/file_sys.h"

#include "assets/art_styles.h"
#include "assets/asset_ids.h"
#include "assets/font_renderer.h"
#include "map/map.h"

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace ekoscape {

class Assets final : public AssetLoader {
public:
  using OnMapFile = std::function<
    void(const std::string& group,const std::filesystem::path& map_file,Map& map)
  >;

  static inline const std::filesystem::path kAssetsSubDir{"assets"};
  static inline const std::filesystem::path kIconsSubDir{kAssetsSubDir / "icons"};
  static inline const std::filesystem::path kImagesSubDir{kAssetsSubDir / "images"};
  static inline const std::filesystem::path kMapsSubDir{kAssetsSubDir / "maps"};
  static inline const std::filesystem::path kMusicSubDir{kAssetsSubDir / "music"};
  static inline const std::filesystem::path kTexturesSubDir{kAssetsSubDir / "textures"};

  // For images that don't really work well with make_weird().
  // - The names mean "for mostly black images," etc.
  // - The black & white colors were chosen as throwbacks to the original code in `relics/` (v1.0).
  static inline const Color4f kWeirdBlackColor{0.01f,1.0f};
  static inline const Color4f kWeirdGrayColor = Color4f::kHotPink;
  static inline const Color4f kWeirdWhiteColor = Color4f::kWhite;

  explicit Assets(const FileSys& file_sys,bool is_audio_alive,std::string_view art_style);

  void make_weird(const SceneContext& ctx,bool is_weird);

  void glob_maps_meta(const OnMapFile& on_map) const;

  void load_cpu_gfx(AssetMan& assets,CpuGfxLoader& gfx) override;
  void load_gpu_gfx(AssetMan& assets,GpuGfxLoader& gfx) override;
  void load_audio(AssetMan& assets,AudioLoader& audio) override;

  void prev_art_style();
  void next_art_style();

  bool is_weird() const;

  const Color4f& eko_color() const;
  const Color4f& end_color() const;
  const Color4f& fruit_color() const;
  const Color4f& portal_color() const;
  const Color4f& robot_color() const;
  const Color4f& wall_color() const;

  const std::string& art_style() const;
  const StyledTextureIds& styled_texture_ids() const;

  FontAtlasId font_atlas_id() const;
  FontRenderer& font_renderer();

private:
  using LoadAssetFile = std::function<void(const std::filesystem::path& file)>;

  std::vector<std::filesystem::path> base_dirs_{};
  bool is_audio_alive_ = false;
  bool is_weird_ = false;

  Color4f eko_color_{}; // Cell & Player.
  Color4f end_color_{};
  Color4f fruit_color_{};
  Color4f portal_color_{};
  Color4f robot_color_{};
  Color4f wall_color_{};

  ArtStyles art_styles_;
  FontRenderer font_renderer_{font_atlas_id()};

  void init_base_dirs(const FileSys& file_sys);
  void update_colors();

  void load_image(CpuGfxLoader& gfx,ImageId id,const std::filesystem::path& sub_file);
  void load_texture(GpuGfxLoader& gfx,TextureId id,const std::filesystem::path& sub_file);
  void load_sprite(GpuGfxLoader& gfx,SpriteId id,const std::filesystem::path& sub_file,
                   const Color4f& weird_color = Color4f::kNone);
  void load_font_atlas(GpuGfxLoader& gfx,FontAtlasId id,const std::filesystem::path& sub_file,
                       const FontAtlas::Config& config);

  void load_music(AudioLoader& audio,MusicId id,const std::filesystem::path& sub_file);

  void load_asset(const std::filesystem::path& sub_file,bool fail_on_error,const LoadAssetFile& load_file);
};

} // namespace ekoscape
#endif
