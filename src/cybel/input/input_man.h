/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_INPUT_MAN_H_
#define CYBEL_INPUT_INPUT_MAN_H_

#include "cybel/common.h"

#include "cybel/input/game_ctrl.h"
#include "cybel/input/joypad_input.h"
#include "cybel/input/joystick.h"
#include "cybel/input/key_input.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cybel {

using InputIds = std::unordered_set<int>;

class InputMan {
public:
  class InputMapper {
  public:
    explicit InputMapper(InputMan& input_man,int id);

    void raw_key(std::initializer_list<RawKey> keys,KeyMods mods = 0);
    void raw_key(std::initializer_list<RawKeyInput> keys);

    void sym_key(std::initializer_list<SymKey> keys,KeyMods mods = 0);
    void sym_key(std::initializer_list<SymKeyInput> keys);

    void joypad(std::initializer_list<JoypadInput> inputs);

  private:
    InputMan& input_man_;
    int id_{};
  };

  using MapInputCallback = std::function<void(InputMapper&)>;
  using OnInputEvent = std::function<void(int id)>;

  static inline const InputIds kEmptyIds{};

  explicit InputMan(int max_id = 0);

  void map_input(int id,const MapInputCallback& callback);

  void begin_input();
  void handle_event(const SDL_Event& event,const OnInputEvent& on_input_event);
  void update_states();

  void set_state(const RawKeyInput& key,bool state);
  void set_state(const SymKeyInput& key,bool state);
  void set_state(JoypadInput input,bool state);

  const InputIds& fetch_ids(const RawKeyInput& key) const;
  const InputIds& fetch_ids(const SymKeyInput& key) const;
  const InputIds& fetch_ids(JoypadInput input) const;
  const std::vector<bool>& states() const;

private:
  // About 8,000.
  static inline const int kJoypadAxisDeadZone = static_cast<int>(std::round(SDL_JOYSTICK_AXIS_MAX * 0.24f));

  int max_id_{};
  std::vector<bool> id_to_state_{};
  std::unordered_set<int> processed_ids_{};

  std::unordered_map<RawKeyInput,InputIds,RawKeyInput::Hash> raw_key_to_ids_{};
  std::unordered_map<SymKeyInput,InputIds,SymKeyInput::Hash> sym_key_to_ids_{};
  std::unordered_map<JoypadInput,InputIds> joypad_input_to_ids_{};

  bool is_joypad_alive_ = false;
  Joystick main_joystick_{};
  GameCtrl main_game_ctrl_{};

  void init_joypad();
  void load_joypads();

  void handle_key_down_event(const SDL_Event& event,const OnInputEvent& on_input_event);
  bool handle_joystick_event(const SDL_Event& event,const OnInputEvent& on_input_event);
  bool handle_game_ctrl_event(const SDL_Event& event,const OnInputEvent& on_input_event);
  void handle_joypad_event(JoypadInput input,const OnInputEvent& on_input_event);

  void update_key_states();
  void update_joypad_states();

  JoypadInput check_joystick_axis();
  JoypadInput check_joystick_button();
  JoypadInput check_game_ctrl_axis();
  JoypadInput check_game_ctrl_button();
};

} // namespace cybel
#endif
