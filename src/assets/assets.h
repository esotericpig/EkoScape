/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_ASSETS_H_
#define EKOSCAPE_ASSETS_ASSETS_H_

#include "common.h"

#include "cybel/audio/music.h"
#include "cybel/gfx/font_atlas.h"
#include "cybel/gfx/image.h"
#include "cybel/gfx/sprite.h"
#include "cybel/gfx/texture.h"
#include "cybel/types/color.h"

#include "assets/font_renderer.h"
#include "map/map.h"

#include <filesystem>
#include <functional>
#include <vector>

namespace ekoscape {

class Assets final {
public:
  using MapCallback = std::function<void(const std::string& group,const std::filesystem::path& map_file,
                                         Map&)>;

  static inline const std::filesystem::path kAssetsSubdir{"assets"};
  static inline const std::filesystem::path kIconsSubdir{kAssetsSubdir / "icons"};
  static inline const std::filesystem::path kImgsSubdir{kAssetsSubdir / "images"};
  static inline const std::filesystem::path kMapsSubdir{kAssetsSubdir / "maps"};
  static inline const std::filesystem::path kMusicSubdir{kAssetsSubdir / "music"};
  static inline const std::filesystem::path kTexsSubdir{kAssetsSubdir / "textures"};

  explicit Assets(std::string_view tex_style,bool has_audio_player,bool make_weird = false);

  void on_context_restored();
  void reload_gfx();
  void reload_music();
  void make_weird();

  void glob_maps_meta(const MapCallback& on_map) const;

  const std::string& prev_tex_style();
  const std::string& next_tex_style();

  bool is_weird() const;
  const std::string& tex_style() const;

  const Texture& ceiling_tex() const;
  const Texture& cell_tex() const;
  const Texture& dead_space_tex() const;
  const Texture& dead_space_ghost_tex() const;
  const Texture& end_tex() const;
  const Texture& end_wall_tex() const;
  const Texture& floor_tex() const;
  const Texture& fruit_tex() const;
  const Texture& portal_tex() const;
  const Texture& robot_tex() const;
  const Texture& wall_tex() const;
  const Texture& wall_ghost_tex() const;
  const Texture& white_tex() const;
  const Texture& white_ghost_tex() const;
  const Texture& star_tex() const;

  const Image& icon_img() const;
  const Sprite& logo_sprite() const;
  const Sprite& keys_sprite() const;
  const Sprite& dantares_sprite() const;
  const Sprite& boring_work_sprite() const;
  const Sprite& goodnight_sprite() const;
  const Sprite& corngrits_sprite() const;

  FontRenderer& font_renderer() const;
  const FontAtlas& font_atlas() const;

  const Color4f& eko_color() const;
  const Color4f& end_color() const;
  const Color4f& fruit_color() const;
  const Color4f& portal_color() const;
  const Color4f& robot_color() const;
  const Color4f& wall_color() const;

  const Music* music() const;

private:
  struct StyledTextures {
    std::string name{};
    std::string dirname{};

    std::unique_ptr<Texture> ceiling_tex{};
    std::unique_ptr<Texture> cell_tex{};
    std::unique_ptr<Texture> dead_space_tex{};
    std::unique_ptr<Texture> dead_space_ghost_tex{};
    std::unique_ptr<Texture> end_tex{};
    std::unique_ptr<Texture> end_wall_tex{};
    std::unique_ptr<Texture> floor_tex{};
    std::unique_ptr<Texture> fruit_tex{};
    std::unique_ptr<Texture> portal_tex{};
    std::unique_ptr<Texture> robot_tex{};
    std::unique_ptr<Texture> wall_tex{};
    std::unique_ptr<Texture> wall_ghost_tex{};
    std::unique_ptr<Texture> white_tex{};
    std::unique_ptr<Texture> white_ghost_tex{};
  };

  using AssetLoader = std::function<void(const std::filesystem::path& base_dir)>;

  /**
   * NOTE: This should only ever be called once, since it uses SDL_GetBasePath(),
   *       which is an expensive operation.
   */
  static std::vector<std::filesystem::path> fetch_base_dirs();
  static inline const std::vector<std::filesystem::path> kBaseDirs = fetch_base_dirs();

  bool has_audio_player_ = false;
  bool is_weird_ = false;

  std::vector<StyledTextures> styled_texs_bag_{};
  std::vector<StyledTextures>::const_iterator styled_texs_bag_it_ = styled_texs_bag_.cbegin();

  Texture* star_tex_ = nullptr;
  std::unique_ptr<Texture> star1_tex_{};
  std::unique_ptr<Texture> star2_tex_{};

  std::unique_ptr<Image> icon_img_{};
  std::unique_ptr<Sprite> logo_sprite_{};
  std::unique_ptr<Sprite> keys_sprite_{};
  std::unique_ptr<Sprite> dantares_sprite_{};
  std::unique_ptr<Sprite> boring_work_sprite_{};
  std::unique_ptr<Sprite> goodnight_sprite_{};
  std::unique_ptr<Sprite> corngrits_sprite_{};

  std::unique_ptr<FontAtlas> font_atlas_{};
  std::unique_ptr<FontRenderer> font_renderer_{};

  Color4f eko_color_{}; // Cell & Player.
  Color4f end_color_{};
  Color4f fruit_color_{};
  Color4f portal_color_{};
  Color4f robot_color_{};
  Color4f wall_color_{};

  std::unique_ptr<Music> music_{};

  void reload_gfx(bool make_weird);
  void reload_gfx(std::string_view tex_style,bool make_weird);
  void reload_styled_texs_bag(std::string_view tex_style);
  StyledTextures load_styled_texs(const std::filesystem::path& dir) const;

  void load_asset(const AssetLoader& load_from) const;
  std::unique_ptr<Image> load_img(const std::filesystem::path& subfile) const;
  std::unique_ptr<Sprite> load_sprite(const std::filesystem::path& subfile) const;
  std::unique_ptr<Texture> load_tex(const std::filesystem::path& subfile) const;
};

} // namespace ekoscape
#endif
