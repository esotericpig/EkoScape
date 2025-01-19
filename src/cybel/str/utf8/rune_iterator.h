/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_STR_UTF8_RUNE_ITERATOR_H_
#define CYBEL_STR_UTF8_RUNE_ITERATOR_H_

#include "cybel/common.h"

#include "cybel/str/utf8/rune_util.h"

namespace cybel::utf8 {

/**
 * UTF-8 links:
 * - https://datatracker.ietf.org/doc/html/rfc3629#autoid-3
 * - https://datatracker.ietf.org/doc/html/rfc3629#autoid-4
 * - https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G31703
 * - https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G48534
 */
class RuneIterator {
public:
  static char32_t next_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);
  static char32_t prev_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);

  static RuneIterator begin(std::string_view str,std::size_t index = 0);
  static RuneIterator end(std::string_view str,std::size_t relative_count = 0);

  bool operator!=(const RuneIterator& other) const;
  bool operator==(const RuneIterator& other) const;
  std::strong_ordering operator<=>(const RuneIterator& other) const;

  char32_t operator*() const;

  RuneIterator& operator++();
  RuneIterator operator++(int);
  RuneIterator& operator--();
  RuneIterator operator--(int);

  std::string_view str() const;
  std::size_t index() const;

  std::uint8_t byte_count() const;
  char32_t rune() const;

private:
  std::string_view str_{};
  std::size_t index_ = 0;

  std::uint8_t byte_count_ = 0;
  char32_t rune_ = RuneUtil::kInvalidRune;

  static char32_t unpack_seq2(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                              unsigned char octet1);
  static char32_t unpack_seq3(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                              unsigned char octet1);
  static char32_t unpack_seq4(std::string_view str,std::size_t index,std::uint8_t& byte_count,
                              unsigned char octet1);

  explicit RuneIterator(std::string_view str,std::size_t index);

  void next_rune();
  void prev_rune();
};

} // Namespace.
#endif
