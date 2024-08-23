/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_SCENE_H_
#define EKOSCAPE_SCENES_MENU_SCENE_H_

#include "cybel/common.h"

#include "cybel/scene/scene.h"

#include "assets.h"
#include "scene_action.h"

#include <vector>

namespace ekoscape {

class MenuScene : public Scene {
public:
  enum class OptionType {
    kPlay,
    kGraphics,
    kCredits,
    kQuit,
  };

  struct Option {
    OptionType type{};
    tiny_utf8::string text{};
  };

  explicit MenuScene(Assets& assets);

  void on_key_down_event(SDL_Keycode key) override;
  int update_scene_logic(const FrameStep& step) override;
  void draw_scene(Renderer& ren) override;

private:
  static const tiny_utf8::string kGraphicsText;
  static const Color4f kArrowColor;
  static const Color4f kCycleArrowColor;
  static const Color4f kTextColor;
  static const tiny_utf8::string kLeftArrowText;
  static const tiny_utf8::string kRightArrowText;
  static const int kSmallSpaceSize;

  Assets& assets_;
  int scene_action_ = SceneAction::kNil;

  int selected_option_index_ = 0;
  std::vector<Option> options_ = {
    {OptionType::kPlay,"play"},
    {OptionType::kGraphics,""},
    {OptionType::kCredits,"credits"},
    {OptionType::kQuit,"quit"},
  };

  void update_graphics_option(Option& option);
};

} // Namespace.
#endif
