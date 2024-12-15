/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_MENU_SCENE_H_
#define EKOSCAPE_SCENES_MENU_SCENE_H_

#include "common.h"

#include "cybel/scene/scene.h"

#include "assets/assets.h"
#include "scene_action.h"

#include <vector>

namespace ekoscape {

class MenuScene : public Scene {
public:
  explicit MenuScene(Assets& assets);

  void on_key_down_event(const KeyEvent& event,const ViewDimens& dimens) override;
  int update_scene_logic(const FrameStep& step,const ViewDimens& dimens) override;
  void draw_scene(Renderer& ren,const ViewDimens& dimens) override;

private:
  enum class OptionType {
    kPlay,
    kGraphics,
    kCredits,
    kQuit,
  };

  struct Option {
    OptionType type{};
    StrUtf8 text{};
  };

  static inline const StrUtf8 kGraphicsText = "gfx: ";

  Assets& assets_;
  int scene_action_ = SceneAction::kNil;

  std::vector<Option> opts_ = {
    {OptionType::kPlay,"play"},
    {OptionType::kGraphics,""},
    {OptionType::kCredits,"credits"},
    {OptionType::kQuit,"quit"},
  };
  int opt_index_ = 0;

  void update_graphics_opt(Option& opt);
};

} // Namespace.
#endif
