/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "menu_scene.h"

namespace ekoscape {

const tiny_utf8::string MenuScene::kGraphicsText = "gfx: ";
const Color4f MenuScene::kArrowColor = {0,252,0};
const Color4f MenuScene::kCycleArrowColor = {254,0,0};
const Color4f MenuScene::kTextColor = {214,214,214};
const tiny_utf8::string MenuScene::kLeftArrowText = "←";
const tiny_utf8::string MenuScene::kRightArrowText = "→";
const int MenuScene::kSmallSpaceSize = 24;

MenuScene::MenuScene(Assets& assets)
    : assets_(assets) {
  for(auto& option: options_) {
    if(option.type == OptionType::kGraphics) {
      update_graphics_option(option);
      break;
    }
  }
}

void MenuScene::on_key_down_event(SDL_Keycode key) {
  auto& selected_option = options_.at(selected_option_index_);

  switch(key) {
    case SDLK_RETURN:
    case SDLK_SPACE:
    case SDLK_KP_ENTER:
      switch(selected_option.type) {
        case OptionType::kPlay:
          scene_action_ = SceneAction::kGoToMenuPlay;
          break;

        case OptionType::kGraphics:
          assets_.next_graphics_style();
          update_graphics_option(selected_option);
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
      if(selected_option_index_ > 0) { --selected_option_index_; }
      break;

    case SDLK_DOWN:
    case SDLK_s:
      if(selected_option_index_ < (static_cast<int>(options_.size()) - 1)) {
        ++selected_option_index_;
      }
      break;

    case SDLK_LEFT:
    case SDLK_a:
      switch(selected_option.type) {
        case OptionType::kGraphics:
          assets_.prev_graphics_style();
          update_graphics_option(selected_option);
          break;

        default: break; // Ignore.
      }
      break;

    case SDLK_RIGHT:
    case SDLK_d:
      switch(selected_option.type) {
        case OptionType::kGraphics:
          assets_.next_graphics_style();
          update_graphics_option(selected_option);
          break;

        default: break; // Ignore.
      }
      break;
  }
}

int MenuScene::update_scene_logic(const FrameStep& /*step*/) {
  const int action = scene_action_;
  scene_action_ = SceneAction::kNil; // Avoid possible infinite loop.

  return action;
}

void MenuScene::draw_scene(Renderer& ren) {
  ren.begin_2d_scene()
     .begin_auto_center();

  ren.wrap_sprite(assets_.logo_sprite(),[&](auto& s) {
    s.draw_quad(150,10,1300,300);
  });

  ren.wrap_font_atlas(assets_.font_atlas(),395,330,40,90,[&](auto& font) {
    for(int i = 0; i < static_cast<int>(options_.size()); ++i) {
      auto& option = options_[i];

      if(i == selected_option_index_) {
        const Color4f* color;
        const tiny_utf8::string* text;

        if(option.type == OptionType::kGraphics) {
          color = &kCycleArrowColor;
          text = &kLeftArrowText;
        } else {
          color = &kArrowColor;
          text = &kRightArrowText;
        }

        ren.wrap_color(*color,[&]() { font.print(*text); });
      } else {
        font.print(); // Space.
      }

      font.pos.x += kSmallSpaceSize;
      ren.wrap_color(kTextColor,[&]() { font.print(option.text); });

      if(i == selected_option_index_) {
        if(option.type == OptionType::kGraphics) {
          font.pos.x += kSmallSpaceSize;
          ren.wrap_color(kCycleArrowColor,[&]() { font.print(kRightArrowText); });
        }
      }

      font.puts();
    }
  });

  ren.wrap_texture(assets_.robot_texture(),[&](auto& tex) {
    tex.draw_quad(10,368,300,256);
  });
  ren.wrap_texture(assets_.cell_texture(),[&](auto& tex) {
    tex.draw_quad(10,634,300,256);
  });
  ren.wrap_sprite(assets_.keys_sprite(),[&](auto& s) {
    s.draw_quad(849,670,741,220); // 1482x440.
  });
}

void MenuScene::update_graphics_option(Option& option) {
  option.text = kGraphicsText + assets_.graphics_style_name();
}

} // Namespace.
