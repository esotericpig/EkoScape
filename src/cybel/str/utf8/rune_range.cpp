/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rune_range.h"

namespace cybel::utf8 {

RuneRange::RuneRange(std::string_view str,std::size_t rune_count)
  : str_(str),rune_count_(rune_count) {}

RuneIterator RuneRange::begin() const { return RuneIterator::begin(str_,rune_count_); }

RuneIterator RuneRange::end() const { return RuneIterator::end(str_,rune_count_); }

RuneIterator RuneRange::cbegin() const { return begin(); }

RuneIterator RuneRange::cend() const { return end(); }

RuneIterator::reverse_iterator RuneRange::rbegin() const {
  return RuneIterator::rbegin(str_,rune_count_);
}

RuneIterator::reverse_iterator RuneRange::rend() const {
  return RuneIterator::rend(str_,rune_count_);
}

RuneIterator::reverse_iterator RuneRange::crbegin() const { return rbegin(); }

RuneIterator::reverse_iterator RuneRange::crend() const { return rend(); }

} // Namespace.
