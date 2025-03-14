/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_STR_UTF8_RUNE_RANGE_H_
#define CYBEL_STR_UTF8_RUNE_RANGE_H_

#include "cybel/common.h"

#include "cybel/str/utf8/rune_iterator.h"

#include <ranges>

namespace cybel::utf8 {

class RuneRange : public std::ranges::view_interface<RuneRange> {
public:
  using const_iterator = RuneIterator;
  using const_reverse_iterator = RuneIterator::reverse_iterator;

  // NOTE: Necessary for std::views::reverse().
  explicit RuneRange() = default;
  explicit RuneRange(std::string_view str,std::size_t begin_rune_count = 0,std::size_t end_rune_count = 0);

  RuneIterator begin() const;
  RuneIterator end() const;
  RuneIterator cbegin() const;
  RuneIterator cend() const;

  RuneIterator::reverse_iterator rbegin() const;
  RuneIterator::reverse_iterator rend() const;
  RuneIterator::reverse_iterator crbegin() const;
  RuneIterator::reverse_iterator crend() const;

private:
  std::string_view str_{};
  std::size_t begin_rune_count_ = 0;
  std::size_t end_rune_count_ = 0;
};

} // namespace cybel::utf8
#endif
