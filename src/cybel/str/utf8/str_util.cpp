/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "str_util.h"

#include "cybel/str/utf8/rune_iterator.h"
#include "cybel/str/utf8/rune_util.h"

namespace cybel::utf8 {

int StrUtil::casecmp_ascii(std::string_view str1,std::string_view str2) {
  const std::size_t len1 = str1.size();
  const std::size_t len2 = str2.size();
  const std::size_t min_len = std::min(len1,len2);

  for(std::size_t i = 0; i < min_len; ++i) {
    // `int` because of std::tolower().
    int c1 = static_cast<unsigned char>(str1[i]);
    int c2 = static_cast<unsigned char>(str2[i]);

    if(c1 <= RuneUtil::kMaxAsciiOctet) { c1 = std::tolower(c1); }
    if(c2 <= RuneUtil::kMaxAsciiOctet) { c2 = std::tolower(c2); }

    if(c1 < c2) { return -1; }
    if(c1 > c2) { return 1; }
  }

  if(len1 < len2) { return -1; }
  if(len1 > len2) { return 1; }
  return 0;
}

std::size_t StrUtil::count_runes(std::string_view str) {
  if(str.empty()) { return 0; }

  std::size_t rune_count = 0;
  std::uint8_t byte_count = 0;

  for(std::size_t i = 0; i < str.size(); i += byte_count,++rune_count) {
    RuneIterator::next_rune(str,i,byte_count);

    if(byte_count == 0) { break; }
  }

  return rune_count;
}

std::string StrUtil::ellipsize(std::string_view str,std::size_t max_len) {
  if(max_len == 0 || str.empty()) { return ""; }
  if(str.size() <= max_len) { return std::string{str}; }

  auto len = count_runes(str);

  if(len <= max_len) { return std::string{str}; }

  --max_len; // Make room for ellipsis at end.

  std::size_t last_index = str.size() - 1;
  std::uint8_t byte_count = 0;

  for(; len > max_len; --len) {
    RuneIterator::prev_rune(str,last_index,byte_count);

    if(byte_count == 0) { break; }

    if(byte_count < last_index) {
      last_index -= byte_count;
    } else {
      // Show the one little char left w/o ellipsis.
      return std::string{str.substr(0,last_index + 1)};
    }
  }

  return std::string{str.substr(0,last_index + 1)} + "…";
}

std::string StrUtil::pad(std::string_view str,std::size_t new_len) {
  if(new_len == 0 || (new_len == 1 && !str.empty())) { return std::string{str}; }

  const auto len = count_runes(str);

  if(len >= new_len) { return std::string{str}; }

  return std::string{str} + std::string(new_len - len,' ');
}

std::string StrUtil::strip(std::string_view str) {
  if(str.empty()) { return std::string{str}; }

  // lstrip().
  std::size_t first_index = 0;
  std::uint8_t byte_count = 0;

  for(; first_index < str.size(); first_index += byte_count) {
    auto rune = RuneIterator::next_rune(str,first_index,byte_count);

    if(byte_count == 0 || !RuneUtil::is_whitespace(rune)) { break; }
  }

  if(first_index >= str.size()) { return ""; }

  // rstrip().
  std::size_t last_pos = str.size();

  while(last_pos > first_index) {
    auto rune = RuneIterator::prev_rune(str,last_pos - 1,byte_count);

    if(byte_count == 0 || !RuneUtil::is_whitespace(rune)) { break; }

    if(byte_count < last_pos) {
      last_pos -= byte_count;
    } else {
      last_pos = 0;
      break;
    }
  }

  // Shouldn't happen, technically.
  if(last_pos < first_index) { return ""; }

  return std::string{str.substr(first_index,last_pos - first_index)};
}

} // Namespace.
