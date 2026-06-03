/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "art_styles.h"

#include "cybel/str/utf8/str_util.h"
#include "cybel/util/util.h"

#include "assets/assets.h"

#include <algorithm>
#include <cassert>
#include <system_error>
#include <unordered_set>

namespace ekoscape {

ArtStyles::ArtStyles(std::string_view style)
  : style_dir_name_{style},
    style_name_{style} {}

void ArtStyles::load_gpu_gfx(const AssetMan& assets,GpuGfxLoader& gfx,bool is_weird) {
  texture_styles_.clear();
  texture_styles_index_ = 0;

  std::unordered_set<std::filesystem::path> loaded_dir_names{};
  std::string errors{};

  // Crawl textures dir and load valid style dirs that have all textures.
  for(const auto& asset_dir : assets.asset_dirs()) {
    const auto tex_dir = asset_dir / Assets::kTexturesSubDir;
    std::error_code ec{};

    if(!is_directory(tex_dir,ec)) { continue; } // ADL (Argument-Dependent Lookup).

    try {
      for(const auto& style_entry : std::filesystem::directory_iterator(tex_dir)) {
        if(!style_entry.is_directory(ec)) { continue; }

        const auto style_dir = style_entry.path();
        const auto style_dir_name = style_dir.filename();

        if(loaded_dir_names.contains(style_dir_name)) { continue; }

        texture_styles_.emplace_back(gfx,is_weird,texture_styles_.size(),style_dir);
        loaded_dir_names.insert(style_dir_name); // Success.
      }
    } catch(const CybelError& e) {
      std::cerr << "[WARN] " << e.what() << '\n';

      errors += "\n- ";
      errors += e.what();
    } catch(const std::filesystem::filesystem_error& e) {
      const auto err_msg = Util::build_str("Failed to crawl Textures folder `",tex_dir,"`: ",e.what(),'.');

      std::cerr << "[WARN] " << err_msg << '\n';

      errors += "\n- ";
      errors += err_msg;
    }
  }

  if(texture_styles_.empty()) {
    throw CybelError{"Failed to find/load any Styled Textures in Textures folder `",Assets::kTexturesSubDir,
                     "`.\n",errors};
  }

  texture_styles_.shrink_to_fit();

  // Sort the styles alphabetically, ignoring case.
  std::ranges::sort(texture_styles_,[](const auto& style1,const auto& style2) {
    return utf8::StrUtil::casecmp_ascii(style1.dir_name,style2.dir_name) < 0;
  });

  // Auto-select the preferred style, ignoring case.
  for(; texture_styles_index_ < texture_styles_.size(); ++texture_styles_index_) {
    const auto& tex_style = texture_styles_[texture_styles_index_];

    if(utf8::StrUtil::casecmp_ascii(tex_style.dir_name,style_dir_name_) == 0) {
      break;
    }
  }

  if(texture_styles_index_ >= texture_styles_.size()) {
    texture_styles_index_ = 0;

    std::cerr << "[WARN] Failed to find/load Styled Textures for Art Style `" << style_dir_name_ << "`.\n";
  }

  update_style();
}

void ArtStyles::update_style() {
  assert(!texture_styles_.empty());
  assert(texture_styles_index_ < texture_styles_.size());

  const auto& tex_style = texture_styles_[texture_styles_index_];
  const auto id_begin = tex_style.id_begin;
  auto& ids = styled_texture_ids_;

  style_dir_name_ = tex_style.dir_name;
  style_name_ = tex_style.name;

  ids.ceiling = id_begin + static_cast<asset_id_t>(StyledTextureId::kCeiling);
  ids.cell = id_begin + static_cast<asset_id_t>(StyledTextureId::kCell);
  ids.dead_space = id_begin + static_cast<asset_id_t>(StyledTextureId::kDeadSpace);
  ids.dead_space_ghost = id_begin + static_cast<asset_id_t>(StyledTextureId::kDeadSpaceGhost);
  ids.end = id_begin + static_cast<asset_id_t>(StyledTextureId::kEnd);
  ids.end_wall = id_begin + static_cast<asset_id_t>(StyledTextureId::kEndWall);
  ids.floor = id_begin + static_cast<asset_id_t>(StyledTextureId::kFloor);
  ids.fruit = id_begin + static_cast<asset_id_t>(StyledTextureId::kFruit);
  ids.portal = id_begin + static_cast<asset_id_t>(StyledTextureId::kPortal);
  ids.robot = id_begin + static_cast<asset_id_t>(StyledTextureId::kRobot);
  ids.wall = id_begin + static_cast<asset_id_t>(StyledTextureId::kWall);
  ids.wall_ghost = id_begin + static_cast<asset_id_t>(StyledTextureId::kWallGhost);
  ids.white = id_begin + static_cast<asset_id_t>(StyledTextureId::kWhite);
  ids.white_ghost = id_begin + static_cast<asset_id_t>(StyledTextureId::kWhiteGhost);
}

void ArtStyles::prev_style() {
  assert(!texture_styles_.empty());

  if(texture_styles_index_ == 0) {
    texture_styles_index_ = texture_styles_.size();
  }
  --texture_styles_index_;

  update_style();
}

void ArtStyles::next_style() {
  texture_styles_index_ = (texture_styles_index_ + 1) % texture_styles_.size();

  update_style();
}

const std::string& ArtStyles::style_name() const { return style_name_; }

const StyledTextureIds& ArtStyles::styled_texture_ids() const { return styled_texture_ids_; }

ArtStyles::TextureStyle::TextureStyle(GpuGfxLoader& gfx,bool is_weird,std::size_t style_count,
                                      const std::filesystem::path& dir)
  : dir_name{dir.filename()},
    name{utf8::StrUtil::ellipsize(dir_name,18)} {
  constexpr auto tex_count = static_cast<asset_id_t>(StyledTextureId::kMax);
  id_begin = static_cast<asset_id_t>(TextureId::kStyleBegin) + (style_count * tex_count);

  load_texture(gfx,is_weird,StyledTextureId::kCeiling,dir / "ceiling.png");
  load_texture(gfx,is_weird,StyledTextureId::kCell,dir / "cell.png");
  load_texture(gfx,is_weird,StyledTextureId::kDeadSpace,dir / "dead_space.png",Assets::kWeirdBlackColor);
  load_texture(gfx,is_weird,StyledTextureId::kDeadSpaceGhost,dir / "dead_space_ghost.png",
               Assets::kWeirdBlackColor);
  load_texture(gfx,is_weird,StyledTextureId::kEnd,dir / "end.png");
  load_texture(gfx,is_weird,StyledTextureId::kEndWall,dir / "end_wall.png");
  load_texture(gfx,is_weird,StyledTextureId::kFloor,dir / "floor.png");
  load_texture(gfx,is_weird,StyledTextureId::kFruit,dir / "fruit.png");
  load_texture(gfx,is_weird,StyledTextureId::kPortal,dir / "portal.png");
  load_texture(gfx,is_weird,StyledTextureId::kRobot,dir / "robot.png",Assets::kWeirdGrayColor);
  load_texture(gfx,is_weird,StyledTextureId::kWall,dir / "wall.png");
  load_texture(gfx,is_weird,StyledTextureId::kWallGhost,dir / "wall_ghost.png");
  load_texture(gfx,is_weird,StyledTextureId::kWhite,dir / "white.png",Assets::kWeirdWhiteColor);
  load_texture(gfx,is_weird,StyledTextureId::kWhiteGhost,dir / "white_ghost.png",Assets::kWeirdWhiteColor);
}

void ArtStyles::TextureStyle::load_texture(GpuGfxLoader& gfx,bool is_weird,StyledTextureId id,
                                           const std::filesystem::path& file,const Color4f& weird_color) {
  Image image = gfx.load_image(file);

  if(is_weird) {
    if(weird_color == Color4f::kNone) {
      image.make_weird();
    } else {
      image.colorize(weird_color);
    }
  }

  gfx.load_texture(id_begin + static_cast<asset_id_t>(id),image);
}

} // namespace ekoscape
