/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_ASSETS_H_
#define EKOSCAPE_ASSETS_ASSETS_H_

#include "common.h"

#include "cybel/asset/asset_man.h"
#include "cybel/types/color.h"

#include "assets/font_atlas_id.h"
#include "assets/font_renderer.h"
#include "assets/image_id.h"
#include "assets/music_id.h"
#include "assets/sprite_id.h"
#include "assets/styled_tex_id.h"
#include "assets/texture_id.h"
#include "map/map.h"

#include <filesystem>
#include <functional>
#include <vector>

namespace ekoscape {

class Assets final : AssetMan {
public:
  using MapCallback = std::function<
    void(const std::string& group,const std::filesystem::path& map_file,Map&)
  >;

  static inline const std::filesystem::path kAssetsSubdir{"assets"};
  static inline const std::filesystem::path kIconsSubdir{kAssetsSubdir / "icons"};
  static inline const std::filesystem::path kImagesSubdir{kAssetsSubdir / "images"};
  static inline const std::filesystem::path kMapsSubdir{kAssetsSubdir / "maps"};
  static inline const std::filesystem::path kMusicSubdir{kAssetsSubdir / "music"};
  static inline const std::filesystem::path kTexsSubdir{kAssetsSubdir / "textures"};

  explicit Assets(std::string_view tex_style,bool has_audio_player,bool make_weird = false);

  void on_context_restored();
  void reload_gfx();
  void reload_audio();
  void make_weird();

  void glob_maps_meta(const MapCallback& on_map) const;

  const std::string& prev_tex_style();
  const std::string& next_tex_style();

  bool is_weird() const;
  const std::string& tex_style() const;

  const Texture& star_tex() const;
  Texture* styled_tex(StyledTexId id);
  TextureRef styled_tex_ref(StyledTexId id);
  FontRenderer& font_renderer() const;

  Image* image(ImageId id);
  Texture* tex(TextureId id);
  Sprite* sprite(SpriteId id);
  SpriteRef sprite_ref(SpriteId id);
  FontAtlas& font_atlas();
  FontAtlas* font_atlas(FontAtlasId id);

  const Color4f& eko_color() const;
  const Color4f& end_color() const;
  const Color4f& fruit_color() const;
  const Color4f& portal_color() const;
  const Color4f& robot_color() const;
  const Color4f& wall_color() const;

  Music* music(MusicId id);

private:
  class StyledTextures final : public AssetMan {
  public:
    std::string name{};
    std::string dirname{};

    explicit StyledTextures(const std::filesystem::path& dir,bool make_weird);

    void check_texs();
    void zombify();

    Texture* tex(asset_id_t id) override;

  private:
    std::array<std::unique_ptr<Texture>,static_cast<std::size_t>(StyledTexId::kMax)> texs_{};

    void load_tex(StyledTexId id,const std::filesystem::path& file,bool make_weird,
                  const Color4f& weird_color = Color4f::kBlack);
  };

  using AssetLoader = std::function<void(const std::filesystem::path& base_dir)>;

  /**
   * NOTE: This should only ever be called once, since it uses SDL_GetBasePath(),
   *       which is an expensive operation.
   */
  static std::vector<std::filesystem::path> fetch_base_dirs();
  static inline const std::vector<std::filesystem::path> kBaseDirs = fetch_base_dirs();

  // For images that don't really work well with make_weird().
  // - The names mean "for mostly black images," etc.
  // - The black & white colors were chosen as throwbacks to the original code in relics (v1.0).
  static inline const Color4f kWeirdBlackColor{0.01f,1.0f};
  static inline const Color4f kWeirdGrayColor = Color4f::kHotPink;
  static inline const Color4f kWeirdWhiteColor{1.0f,1.0f};

  bool has_audio_player_ = false;
  bool is_weird_ = false;

  Texture* star_tex_ = nullptr;
  std::vector<StyledTextures> styled_texs_bag_{};
  std::vector<StyledTextures>::iterator styled_texs_bag_it_ = styled_texs_bag_.begin();
  std::unique_ptr<FontRenderer> font_renderer_{};

  std::array<std::unique_ptr<Image>,static_cast<std::size_t>(ImageId::kMax)> images_{};
  std::array<std::unique_ptr<Texture>,static_cast<std::size_t>(TextureId::kMax)> texs_{};
  std::array<std::unique_ptr<Sprite>,static_cast<std::size_t>(SpriteId::kMax)> sprites_{};
  std::array<std::unique_ptr<FontAtlas>,static_cast<std::size_t>(FontAtlasId::kMax)> font_atlases_{};

  Color4f eko_color_{}; // Cell & Player.
  Color4f end_color_{};
  Color4f fruit_color_{};
  Color4f portal_color_{};
  Color4f robot_color_{};
  Color4f wall_color_{};

  std::array<std::unique_ptr<Music>,static_cast<std::size_t>(MusicId::kMax)> music_bag_{};

  using AssetMan::tex_ref;
  using AssetMan::sprite_ref;

  void reload_gfx(bool make_weird);
  void reload_gfx(std::string_view tex_style,bool make_weird);
  void reload_styled_texs_bag(std::string_view tex_style);
  void check_gfx();

  void load_asset(const AssetLoader& load_from,bool fail_on_error = true) const;
  void load_image(ImageId id,const std::filesystem::path& subfile);
  void load_tex(TextureId id,const std::filesystem::path& subfile);
  void load_sprite(SpriteId id,const std::filesystem::path& subfile,
                   const Color4f& weird_color = Color4f::kBlack);
  void load_font_atlas(FontAtlasId id,const std::filesystem::path& subfile,FontAtlas::Builder& builder);
  void load_music(MusicId id,const std::filesystem::path& subfile);

  Image* image(asset_id_t id) override;
  Texture* tex(asset_id_t id) override;
  Sprite* sprite(asset_id_t id) override;
  FontAtlas* font_atlas(asset_id_t id) override;
  Music* music(asset_id_t id) override;
};

} // namespace ekoscape
#endif
