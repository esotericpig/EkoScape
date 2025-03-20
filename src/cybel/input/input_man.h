/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_INPUT_MAN_H_
#define CYBEL_INPUT_INPUT_MAN_H_

#include "cybel/common.h"

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

  private:
    InputMan& input_man_;
    int id_{};
  };

  using MapInputCallback = std::function<void(InputMapper&)>;
  using OnInputEvent = std::function<void(int id)>;

  static inline const InputIds kEmptyIds{};

  explicit InputMan(int max_id = 0);

  void map_input(int id,const MapInputCallback& callback);

  void reset_states();
  void update_states();
  void handle_event(const SDL_Event& event,const OnInputEvent& on_input_event);

  void set_state(const RawKeyInput& key,bool state);
  void set_state(const SymKeyInput& key,bool state);

  const InputIds& fetch_ids(const RawKeyInput& key);
  const InputIds& fetch_ids(const SymKeyInput& key);
  const std::vector<bool>& states() const;

private:
  int max_id_{};
  std::vector<bool> id_to_state_{};

  std::unordered_map<RawKeyInput,InputIds,RawKeyInput::Hash> raw_key_to_ids_{};
  std::unordered_map<SymKeyInput,InputIds,SymKeyInput::Hash> sym_key_to_ids_{};

  void handle_key_down_event(const SDL_Event& event,const OnInputEvent& on_input_event);
};

} // namespace cybel
#endif
