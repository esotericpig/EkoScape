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
#include "cybel/input/input_types.h"
#include "cybel/input/joypad_input.h"
#include "cybel/input/joystick.h"
#include "cybel/input/key_input.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cybel {

/**
 * TODO: Touch (finger) input is not implemented seriously for now, but just for fun,
 *       and it currently relies on the joypad logic.
 */
class InputMan {
public:
  class InputMapper {
  public:
    explicit InputMapper(InputMan& input_man,input_id_t id);

    void raw_key(std::initializer_list<RawKey> keys,key_mods_t mods = KMOD_NONE);
    void raw_key(std::initializer_list<std::pair<key_mods_t,RawKey>> keys);

    void sym_key(std::initializer_list<SymKey> keys,key_mods_t mods = KMOD_NONE);
    void sym_key(std::initializer_list<std::pair<key_mods_t,SymKey>> keys);

    void joypad(std::initializer_list<JoypadInput> inputs);

  private:
    InputMan& input_man_;
    input_id_t id_{};
  };

  using MapInputCallback = std::function<void(InputMapper&)>;
  using OnInputEvent = std::function<void(input_id_t id)>;

  static inline const InputIds kEmptyIds{};

  explicit InputMan(input_id_t max_id = 0);

  void map_input(input_id_t id,const MapInputCallback& callback);

  /// TEST: Only use for testing purposes.
  void use_fake_joypad(bool use_game_ctrl,FakeJoypadInputType input_type);
  /// TEST: Only use for testing purposes.
  void use_mouse_as_finger();

  void begin_input();
  void handle_event(const SDL_Event& event,const OnInputEvent& on_input_event);

  void set_state(const RawKeyInput& key,bool state);
  void set_state(const SymKeyInput& key,bool state);
  void set_state(JoypadInput input,bool state);

  const InputIds& fetch_ids(const RawKeyInput& key) const;
  const InputIds& fetch_ids(const SymKeyInput& key) const;
  const InputIds& fetch_ids(JoypadInput input) const;
  const std::vector<bool>& states() const;

private:
  // About 24% of range: SDL_JOYSTICK_AXIS_MAX(32'767) * 0.24f
  static constexpr Sint16 kJoypadAxisDeadZone = 8'000;

  input_id_t max_id_ = 0;
  std::vector<bool> id_to_state_{};
  std::unordered_set<input_id_t> processed_ids_{};
  OnInputEvent on_input_event_{};

  std::unordered_map<RawKeyInput,InputIds,RawKeyInput::Hash> raw_key_to_ids_{};
  std::unordered_map<SymKeyInput,InputIds,SymKeyInput::Hash> sym_key_to_ids_{};
  std::unordered_map<JoypadInput,InputIds> joypad_input_to_ids_{};

  bool is_joypad_alive_ = false;
  Joystick main_joystick_{};
  GameCtrl main_game_ctrl_{};

  bool is_fake_joypad_ = false;
  bool is_fake_joypad_game_ctrl_ = false;
  FakeJoypadInputType fake_joypad_input_type_{};

  std::vector<bool> touch_input_to_state_{};

  void init_joypad();
  void load_joypads();

  void handle_key_event(const SDL_KeyboardEvent& key);
  void handle_joystick_device_event(const SDL_JoyDeviceEvent& jdevice);
  void handle_joystick_axis_event(const SDL_JoyAxisEvent& jaxis);
  void handle_joystick_hat_event(const SDL_JoyHatEvent& jhat);
  void handle_joystick_button_event(const SDL_JoyButtonEvent& jbutton);
  void handle_game_ctrl_device_event(const SDL_ControllerDeviceEvent& cdevice);
  void handle_game_ctrl_axis_event(const SDL_ControllerAxisEvent& caxis);
  void handle_game_ctrl_button_event(const SDL_ControllerButtonEvent& cbutton);
  void handle_joypad_axis_event(SDL_GameControllerAxis axis,Sint16 value);
  void handle_joypad_event(JoypadInput input,bool state);
  bool emit_fake_joypad_events(const SDL_Event& event);
  void handle_finger_event(const SDL_TouchFingerEvent& tfinger);
  void handle_touch_event(JoypadInput input,bool state);

  void reset_states();
  void reset_touch_states();
};

} // namespace cybel
#endif
