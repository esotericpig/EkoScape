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

#include <iterator>

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
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = char32_t;
  using difference_type = std::ptrdiff_t;
  using pointer = void;
  using reference = char32_t;
  using reverse_iterator = std::reverse_iterator<RuneIterator>;

  static char32_t next_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);
  static char32_t prev_rune(std::string_view str,std::size_t index,std::uint8_t& byte_count);

  static RuneIterator begin(std::string_view str,std::size_t next_rune_count = 0);
  static RuneIterator end(std::string_view str,std::size_t prev_rune_count = 0);

  static reverse_iterator rbegin(std::string_view str,std::size_t prev_rune_count = 0);
  static reverse_iterator rend(std::string_view str,std::size_t next_rune_count = 0);

  // NOTE: Necessary for std::views::reverse().
  explicit RuneIterator() = default;

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
  std::string packed_rune() const;

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

  explicit RuneIterator(std::string_view str,bool is_begin);

  void next_rune();
  void prev_rune();
};

} // namespace cybel::utf8
#endif
