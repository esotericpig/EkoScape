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

namespace cybel::utf8 {

class RuneRange {
public:
  explicit RuneRange(std::string_view str,std::size_t index = 0);

  RuneIterator begin() const;
  RuneIterator end() const;

  RuneIterator cbegin() const;
  RuneIterator cend() const;

private:
  std::string_view str_{};
  std::size_t index_ = 0;
};

} // Namespace.
#endif
