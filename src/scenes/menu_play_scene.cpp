/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_play_scene.h"

namespace ekoscape {

MenuPlayScene::MapOption::MapOption(const CybelStrUtf8& text)
    : text(text) {}

MenuPlayScene::MenuPlayScene(CybelEngine& cybel_engine,Assets& assets,const State& state
    ,const StateCallback& on_state_changed)
    : cybel_engine_(cybel_engine),assets_(assets),state_(state),on_state_changed_(on_state_changed) {
  refresh_maps();
}

void MenuPlayScene::on_key_down_event(SDL_Keycode key) {
  switch(key) {
    case SDLK_RETURN:
    case SDLK_SPACE:
    case SDLK_KP_ENTER:
      if(map_opts_.size() <= 1) {
        cybel_engine_.show_error("No map to select.");
        scene_action_ = SceneAction::kGoBack;
      } else {
        select_map_opt(); // Justin Case.
        scene_action_ = SceneAction::kGoToGame;
      }
      break;

    case SDLK_UP:
    case SDLK_w:
      select_map_opt(map_opt_index_ - 1,true);
      break;

    case SDLK_DOWN:
    case SDLK_s:
      select_map_opt(map_opt_index_ + 1,true);
      break;

    case SDLK_LEFT:
    case SDLK_a:
      prev_map_opt_group();
      break;

    case SDLK_RIGHT:
    case SDLK_d:
      next_map_opt_group();
      break;

    case SDLK_PAGEUP:
      select_map_opt(map_opt_index_ - kMinMapOptsHalf,false);
      break;

    case SDLK_PAGEDOWN:
      select_map_opt(map_opt_index_ + kMinMapOptsHalf,false);
      break;

    // Refresh.
    case SDLK_r:
      refresh_maps();
      break;
  }
}

int MenuPlayScene::update_scene_logic(const FrameStep& /*step*/,const ViewDimens& /*dimens*/) {
  const int action = scene_action_;
  scene_action_ = SceneAction::kNil; // Avoid possible infinite loop.

  return action;
}

void MenuPlayScene::draw_scene(Renderer& ren) {
  ren.begin_2d_scene()
     .begin_auto_center_scale()
     .begin_add_blend();

  assets_.font_renderer().wrap(ren,{25,10,0},0.75f,[&](auto& font) {
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
  map_opt_index_ = 0; // Random map.

  if(!state_.is_rand_map) {
    const int opts_len = static_cast<int>(map_opts_.size());

    for(int i = 0; i < opts_len; ++i) {
      if(map_opts_[i].file == state_.map_file) {
        map_opt_index_ = i;
        break;
      }
    }
  }

  select_map_opt();
}

void MenuPlayScene::glob_maps() {
  const int max_group_len = 15;
  const int max_title_len = 25;

  map_opts_.clear();
  map_opts_.emplace_back("< Random Map >");

  try {
    for(const auto& top_dir: std::filesystem::directory_iterator(assets_.kMapsDir)) {
      if(!top_dir.is_directory()) { continue; }

      const CybelStrUtf8 group = Util::ellips_str(top_dir.path().filename().string(),max_group_len);

      for(const auto& file: std::filesystem::directory_iterator(top_dir)) {
        if(!file.is_regular_file() || !Map::is_map_file(file)) {
          continue;
        }

        Map map{};
        try {
          map.load_file_meta(file);
        } catch(const CybelError& e) {
          std::cerr << "[WARN] " << e.what() << std::endl;
          continue;
        }

        MapOption opt{};
        opt.group = group;
        opt.file = file;
        opt.title = map.title();

        std::stringstream ss{};
        ss << Util::pad_str(Util::ellips_str(opt.title,max_title_len),max_title_len)
           << " [" << opt.group << ']';
        opt.text = ss.str();

        map_opts_.push_back(opt);
      }
    }
  } catch(const std::filesystem::filesystem_error& e) {
    cybel_engine_.show_error(e.what());
  }

  int order = 0;
  std::unordered_map<CybelStrUtf8,int> core_groups{
    {"user",order++},
    {"fanmade",order++},
    {"neo",order++},
    {"classic",order++},
  };

  std::sort(
    map_opts_.begin() + 1,map_opts_.end()
    ,[&](const auto& opt1,const auto& opt2) {
      const bool is_core_group1 = core_groups.contains(opt1.group);
      const bool is_core_group2 = core_groups.contains(opt2.group);

      // Bubble non-core groups to top.
      if(is_core_group1 && !is_core_group2) { return false; } // 1.
      if(!is_core_group1 && is_core_group2) { return true; } // -1.

      int group_cmp = 0;

      if(is_core_group1 && is_core_group2) {
        group_cmp = core_groups[opt1.group] - core_groups[opt2.group];
      } else {
        group_cmp = Util::comparei_str(opt1.group,opt2.group);
      }

      if(group_cmp != 0) { return group_cmp < 0; }
      return Util::comparei_str(opt1.title,opt2.title) < 0;
    }
  );
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

  const int opts_len = static_cast<int>(map_opts_.size());
  const MapOption& sel_opt = map_opts_.at(map_opt_index_);
  int i = map_opt_index_;

  for(; i < opts_len; ++i) {
    const MapOption& opt = map_opts_[i];
    if(opt.group != sel_opt.group) { break; }
  }

  select_map_opt(i,true);
}

void MenuPlayScene::select_map_opt() {
  select_map_opt(map_opt_index_,false,true);
}

void MenuPlayScene::select_map_opt(int index,bool wrap,bool force) {
  if(map_opts_.empty()) {
    if(force) {
      state_.map_file.clear();
      state_.is_rand_map = true;
      on_state_changed_(state_);
    }
    return;
  }

  const int opts_len = static_cast<int>(map_opts_.size());

  if(index < 0) {
    index = wrap ? (opts_len - 1) : 0;
  } else if(index >= opts_len) {
    index = wrap ? 0 : (opts_len - 1);
  }
  if(!force && index == map_opt_index_) { return; }

  map_opt_index_ = index;
  state_.is_rand_map = (map_opt_index_ == 0);

  if(state_.is_rand_map) {
    if(opts_len >= 2) {
      state_.map_file = map_opts_.at(Rando::it().rand_int(1,opts_len)).file;
    } else {
      state_.map_file.clear();
    }
  } else {
    state_.map_file = map_opts_.at(map_opt_index_).file;
  }

  on_state_changed_(state_);
}

} // Namespace.
