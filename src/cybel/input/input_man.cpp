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
  : max_id_(max_id),
    id_to_state_((max_id > 0) ? (max_id + 1) : 25,false),
    id_to_event_state_(id_to_state_.size(),false),
    touch_input_to_state_(static_cast<std::size_t>(JoypadInput::kMax),false) {
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
  if(!is_joypad_alive_ || (main_game_ctrl_ || main_joystick_)) { return; }

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
    id_to_event_state_.resize(new_size,false);
  }
  // Not `else if`, in case of casting overflow.
  if(id >= static_cast<int>(id_to_state_.size())) {
    throw CybelError{"Invalid input ID [",id,"] is >= maximum ID count [",
                     static_cast<int>(id_to_state_.size()),'/',id_to_state_.size(),"]."};
  }

  InputMapper mapper{*this,id};
  callback(mapper);
}

void InputMan::use_fake_joypad(bool use_game_ctrl,FakeJoypadInputType input_type) {
  const auto id = SDL_JoystickAttachVirtual(
    use_game_ctrl ? SDL_JOYSTICK_TYPE_GAMECONTROLLER : SDL_JOYSTICK_TYPE_ARCADE_PAD,
    SDL_CONTROLLER_AXIS_MAX,
    SDL_CONTROLLER_BUTTON_MAX,
    1
  );

  if(id == -1) {
    std::cerr << "[WARN] Failed to create fake joypad: " << Util::get_sdl_error() << '.' << std::endl;
    return;
  }

  is_fake_joypad_ = true;
  is_fake_joypad_game_ctrl_ = use_game_ctrl;
  fake_joypad_input_type_ = input_type;

  if(is_fake_joypad_game_ctrl_) {
    std::cout << "[INFO] Using fake game controller." << std::endl;

    if(fake_joypad_input_type_ == FakeJoypadInputType::kHat) {
      std::cerr << "[WARN] Fake game controller input type is set to hat, which won't work." << std::endl;
    }
  } else {
    std::cout << "[INFO] Using fake joystick." << std::endl;

    if(fake_joypad_input_type_ == FakeJoypadInputType::kDpad) {
      std::cerr << "[WARN] Fake joystick input type is set to dpad, which won't work." << std::endl;
    }
  }
}

void InputMan::use_mouse_as_finger() {
  SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS,"1");
}

void InputMan::begin_input() {
  processed_ids_.clear();
}

void InputMan::handle_event(const SDL_Event& event,const OnInputEvent& on_input_event) {
  on_input_event_ = on_input_event;

  if(is_fake_joypad_ && emit_fake_joypad_events(event)) { return; }

  switch(event.type) {
    case SDL_KEYDOWN:
      handle_key_down_event(event.key);
      break;

    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:
      handle_joystick_device_event(event.jdevice);
      break;
    case SDL_JOYAXISMOTION:
      handle_joystick_axis_event(event.jaxis);
      break;
    case SDL_JOYHATMOTION:
      handle_joystick_hat_event(event.jhat);
      break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
      handle_joystick_button_event(event.jbutton);
      break;

    case SDL_CONTROLLERDEVICEADDED:
    case SDL_CONTROLLERDEVICEREMOVED:
      handle_game_ctrl_device_event(event.cdevice);
      break;
    case SDL_CONTROLLERAXISMOTION:
      handle_game_ctrl_axis_event(event.caxis);
      break;
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
      handle_game_ctrl_button_event(event.cbutton);
      break;

    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
    //case SDL_FINGERMOTION: // Can't currently handle correctly.
      handle_finger_event(event.tfinger);
      break;

    default: break;
  }
}

void InputMan::handle_key_down_event(const SDL_KeyboardEvent& key) {
  const RawKeyInput raw_key{key.keysym.scancode,key.keysym.mod};
  const SymKeyInput sym_key{key.keysym.sym,key.keysym.mod};

  for(auto id : fetch_ids(raw_key)) {
    // Not inserted? (already processed)
    if(!processed_ids_.insert(id).second) { continue; }

    on_input_event_(id);
  }
  for(auto id : fetch_ids(sym_key)) {
    // Not inserted? (already processed)
    if(!processed_ids_.insert(id).second) { continue; }

    on_input_event_(id);
  }
}

void InputMan::handle_joystick_device_event(const SDL_JoyDeviceEvent& jdevice) {
  if(!is_joypad_alive_) { return; }

  if(jdevice.type == SDL_JOYDEVICEADDED) {
    // Game controllers also emit this event.
    if(main_game_ctrl_.matches(jdevice.which)) { return; }

    main_joystick_.open(jdevice.which);

    if(main_joystick_) {
      std::cout << "[INFO] Added joystick: " << jdevice.which << '.' << std::endl;

      main_game_ctrl_.close();
      reset_joypad_states();
    }
  } else if(jdevice.type == SDL_JOYDEVICEREMOVED) {
    if(!main_joystick_.matches(jdevice.which)) { return; }

    main_joystick_.close();
    std::cout << "[INFO] Removed joystick: " << jdevice.which << '.' << std::endl;

    if(!main_game_ctrl_) {
      reset_joypad_states();
      load_joypads(); // Try to find another one.
    }
  }
}

void InputMan::handle_joystick_axis_event(const SDL_JoyAxisEvent& jaxis) {
  if(!main_joystick_.matches(jaxis.which)) { return; }

  switch(jaxis.axis) {
    // X-axis.
    case 0: return handle_joypad_axis_event(SDL_CONTROLLER_AXIS_LEFTX,jaxis.value);
    // Y-axis.
    case 1: return handle_joypad_axis_event(SDL_CONTROLLER_AXIS_LEFTY,jaxis.value);

    default: break;
  }
}

void InputMan::handle_joystick_hat_event(const SDL_JoyHatEvent& jhat) {
  if(!main_joystick_.matches(jhat.which)) { return; }

  const auto value = jhat.value;

  handle_joypad_event(JoypadInput::kUp,value & SDL_HAT_UP);
  handle_joypad_event(JoypadInput::kDown,value & SDL_HAT_DOWN);
  handle_joypad_event(JoypadInput::kLeft,value & SDL_HAT_LEFT);
  handle_joypad_event(JoypadInput::kRight,value & SDL_HAT_RIGHT);
}

void InputMan::handle_joystick_button_event(const SDL_JoyButtonEvent& jbutton) {
  if(!main_joystick_.matches(jbutton.which)) { return; }

  // Should be the same as `jbutton.type == SDL_JOYBUTTONDOWN`.
  const bool is_pressed = (jbutton.state == SDL_PRESSED);

  switch(jbutton.button) {
    case 0: return handle_joypad_event(JoypadInput::kA,is_pressed);
    case 1: return handle_joypad_event(JoypadInput::kB,is_pressed);

    default: break;
  }
}

void InputMan::handle_game_ctrl_device_event(const SDL_ControllerDeviceEvent& cdevice) {
  if(!is_joypad_alive_) { return; }

  if(cdevice.type == SDL_CONTROLLERDEVICEADDED) {
    main_game_ctrl_.open(cdevice.which);

    if(main_game_ctrl_) {
      std::cout << "[INFO] Added game controller: " << cdevice.which << '.' << std::endl;

      main_joystick_.close();
      reset_joypad_states();
    }
  } else if(cdevice.type == SDL_CONTROLLERDEVICEREMOVED) {
    if(!main_game_ctrl_.matches(cdevice.which)) { return; }

    main_game_ctrl_.close();
    std::cout << "[INFO] Removed game controller: " << cdevice.which << '.' << std::endl;

    if(!main_joystick_) {
      reset_joypad_states();
      load_joypads(); // Try to find another one.
    }
  }
}

void InputMan::handle_game_ctrl_axis_event(const SDL_ControllerAxisEvent& caxis) {
  if(!main_game_ctrl_.matches(caxis.which)) { return; }

  handle_joypad_axis_event(static_cast<SDL_GameControllerAxis>(caxis.axis),caxis.value);
}

void InputMan::handle_game_ctrl_button_event(const SDL_ControllerButtonEvent& cbutton) {
  if(!main_game_ctrl_.matches(cbutton.which)) { return; }

  // Should be the same as `cbutton.type == SDL_CONTROLLERBUTTONDOWN`.
  const bool is_pressed = (cbutton.state == SDL_PRESSED);

  switch(cbutton.button) {
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return handle_joypad_event(JoypadInput::kUp,is_pressed);
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return handle_joypad_event(JoypadInput::kDown,is_pressed);
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return handle_joypad_event(JoypadInput::kLeft,is_pressed);
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return handle_joypad_event(JoypadInput::kRight,is_pressed);
    case SDL_CONTROLLER_BUTTON_A: return handle_joypad_event(JoypadInput::kA,is_pressed);
    case SDL_CONTROLLER_BUTTON_B: return handle_joypad_event(JoypadInput::kB,is_pressed);

    default: break;
  }
}

void InputMan::handle_joypad_axis_event(SDL_GameControllerAxis axis,Sint16 value) {
  const bool is_live_zone = std::abs(value) > kJoypadAxisDeadZone;
  const bool is_live_neg = is_live_zone && value < 0;
  const bool is_live_pos = is_live_zone && value > 0;

  switch(axis) {
    // X-axis.
    case SDL_CONTROLLER_AXIS_LEFTX:
    case SDL_CONTROLLER_AXIS_RIGHTX:
      handle_joypad_event(JoypadInput::kLeft,is_live_neg);
      handle_joypad_event(JoypadInput::kRight,is_live_pos);
      break;

    // Y-axis.
    case SDL_CONTROLLER_AXIS_LEFTY:
    case SDL_CONTROLLER_AXIS_RIGHTY:
      handle_joypad_event(JoypadInput::kUp,is_live_neg);
      handle_joypad_event(JoypadInput::kDown,is_live_pos);
      break;

    default: break;
  }
}

void InputMan::handle_joypad_event(JoypadInput input,bool state) {
  if(input <= JoypadInput::kNone || input >= JoypadInput::kMax) { return; }

  set_state(input,state);

  if(!state) { return; } // Don't emit event.

  for(auto id : fetch_ids(input)) {
    // Not inserted? (already processed)
    if(!processed_ids_.insert(id).second) { continue; }

    on_input_event_(id);
  }
}

bool InputMan::emit_fake_joypad_events(const SDL_Event& event) {
  if(!(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)) { return false; }

  SDL_Joystick* joystick = NULL;

  if(is_fake_joypad_game_ctrl_) {
    if(main_game_ctrl_) {
      joystick = SDL_GameControllerGetJoystick(main_game_ctrl_.object());
    }
  } else if(main_joystick_) {
    joystick = main_joystick_.object();
  }

  if(joystick == NULL) { return false; }

  const bool is_pressed = (event.type == SDL_KEYDOWN);

  int axis = -1;
  Sint16 axis_value = is_pressed ? (kJoypadAxisDeadZone + 1) : 0;
  const Uint8 old_hat_value = SDL_JoystickGetHat(joystick,0);
  Uint8 hat_value = 0;
  int dpad = -1;
  int button = -1;
  const Uint8 button_value = is_pressed ? SDL_PRESSED : SDL_RELEASED;

  switch(event.key.keysym.scancode) {
    // Up.
    case SDL_SCANCODE_KP_8:
      axis = is_fake_joypad_game_ctrl_ ? SDL_CONTROLLER_AXIS_LEFTY : 1;
      axis_value = -axis_value;
      hat_value = SDL_HAT_UP;
      dpad = SDL_CONTROLLER_BUTTON_DPAD_UP;
      break;

    // Down.
    case SDL_SCANCODE_KP_2:
      axis = is_fake_joypad_game_ctrl_ ? SDL_CONTROLLER_AXIS_LEFTY : 1;
      hat_value = SDL_HAT_DOWN;
      dpad = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
      break;

    // Left.
    case SDL_SCANCODE_KP_4:
      axis = is_fake_joypad_game_ctrl_ ? SDL_CONTROLLER_AXIS_LEFTX : 0;
      axis_value = -axis_value;
      hat_value = SDL_HAT_LEFT;
      dpad = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
      break;

    // Right.
    case SDL_SCANCODE_KP_6:
      axis = is_fake_joypad_game_ctrl_ ? SDL_CONTROLLER_AXIS_LEFTX : 0;
      hat_value = SDL_HAT_RIGHT;
      dpad = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
      break;

    // Button A.
    case SDL_SCANCODE_KP_5:
      dpad = SDL_CONTROLLER_BUTTON_A;
      button = 0;
      break;

    // Button B.
    case SDL_SCANCODE_KP_0:
      dpad = SDL_CONTROLLER_BUTTON_B;
      button = 1;
      break;

    default: break;
  }

  switch(fake_joypad_input_type_) {
    case FakeJoypadInputType::kAxis:
      if(axis != -1) {
        std::cout << "[INFO] Faking joypad axis event: " << axis << ',' << axis_value << '.' << std::endl;
        SDL_JoystickSetVirtualAxis(joystick,axis,axis_value);
        SDL_JoystickUpdate();
        return true;
      }
      break;

    case FakeJoypadInputType::kHat:
      if(hat_value != 0) {
        hat_value = is_pressed ? (old_hat_value | hat_value) : (old_hat_value & ~hat_value);

        std::cout << "[INFO] Faking joypad hat event: " << static_cast<int>(hat_value) << '.' << std::endl;
        SDL_JoystickSetVirtualHat(joystick,0,hat_value);
        SDL_JoystickUpdate();
        return true;
      }
      break;

    case FakeJoypadInputType::kDpad:
      if(dpad != -1) {
        std::cout << "[INFO] Faking joypad dpad event: " << dpad << ',' << static_cast<int>(button_value)
                  << '.' << std::endl;
        SDL_JoystickSetVirtualButton(joystick,dpad,button_value);
        SDL_JoystickUpdate();
        return true;
      }
      break;
  }

  // NOTE: Must process dpad before button, else button will eat dpad events.
  if(button != -1) {
    std::cout << "[INFO] Faking joypad button event: " << button << ',' << static_cast<int>(button_value)
              << '.' << std::endl;
    SDL_JoystickSetVirtualButton(joystick,button,button_value);
    SDL_JoystickUpdate();
    return true;
  }

  return false;
}

void InputMan::handle_finger_event(const SDL_TouchFingerEvent& tfinger) {
  static constexpr float kCenterMin = 1.0f / 3.0f;
  static constexpr float kCenterMax = 1.0f - kCenterMin;

  const bool is_pressed = (tfinger.type == SDL_FINGERDOWN || tfinger.type == SDL_FINGERMOTION);
  const float x = tfinger.x;
  const float y = tfinger.y;

  // Center?
  if((x >= kCenterMin && x <= kCenterMax) &&
     (y >= kCenterMin && y <= kCenterMax)) {
    handle_touch_event(JoypadInput::kA,is_pressed);
  } else if(x < kCenterMin) {
    // If holding left & right, emit down.
    if(is_pressed && touch_input_to_state_[static_cast<std::size_t>(JoypadInput::kRight)]) {
      handle_touch_event(JoypadInput::kLeft,false);
      handle_touch_event(JoypadInput::kRight,false);
      handle_touch_event(JoypadInput::kDown,is_pressed);
    } else {
      handle_touch_event(JoypadInput::kLeft,is_pressed);
      handle_touch_event(JoypadInput::kDown,false);
    }
  } else if(x > kCenterMax) {
    // If holding left & right, emit down.
    if(is_pressed && touch_input_to_state_[static_cast<std::size_t>(JoypadInput::kLeft)]) {
      handle_touch_event(JoypadInput::kLeft,false);
      handle_touch_event(JoypadInput::kRight,false);
      handle_touch_event(JoypadInput::kDown,is_pressed);
    } else {
      handle_touch_event(JoypadInput::kRight,is_pressed);
      handle_touch_event(JoypadInput::kDown,false);
    }
  } else if(y < kCenterMin) {
    handle_touch_event(JoypadInput::kUp,is_pressed);
  } else if(y > kCenterMax) {
    handle_touch_event(JoypadInput::kDown,is_pressed);
  } else {
    // Shouldn't happen technically.
    handle_touch_event(JoypadInput::kUp,false);
    handle_touch_event(JoypadInput::kDown,false);
    handle_touch_event(JoypadInput::kLeft,false);
    handle_touch_event(JoypadInput::kRight,false);
    handle_touch_event(JoypadInput::kA,false);
  }
}

void InputMan::handle_touch_event(JoypadInput input,bool state) {
  if(input <= JoypadInput::kNone || input >= JoypadInput::kMax) { return; }

  touch_input_to_state_[static_cast<std::size_t>(input)] = state;
  set_state(input,state);

  if(!state) { return; } // Don't emit event.

  for(auto id : fetch_ids(input)) {
    // Not inserted? (already processed)
    if(!processed_ids_.insert(id).second) { continue; }

    on_input_event_(id);
  }
}

void InputMan::update_states() {
  id_to_state_ = id_to_event_state_;

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

void InputMan::reset_joypad_states() {
  static constexpr auto kMaxJoypadInputValue = static_cast<JoypadInputT>(JoypadInput::kMax);

  JoypadInputT input_value = static_cast<JoypadInputT>(JoypadInput::kNone) + 1;

  for(; input_value < kMaxJoypadInputValue; ++input_value) {
    set_state(static_cast<JoypadInput>(input_value),false);
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
  const auto it = joypad_input_to_ids_.find(input);
  if(it == joypad_input_to_ids_.end()) { return; }

  for(const auto id : it->second) {
    id_to_event_state_[id] = state;
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
