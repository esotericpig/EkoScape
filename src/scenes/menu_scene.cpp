/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_scene.h"

namespace ekoscape {

MenuScene::MenuScene(Assets& assets)
    : assets_(assets) {
  for(auto& opt: opts_) {
    if(opt.type == OptionType::kGraphics) {
      update_graphics_opt(opt);
      break;
    }
  }
}

void MenuScene::on_key_down_event(const KeyEvent& event,const ViewDimens& /*dimens*/) {
  Option& sel_opt = opts_.at(opt_index_);

  switch(event.key) {
    case SDLK_RETURN:
    case SDLK_SPACE:
    case SDLK_KP_ENTER:
      switch(sel_opt.type) {
        case OptionType::kPlay:
          scene_action_ = SceneAction::kGoToMenuPlay;
          break;

        case OptionType::kGraphics:
          assets_.next_tex_style();
          update_graphics_opt(sel_opt);
          break;

        case OptionType::kCredits:
          scene_action_ = SceneAction::kGoToMenuCredits;
          break;

        case OptionType::kQuit:
          scene_action_ = SceneAction::kQuit;
          break;
      }
      break;

    case SDLK_UP:
    case SDLK_w:
      if(opt_index_ > 0) {
        --opt_index_;
      } else if(opts_.size() > 0) {
        opt_index_ = static_cast<int>(opts_.size()) - 1; // Wrap to bottom.
      }
      break;

    case SDLK_DOWN:
    case SDLK_s:
      if(opt_index_ < (static_cast<int>(opts_.size()) - 1)) {
        ++opt_index_;
      } else {
        opt_index_ = 0; // Wrap to top.
      }
      break;

    case SDLK_LEFT:
    case SDLK_a:
      switch(sel_opt.type) {
        case OptionType::kGraphics:
          assets_.prev_tex_style();
          update_graphics_opt(sel_opt);
          break;

        default: break; // Ignore.
      }
      break;

    case SDLK_RIGHT:
    case SDLK_d:
      switch(sel_opt.type) {
        case OptionType::kGraphics:
          assets_.next_tex_style();
          update_graphics_opt(sel_opt);
          break;

        default: break; // Ignore.
      }
      break;
  }
}

int MenuScene::update_scene_logic(const FrameStep& /*step*/,const ViewDimens& /*dimens*/) {
  return std::exchange(scene_action_,SceneAction::kNil);
}

void MenuScene::draw_scene(Renderer& ren,const ViewDimens& /*dimens*/) {
  ren.begin_2d_scene()
     .begin_auto_center_scale()
     .begin_add_blend();

  ren.wrap_sprite(assets_.logo_sprite(),[&](auto& s) {
    s.draw_quad({150,10,0},{1300,300});
  });

  assets_.font_renderer().wrap(ren,{395,330,0},[&](auto& font) {
    for(std::size_t i = 0; i < opts_.size(); ++i) {
      Option& opt = opts_[i];
      int styles = 0;

      if(static_cast<int>(i) == opt_index_) {
        if(opt.type == OptionType::kGraphics) {
          styles |= FontRenderer::kMenuStyleCycle;
        } else {
          styles |= FontRenderer::kMenuStyleSelected;
        }
      }

      font.draw_menu_opt(opt.text,styles);
    }
  });

  ren.wrap_tex(assets_.robot_tex(),[&](auto& tex) {
    tex.draw_quad({10,368,0},{300,256});
  });
  ren.wrap_tex(assets_.cell_tex(),[&](auto& tex) {
    tex.draw_quad({10,634,0},{300,256});
  });
  ren.wrap_sprite(assets_.keys_sprite(),[&](auto& s) {
    s.draw_quad({849,670,0},{741,220}); // 1482x440.
  });

  ren.end_blend()
     .end_scale();
}

void MenuScene::update_graphics_opt(Option& opt) {
  opt.text = kGraphicsText + assets_.tex_style();
}

} // Namespace.
