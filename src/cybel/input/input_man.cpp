/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "input_man.h"

#include "cybel/types/cybel_error.h"

namespace cybel {

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

InputMan::InputMan(int max_id)
  : max_id_(max_id),id_to_state_((max_id > 0) ? (max_id + 1) : 25,false) {
  // SDL_INIT_GAMECONTROLLER also auto-initializes SDL_INIT_JOYSTICK.
  if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
    // Don't fail, since game controllers are optional.
    std::cerr << "[WARN] Failed to init SDL_INIT_GAMECONTROLLER: " << Util::get_sdl_error() << '.'
              << std::endl;
  }
}

void InputMan::map_input(int id,const MapInputCallback& callback) {
  if(id < 0) { throw CybelError{"Invalid input ID [",id,"] is < 0."}; }

  if(max_id_ <= 0 && id >= static_cast<int>(id_to_state_.size())) {
    const auto new_size = std::max(
      static_cast<std::size_t>(id) + 1,
      (id_to_state_.size() + 1) << 1
    );
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

void InputMan::reset_states() {
  // Can't use std::ranges::fill() because std::vector<bool> is a specialized container
  //     that doesn't meet the requirements for Ranges.
  std::fill(id_to_state_.begin(),id_to_state_.end(),false);
}

void InputMan::update_states() {
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

void InputMan::handle_event(const SDL_Event& event,const OnInputEvent& on_input_event) {
  switch(event.type) {
    case SDL_KEYDOWN:
      handle_key_down_event(event,on_input_event);
      break;
  }
}

void InputMan::handle_key_down_event(const SDL_Event& event,const OnInputEvent& on_input_event) {
  const RawKeyInput raw_key{event.key.keysym.scancode,event.key.keysym.mod};
  const SymKeyInput sym_key{event.key.keysym.sym,event.key.keysym.mod};

  std::unordered_set<int> processed_ids{};

  for(auto id : fetch_ids(raw_key)) {
    // Not inserted? (already processed)
    if(!processed_ids.insert(id).second) { continue; }

    on_input_event(id);
  }
  for(auto id : fetch_ids(sym_key)) {
    // Not inserted? (already processed)
    if(!processed_ids.insert(id).second) { continue; }

    on_input_event(id);
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

const InputIds& InputMan::fetch_ids(const RawKeyInput& key) {
  const auto it = raw_key_to_ids_.find(key);

  return (it != raw_key_to_ids_.end()) ? it->second : kEmptyIds;
}

const InputIds& InputMan::fetch_ids(const SymKeyInput& key) {
  const auto it = sym_key_to_ids_.find(key);

  return (it != sym_key_to_ids_.end()) ? it->second : kEmptyIds;
}

const std::vector<bool>& InputMan::states() const { return id_to_state_; }

} // namespace cybel
