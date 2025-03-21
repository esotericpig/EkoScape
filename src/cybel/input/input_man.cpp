/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "input_man.h"

#include "cybel/types/cybel_error.h"

namespace cybel {

InputMan::InputMan(int max_id)
  : max_id_(max_id),id_to_state_((max_id > 0) ? (max_id + 1) : 25,false) {
  init_joypad();
}

void InputMan::init_joypad() {
  // SDL_INIT_GAMECONTROLLER also auto-initializes SDL_INIT_JOYSTICK.
  if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
    // Don't fail, since optional.
    std::cerr << "[WARN] Failed to init game controllers & joysticks: " << Util::get_sdl_error() << '.'
              << std::endl;
    return;
  }

  is_joypad_alive_ = true;
  load_joypads();
}

void InputMan::load_joypads() {
  if(!is_joypad_alive_ || main_game_ctrl_ || main_joystick_) { return; }

  // Prefer game controllers over joysticks.
  for(int i = 0; i < SDL_NumJoysticks(); ++i) {
    if(!main_game_ctrl_ && SDL_IsGameController(i) == SDL_TRUE) {
      main_game_ctrl_.open(i);
      main_joystick_.close();
      break;
    }

    if(!main_joystick_) { main_joystick_.open(i); }
  }
}

void InputMan::map_input(int id,const MapInputCallback& callback) {
  if(id < 0) { throw CybelError{"Invalid input ID [",id,"] is < 0."}; }

  // Growable and too small?
  if(max_id_ <= 0 && id >= static_cast<int>(id_to_state_.size())) {
    const auto new_size = std::max(static_cast<std::size_t>(id) + 1,
                                   (id_to_state_.size() + 1) << 1);
    id_to_state_.resize(new_size,false);
  }
  // Not `else if`, in case of casting overflow.
  if(id >= static_cast<int>(id_to_state_.size())) {
    throw CybelError{"Invalid input ID [",id,"] is >= maximum ID count [",
                     static_cast<int>(id_to_state_.size()),',',id_to_state_.size(),"]."};
  }

  InputMapper mapper{*this,id};
  callback(mapper);
}

void InputMan::begin_input() {
  // Can't use std::ranges::fill() because std::vector<bool> is a specialized container
  //     that doesn't meet the requirements of Ranges.
  std::fill(id_to_state_.begin(),id_to_state_.end(),false);

  processed_ids_.clear();
}

void InputMan::handle_event(const SDL_Event& event,const OnInputEvent& on_input_event) {
  switch(event.type) {
    case SDL_KEYDOWN:
      handle_key_down_event(event,on_input_event);
      break;

    default:
      if(is_joypad_alive_) {
        if(!handle_joystick_event(event,on_input_event)) {
          handle_game_ctrl_event(event,on_input_event);
        }
      }
      break;
  }
}

void InputMan::handle_key_down_event(const SDL_Event& event,const OnInputEvent& on_input_event) {
  const RawKeyInput raw_key{event.key.keysym.scancode,event.key.keysym.mod};
  const SymKeyInput sym_key{event.key.keysym.sym,event.key.keysym.mod};

  for(auto id : fetch_ids(raw_key)) {
    // Not inserted? (already processed)
    if(!processed_ids_.insert(id).second) { continue; }

    on_input_event(id);
  }
  for(auto id : fetch_ids(sym_key)) {
    // Not inserted? (already processed)
    if(!processed_ids_.insert(id).second) { continue; }

    on_input_event(id);
  }
}

bool InputMan::handle_joystick_event(const SDL_Event& event,const OnInputEvent& on_input_event) {
  switch(event.type) {
    case SDL_JOYDEVICEADDED:
      // Game controllers also fire this event.
      if(!main_game_ctrl_.matches(event.jdevice.which)) {
        main_joystick_.open(event.jdevice.which);

        if(main_joystick_) { main_game_ctrl_.close(); }
      }
      break;

    case SDL_JOYDEVICEREMOVED:
      if(main_joystick_.matches(event.jdevice.which)) {
        main_joystick_.close();
        load_joypads(); // Try to find another one.
      }
      break;

    case SDL_JOYAXISMOTION:
      if(main_joystick_.matches(event.jaxis.which)) {
        main_joystick_.state.axis = event.jaxis.axis;
        main_joystick_.state.axis_value = event.jaxis.value;

        handle_joypad_event(check_joystick_axis(),on_input_event);
      }
      break;

    case SDL_JOYHATMOTION: {
      if(main_joystick_.matches(event.jhat.which)) {
        const auto hat_value = event.jhat.value;
        main_joystick_.state.hat_value = hat_value;

        if(hat_value > 0) {
          if(hat_value & SDL_HAT_UP) { handle_joypad_event(JoypadInput::kUp,on_input_event); }
          if(hat_value & SDL_HAT_DOWN) { handle_joypad_event(JoypadInput::kDown,on_input_event); }
          if(hat_value & SDL_HAT_LEFT) { handle_joypad_event(JoypadInput::kLeft,on_input_event); }
          if(hat_value & SDL_HAT_RIGHT) { handle_joypad_event(JoypadInput::kRight,on_input_event); }
        }
      }
    } break;

    case SDL_JOYBUTTONDOWN:
      if(main_joystick_.matches(event.jbutton.which)) {
        main_joystick_.state.button = event.jbutton.button;

        handle_joypad_event(check_joystick_button(),on_input_event);
      }
      break;

    case SDL_JOYBUTTONUP:
      if(main_joystick_.matches(event.jbutton.which)) {
        main_joystick_.state.button = -1;
      }
      break;

    default: return false;
  }

  return true;
}

bool InputMan::handle_game_ctrl_event(const SDL_Event& event,const OnInputEvent& on_input_event) {
  switch(event.type) {
    case SDL_CONTROLLERDEVICEADDED:
      main_game_ctrl_.open(event.cdevice.which);

      if(main_game_ctrl_) { main_joystick_.close(); }
      break;

    case SDL_CONTROLLERDEVICEREMOVED:
      if(main_game_ctrl_.matches(event.cdevice.which)) {
        main_game_ctrl_.close();
        load_joypads(); // Try to find another one.
      }
      break;

    case SDL_CONTROLLERAXISMOTION:
      if(main_game_ctrl_.matches(event.caxis.which)) {
        main_game_ctrl_.state.axis = static_cast<SDL_GameControllerAxis>(event.caxis.axis);
        main_game_ctrl_.state.axis_value = event.caxis.value;

        handle_joypad_event(check_game_ctrl_axis(),on_input_event);
      }
      break;

    case SDL_CONTROLLERBUTTONDOWN:
      if(main_game_ctrl_.matches(event.cbutton.which)) {
        main_game_ctrl_.state.button = static_cast<SDL_GameControllerButton>(event.cbutton.button);

        handle_joypad_event(check_game_ctrl_button(),on_input_event);
      }
      break;

    case SDL_CONTROLLERBUTTONUP:
      if(main_game_ctrl_.matches(event.cbutton.which)) {
        main_game_ctrl_.state.button = SDL_CONTROLLER_BUTTON_INVALID;
      }
      break;

    default: return false;
  }

  return true;
}

void InputMan::handle_joypad_event(JoypadInput input,const OnInputEvent& on_input_event) {
  if(input == JoypadInput::kNone) { return; }

  for(auto id : fetch_ids(input)) {
    // Not inserted? (already processed)
    if(!processed_ids_.insert(id).second) { continue; }

    on_input_event(id);
  }
}

void InputMan::update_states() {
  update_key_states();
  update_joypad_states();
}

void InputMan::update_key_states() {
  int num_keys = 0;
  const auto* raw_keys = SDL_GetKeyboardState(&num_keys);
  const KeyMods mods = SDL_GetModState();

  for(int i = 0; i < num_keys; ++i) {
    if(raw_keys[i] == 1) {
      const auto raw_key = static_cast<RawKey>(i);
      const SymKey sym_key = SDL_GetKeyFromScancode(raw_key);

      set_state(RawKeyInput{raw_key,mods},true);
      set_state(SymKeyInput{sym_key,mods},true);
    }
  }
}

void InputMan::update_joypad_states() {
  if(!is_joypad_alive_) { return; }

  if(main_game_ctrl_) {
    set_state(check_game_ctrl_axis(),true);
    set_state(check_game_ctrl_button(),true);
  } else if(main_joystick_) {
    set_state(check_joystick_axis(),true);
    set_state(check_joystick_button(),true);

    const auto hat_value = main_joystick_.state.hat_value;

    if(hat_value > 0) {
      if(hat_value & SDL_HAT_UP) { set_state(JoypadInput::kUp,true); }
      if(hat_value & SDL_HAT_DOWN) { set_state(JoypadInput::kDown,true); }
      if(hat_value & SDL_HAT_LEFT) { set_state(JoypadInput::kLeft,true); }
      if(hat_value & SDL_HAT_RIGHT) { set_state(JoypadInput::kRight,true); }
    }
  }
}

JoypadInput InputMan::check_joystick_axis() {
  const auto value = main_joystick_.state.axis_value;

  if(std::abs(value) > kJoypadAxisDeadZone) {
    switch(main_joystick_.state.axis) {
      // X-axis.
      case 0: return (value < 0) ? JoypadInput::kLeft : JoypadInput::kRight;
      // Y-axis.
      case 1: return (value < 0) ? JoypadInput::kUp : JoypadInput::kDown;

      default: break;
    }
  }

  return JoypadInput::kNone;
}

JoypadInput InputMan::check_joystick_button() {
  switch(main_joystick_.state.button) {
    case 0: return JoypadInput::kA;
    case 1: return JoypadInput::kB;

    default: return JoypadInput::kNone;
  }
}

JoypadInput InputMan::check_game_ctrl_axis() {
  const auto value = main_game_ctrl_.state.axis_value;

  if(std::abs(value) > kJoypadAxisDeadZone) {
    switch(main_game_ctrl_.state.axis) {
      // X-axis.
      case SDL_CONTROLLER_AXIS_LEFTX:
      case SDL_CONTROLLER_AXIS_RIGHTX:
        return (value < 0) ? JoypadInput::kLeft : JoypadInput::kRight;

      // Y-axis.
      case SDL_CONTROLLER_AXIS_LEFTY:
      case SDL_CONTROLLER_AXIS_RIGHTY:
        return (value < 0) ? JoypadInput::kUp : JoypadInput::kDown;

      default: break;
    }
  }

  return JoypadInput::kNone;
}

JoypadInput InputMan::check_game_ctrl_button() {
  switch(main_game_ctrl_.state.button) {
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return JoypadInput::kUp;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return JoypadInput::kDown;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return JoypadInput::kLeft;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return JoypadInput::kRight;
    case SDL_CONTROLLER_BUTTON_A: return JoypadInput::kA;
    case SDL_CONTROLLER_BUTTON_B: return JoypadInput::kB;

    default: return JoypadInput::kNone;
  }
}

void InputMan::set_state(const RawKeyInput& key,bool state) {
  const auto it = raw_key_to_ids_.find(key);
  if(it == raw_key_to_ids_.end()) { return; }

  for(const auto id : it->second) {
    id_to_state_[id] = state;
  }
}

void InputMan::set_state(const SymKeyInput& key,bool state) {
  const auto it = sym_key_to_ids_.find(key);
  if(it == sym_key_to_ids_.end()) { return; }

  for(const auto id : it->second) {
    id_to_state_[id] = state;
  }
}

void InputMan::set_state(JoypadInput input,bool state) {
  if(input == JoypadInput::kNone) { return; }

  const auto it = joypad_input_to_ids_.find(input);
  if(it == joypad_input_to_ids_.end()) { return; }

  for(const auto id : it->second) {
    id_to_state_[id] = state;
  }
}

const InputIds& InputMan::fetch_ids(const RawKeyInput& key) const {
  const auto it = raw_key_to_ids_.find(key);

  return (it != raw_key_to_ids_.end()) ? it->second : kEmptyIds;
}

const InputIds& InputMan::fetch_ids(const SymKeyInput& key) const {
  const auto it = sym_key_to_ids_.find(key);

  return (it != sym_key_to_ids_.end()) ? it->second : kEmptyIds;
}

const InputIds& InputMan::fetch_ids(JoypadInput input) const {
  const auto it = joypad_input_to_ids_.find(input);

  return (it != joypad_input_to_ids_.end()) ? it->second : kEmptyIds;
}

const std::vector<bool>& InputMan::states() const { return id_to_state_; }

InputMan::InputMapper::InputMapper(InputMan& input_man,int id)
  : input_man_(input_man),id_(id) {}

void InputMan::InputMapper::raw_key(std::initializer_list<RawKey> keys,KeyMods mods) {
  for(const auto& key : keys) {
    input_man_.raw_key_to_ids_[RawKeyInput{key,mods}].insert(id_);
  }
}

void InputMan::InputMapper::raw_key(std::initializer_list<RawKeyInput> keys) {
  for(const auto& key : keys) {
    input_man_.raw_key_to_ids_[key].insert(id_);
  }
}

void InputMan::InputMapper::sym_key(std::initializer_list<SymKey> keys,KeyMods mods) {
  for(const auto& key : keys) {
    input_man_.sym_key_to_ids_[SymKeyInput{key,mods}].insert(id_);
  }
}

void InputMan::InputMapper::sym_key(std::initializer_list<SymKeyInput> keys) {
  for(const auto& key : keys) {
    input_man_.sym_key_to_ids_[key].insert(id_);
  }
}

void InputMan::InputMapper::joypad(std::initializer_list<JoypadInput> inputs) {
  for(const auto input : inputs) {
    input_man_.joypad_input_to_ids_[input].insert(id_);
  }
}

} // namespace cybel
