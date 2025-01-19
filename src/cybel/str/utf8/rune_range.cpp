/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rune_range.h"

namespace cybel::utf8 {

RuneRange::RuneRange(std::string_view str,std::size_t index)
    : str_(str),index_(index) {}

RuneIterator RuneRange::begin() const { return RuneIterator::begin(str_,index_); }
RuneIterator RuneRange::end() const { return RuneIterator::end(str_); }

RuneIterator RuneRange::cbegin() const { return begin(); }
RuneIterator RuneRange::cend() const { return end(); }

} // Namespace.
