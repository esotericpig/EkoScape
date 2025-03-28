/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_play_scene.h"

#include "cybel/str/utf8/str_util.h"
#include "cybel/util/rando.h"

#include "input/input_action.h"

namespace ekoscape {

MenuPlayScene::MapOption::MapOption(std::string_view text)
  : text(text) {}

MenuPlayScene::MenuPlayScene(GameContext& ctx,State& state)
  : ctx_(ctx),state_(state) {
  refresh_maps();
}

void MenuPlayScene::on_scene_input_event(int input_id,const ViewDimens& /*dimens*/) {
  switch(input_id) {
    case InputAction::kSelect:
      if(map_opt_index_ == 1) {
        scene_action_ = SceneAction::kGoBack;
      } else {
        if(map_opts_.size() <= kNonMapOptCount) {
          ctx_.cybel_engine.show_error("No map to select.");
        } else {
          sync_map_opt(); // Justin Case.
          scene_action_ = SceneAction::kGoToGame;
        }
      }
      break;

    case InputAction::kUp:
      select_map_opt(map_opt_index_ - 1,true);
      break;

    case InputAction::kDown:
      select_map_opt(map_opt_index_ + 1,true);
      break;

    case InputAction::kLeft:
      prev_map_opt_group();
      break;

    case InputAction::kRight:
      next_map_opt_group();
      break;

    case InputAction::kPageUp:
      select_map_opt(map_opt_index_ - kMinMapOptsHalf,false);
      break;

    case InputAction::kPageDown:
      select_map_opt(map_opt_index_ + kMinMapOptsHalf,false);
      break;

    case InputAction::kRefresh:
      refresh_maps();
      break;
  }
}

int MenuPlayScene::update_scene_logic(const FrameStep& /*step*/,const ViewDimens& /*dimens*/) {
  return std::exchange(scene_action_,SceneAction::kNil);
}

void MenuPlayScene::draw_scene(Renderer& ren,const ViewDimens& /*dimens*/) {
  ren.begin_2d_scene()
     .begin_auto_center_scale()
     .begin_add_blend();

  ctx_.assets.font_renderer().wrap(ren,Pos3i{25,10,0},0.75f,[&](auto& font) {
    if(!map_opts_.empty()) {
      const int opts_len = static_cast<int>(map_opts_.size());
      const int first_i = map_opt_index_ - kMapOptsHalf1;
      const int max_len = std::min(map_opt_index_ + 1 + kMapOptsHalf2,opts_len);

      if(first_i > 0) { // More options hidden at top?
        font.print_blanks(kUpDownArrowIndent);
        font.draw_menu_up_arrow();
      } else {
        font.puts();
      }
      for(int i = first_i; i < map_opt_index_; ++i) {
        if(i >= 0) {
          font.draw_menu_opt(map_opts_[i].text);
        } else {
          font.puts();
        }
      }

      font.draw_menu_opt(map_opts_.at(map_opt_index_).text,FontRenderer::kMenuStyleSelected);

      for(int i = map_opt_index_ + 1; i < max_len; ++i) {
        font.draw_menu_opt(map_opts_[i].text);
      }
      if(max_len < opts_len) { // More options hidden at bottom?
        font.print_blanks(kUpDownArrowIndent);
        font.draw_menu_down_arrow();
      }
    }
  });

  ren.end_blend()
     .end_scale();
}

void MenuPlayScene::refresh_maps() {
  glob_maps();
  sync_map_opt();
}

void MenuPlayScene::glob_maps() {
  static constexpr int kMaxTitleLen = 25;
  static constexpr int kMaxGroupLen = 17;

  map_opts_.clear();
  map_opts_.emplace_back("< random map >");
  map_opts_.emplace_back("< go back >");
  map_opt_index_ = 0;

  ctx_.assets.glob_maps_meta([&](const auto& group,const auto& map_file,const auto& map) {
    MapOption opt{};

    opt.group = group;
    opt.file = map_file;
    opt.title = map.title();
    opt.text = utf8::StrUtil::ljust(utf8::StrUtil::ellipsize(opt.title,kMaxTitleLen),kMaxTitleLen) +
               ' ' + utf8::StrUtil::ellipsize(opt.group,kMaxGroupLen);

    map_opts_.push_back(opt);
  });

  if(map_opts_.size() <= kNonMapOptCount) {
    ctx_.cybel_engine.show_error("No maps were found/loaded in the sub folders of the maps folder [" +
                                 Assets::kMapsSubdir.string() + "].");
    return;
  }

  // Sort by: non-core group, core group, & title.
  std::sort(
    map_opts_.begin() + kNonMapOptCount,map_opts_.end(),
    [&](const auto& opt1,const auto& opt2) {
      // Return true if opt1 < opt2.

      const bool is_core_group1 = kCoreGroupToPriority.contains(opt1.group);
      const bool is_core_group2 = kCoreGroupToPriority.contains(opt2.group);

      // Bubble non-core groups to top.
      if(is_core_group1 && !is_core_group2) { return false; } // opt1 > opt2.
      if(!is_core_group1 && is_core_group2) { return true; } // opt1 < opt2.

      int group_cmp = 0;

      if(is_core_group1 && is_core_group2) {
        group_cmp = kCoreGroupToPriority[opt1.group] - kCoreGroupToPriority[opt2.group];
      } else {
        group_cmp = utf8::StrUtil::casecmp_ascii(opt1.group,opt2.group);
      }

      if(group_cmp != 0) { return group_cmp < 0; }
      return utf8::StrUtil::casecmp_ascii(opt1.title,opt2.title) < 0;
    }
  );

  // Select the correct map from the previous/current state.
  if(!state_.is_rand_map) {
    for(int i = 0; i < static_cast<int>(map_opts_.size()); ++i) {
      if(map_opts_[i].file == state_.map_file) {
        map_opt_index_ = i;
        break;
      }
    }
  }
}

void MenuPlayScene::prev_map_opt_group() {
  if(map_opts_.empty()) { return; }

  const MapOption& sel_opt = map_opts_.at(map_opt_index_);
  int i = map_opt_index_;

  for(; i >= 0; --i) {
    const MapOption& opt = map_opts_[i];
    if(opt.group != sel_opt.group) { break; }
  }

  select_map_opt(i,true);
}

void MenuPlayScene::next_map_opt_group() {
  if(map_opts_.empty()) { return; }

  const MapOption& sel_opt = map_opts_.at(map_opt_index_);
  int i = map_opt_index_;

  for(; i < static_cast<int>(map_opts_.size()); ++i) {
    const MapOption& opt = map_opts_[i];
    if(opt.group != sel_opt.group) { break; }
  }

  select_map_opt(i,true);
}

void MenuPlayScene::sync_map_opt() {
  select_map_opt(map_opt_index_,false,true);
}

void MenuPlayScene::select_map_opt(int index,bool wrap,bool force) {
  if(map_opts_.empty()) { return; }

  const auto opts_len = static_cast<int>(map_opts_.size());

  if(index < 0) {
    index = wrap ? (opts_len - 1) : 0;
  } else if(index >= opts_len) {
    index = wrap ? 0 : (opts_len - 1);
  }
  if(!force && index == map_opt_index_) { return; }

  map_opt_index_ = index;
  state_.is_rand_map = (map_opt_index_ == 0);

  if(opts_len <= kNonMapOptCount) {
    if(force) {
      state_.map_file.clear();
      state_.is_rand_map = true;
    }
  } else if(state_.is_rand_map) {
    state_.map_file = map_opts_.at(Rando::it().rand_int(kNonMapOptCount,opts_len)).file;
  } else {
    state_.map_file = map_opts_.at(map_opt_index_).file;
  }
}

} // namespace ekoscape
