/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "input_man.h"

#include "cybel/types/cybel_error.h"

namespace cybel {

InputMan::Wrapper::Wrapper(InputMan& input_man,int id)
    : input_man_(input_man),id_(id) {}

void InputMan::Wrapper::raw_key(const std::initializer_list<RawKey>& keys,KeyMods mods) {
  for(const auto& key: keys) {
    raw_key(RawKeyInput{key,mods});
  }
}

void InputMan::Wrapper::raw_key(const RawKeyInput& key) {
  input_man_.raw_key_to_ids_[key].insert(id_);
}

void InputMan::Wrapper::raw_key(const std::initializer_list<RawKeyInput>& keys) {
  for(const auto& key: keys) {
    raw_key(key);
  }
}

void InputMan::Wrapper::sym_key(const std::initializer_list<SymKey>& keys,KeyMods mods) {
  for(const auto& key: keys) {
    sym_key(SymKeyInput{key,mods});
  }
}

void InputMan::Wrapper::sym_key(const SymKeyInput& key) {
  input_man_.sym_key_to_ids_[key].insert(id_);
}

void InputMan::Wrapper::sym_key(const std::initializer_list<SymKeyInput>& keys) {
  for(const auto& key: keys) {
    sym_key(key);
  }
}

InputMan::InputMan(int max_id)
    : max_id_(max_id),id_to_state_((max_id > 0) ? (max_id + 1) : 25,false) {}

void InputMan::map_input(int id,const WrapCallback& callback) {
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
    throw CybelError{"Invalid input ID [",id,"] is >= maximum ID count ["
        ,static_cast<int>(id_to_state_.size()),',',id_to_state_.size(),"]."};
  }

  Wrapper wrapper{*this,id};
  callback(wrapper);
}

void InputMan::reset_states() {
  // Can't use std::ranges::fill() because std::vector<bool> is a specialized container
  //     that doesn't meet the requirements for Ranges.
  std::fill(id_to_state_.begin(),id_to_state_.end(),false);
}

void InputMan::set_state(const RawKeyInput& key,bool state) {
  auto it = raw_key_to_ids_.find(key);
  if(it == raw_key_to_ids_.end()) { return; }

  for(auto id: it->second) {
    id_to_state_[id] = state;
  }
}

void InputMan::set_state(const SymKeyInput& key,bool state) {
  auto it = sym_key_to_ids_.find(key);
  if(it == sym_key_to_ids_.end()) { return; }

  for(auto id: it->second) {
    id_to_state_[id] = state;
  }
}

const InputIds& InputMan::fetch_ids(const RawKeyInput& key) {
  auto it = raw_key_to_ids_.find(key);

  return (it != raw_key_to_ids_.end()) ? it->second : kEmptyIds;
}

const InputIds& InputMan::fetch_ids(const SymKeyInput& key) {
  auto it = sym_key_to_ids_.find(key);

  return (it != sym_key_to_ids_.end()) ? it->second : kEmptyIds;
}

const std::vector<bool>& InputMan::states() const { return id_to_state_; }

} // Namespace.
