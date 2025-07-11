/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "input_man.h"

#include "cybel/types/cybel_error.h"

namespace cybel {

InputMan::InputMan(input_id_t max_id)
  : max_id_(max_id),
    id_to_state_((max_id > 0) ? (max_id + 1) : 32,false),
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

void InputMan::map_input(input_id_t id,const MapInputCallback& callback) {
  if(id >= id_to_state_.size()) {
    // Growable?
    if(max_id_ <= 0) {
      const auto new_size = std::max<std::size_t>({id,id_to_state_.size(),1}) << 1;
      id_to_state_.resize(new_size,false);
    }

    if(id >= id_to_state_.size()) {
      throw CybelError{"Invalid input ID [",id,"] is >= maximum ID count [",id_to_state_.size(),"]."};
    }
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
    case SDL_KEYUP:
      handle_key_event(event.key);
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
    //case SDL_FINGERMOTION: // TODO: Touch drag is not currently handled.
      handle_finger_event(event.tfinger);
      break;

    default: break;
  }
}

void InputMan::handle_key_event(const SDL_KeyboardEvent& key) {
  // Should be the same as `key.type == SDL_KEYDOWN`.
  const bool is_pressed = (key.state == SDL_PRESSED);

  RawKeyInput raw_key{key.keysym.mod,key.keysym.scancode};
  SymKeyInput sym_key{key.keysym.mod,static_cast<SymKey>(key.keysym.sym)};

  for(int i = 0; i < 2; ++i) {
    set_state(raw_key,is_pressed);
    set_state(sym_key,is_pressed);

    if(is_pressed) {
      for(auto id : fetch_ids(raw_key)) {
        // Inserted? (not already processed)
        if(processed_ids_.insert(id).second) { on_input_event_(id); }
      }
      for(auto id : fetch_ids(sym_key)) {
        // Inserted? (not already processed)
        if(processed_ids_.insert(id).second) { on_input_event_(id); }
      }
    }

    const auto norm_dup_mods = KeyMods::norm_dup_mods(raw_key.mods());

    // No duplicate modifier keys?
    if(raw_key.mods() == norm_dup_mods) { break; }

    raw_key = RawKeyInput{norm_dup_mods,raw_key.key()};
    sym_key = SymKeyInput{norm_dup_mods,sym_key.key()};
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
      reset_states();
    }
  } else if(jdevice.type == SDL_JOYDEVICEREMOVED) {
    if(!main_joystick_.matches(jdevice.which)) { return; }

    main_joystick_.close();
    reset_states();
    std::cout << "[INFO] Removed joystick: " << jdevice.which << '.' << std::endl;

    if(!main_game_ctrl_) {
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
      reset_states();
    }
  } else if(cdevice.type == SDL_CONTROLLERDEVICEREMOVED) {
    if(!main_game_ctrl_.matches(cdevice.which)) { return; }

    main_game_ctrl_.close();
    reset_states();
    std::cout << "[INFO] Removed game controller: " << cdevice.which << '.' << std::endl;

    if(!main_joystick_) {
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
      joystick = SDL_GameControllerGetJoystick(main_game_ctrl_.handle());
    }
  } else if(main_joystick_) {
    joystick = main_joystick_.handle();
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
      axis_value = static_cast<Sint16>(-axis_value);
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
      axis_value = static_cast<Sint16>(-axis_value);
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

  const bool is_pressed = (tfinger.type == SDL_FINGERDOWN);
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
  } else { // Shouldn't happen technically.
    reset_touch_states();
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

void InputMan::reset_states() {
  std::fill(id_to_state_.begin(),id_to_state_.end(),false);
}

void InputMan::reset_touch_states() {
  std::fill(touch_input_to_state_.begin(),touch_input_to_state_.end(),false);
  reset_states();
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

InputMan::InputMapper::InputMapper(InputMan& input_man,input_id_t id)
  : input_man_(input_man),id_(id) {}

void InputMan::InputMapper::raw_key(std::initializer_list<RawKey> keys,key_mods_t mods) {
  for(const auto& key : keys) {
    input_man_.raw_key_to_ids_[RawKeyInput{mods,key}].insert(id_);
  }
}

void InputMan::InputMapper::raw_key(std::initializer_list<std::pair<key_mods_t,RawKey>> keys) {
  for(const auto& [mods,key] : keys) {
    input_man_.raw_key_to_ids_[RawKeyInput{mods,key}].insert(id_);
  }
}

void InputMan::InputMapper::sym_key(std::initializer_list<SymKey> keys,key_mods_t mods) {
  for(const auto& key : keys) {
    input_man_.sym_key_to_ids_[SymKeyInput{mods,key}].insert(id_);
  }
}

void InputMan::InputMapper::sym_key(std::initializer_list<std::pair<key_mods_t,SymKey>> keys) {
  for(const auto& [mods,key] : keys) {
    input_man_.sym_key_to_ids_[SymKeyInput{mods,key}].insert(id_);
  }
}

void InputMan::InputMapper::joypad(std::initializer_list<JoypadInput> inputs) {
  for(const auto input : inputs) {
    input_man_.joypad_input_to_ids_[input].insert(id_);
  }
}

} // namespace cybel
