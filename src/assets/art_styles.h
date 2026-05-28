/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_ASSETS_ART_STYLES_H_
#define EKOSCAPE_ASSETS_ART_STYLES_H_

#include "common.h"

#include "cybel/asset/asset_loaders.h"
#include "cybel/asset/asset_types.h"
#include "cybel/types/color.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace ekoscape {

struct StyledTextureIds final {
  asset_id_t ceiling{};
  asset_id_t cell{};
  asset_id_t dead_space{};
  asset_id_t dead_space_ghost{};
  asset_id_t end{};
  asset_id_t end_wall{};
  asset_id_t floor{};
  asset_id_t fruit{};
  asset_id_t portal{};
  asset_id_t robot{};
  asset_id_t wall{};
  asset_id_t wall_ghost{};
  asset_id_t white{};
  asset_id_t white_ghost{};
};

class ArtStyles final {
public:
  explicit ArtStyles(std::string_view style);

  void load(GpuGfxLoader& gfx,const std::vector<std::filesystem::path>& base_dirs,bool is_weird);

  void prev_style();
  void next_style();

  const std::string& style_name() const;
  const StyledTextureIds& styled_texture_ids() const;

private:
  enum class StyledTextureId : asset_id_t {
    kCeiling,
    kCell,
    kDeadSpace,
    kDeadSpaceGhost,
    kEnd,
    kEndWall,
    kFloor,
    kFruit,
    kPortal,
    kRobot,
    kWall,
    kWallGhost,
    kWhite,
    kWhiteGhost,

    kMax
  };

  class TextureStyle final {
  public:
    std::string dir_name{};
    std::string name{};
    asset_id_t id_begin{};

    explicit TextureStyle(GpuGfxLoader& gfx,bool is_weird,std::size_t style_count,
                          const std::filesystem::path& dir);

  private:
    void load_texture(GpuGfxLoader& gfx,bool is_weird,StyledTextureId id,const std::filesystem::path& file,
                      const Color4f& weird_color = Color4f::kNone);
  };

  std::string style_dir_name_{};
  std::string style_name_{};

  std::vector<TextureStyle> texture_styles_{};
  std::size_t texture_styles_index_ = 0;
  StyledTextureIds styled_texture_ids_{};

  void update_style();
};

} // namespace ekoscape
#endif
