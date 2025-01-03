/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_INPUT_INPUT_MAN_H_
#define CYBEL_INPUT_INPUT_MAN_H_

#include "cybel/common.h"

#include "cybel/types/cybel_error.h"
#include "cybel/types/key_input.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cybel {

using InputIds = std::unordered_set<int>;

class InputMan {
public:
  class Wrapper {
  public:
    explicit Wrapper(InputMan& input_man,int id);

    void raw_key(const std::initializer_list<RawKey>& keys,KeyMods mods = 0);
    void raw_key(const RawKeyInput& key);
    void raw_key(const std::initializer_list<RawKeyInput>& keys);

    void sym_key(const std::initializer_list<SymKey>& keys,KeyMods mods = 0);
    void sym_key(const SymKeyInput& key);
    void sym_key(const std::initializer_list<SymKeyInput>& keys);

  private:
    InputMan& input_man_;
    int id_{};
  };

  using WrapCallback = std::function<void(Wrapper&)>;

  static inline const InputIds kEmptyIds{};

  explicit InputMan(int max_id = 0);

  void map_input(int id,const WrapCallback& callback);

  void reset_states();
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
};

} // Namespace.
#endif
