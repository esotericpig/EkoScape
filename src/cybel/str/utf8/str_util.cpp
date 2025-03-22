/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "str_util.h"

#include "cybel/str/utf8/rune_iterator.h"
#include "cybel/str/utf8/rune_util.h"

#include <sstream>

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
    RuneUtil::next_rune(str,i,byte_count);
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
    RuneUtil::prev_rune(str,last_index,byte_count);

    if(byte_count < last_index) {
      last_index -= byte_count;
    } else {
      // Show the one little rune left w/o ellipsis.
      return std::string{str.substr(0,last_index + 1)};
    }
  }

  return std::string{str.substr(0,last_index + 1)} + "…";
}

std::string StrUtil::ljust(std::string_view str,std::size_t new_len) {
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
    const auto rune = RuneUtil::next_rune(str,first_index,byte_count);

    if(!RuneUtil::is_whitespace(rune)) { break; }
  }

  if(first_index >= str.size()) { return ""; }

  // rstrip().
  std::size_t last_pos = str.size();
  byte_count = 0;

  while(last_pos > first_index) {
    const auto rune = RuneUtil::prev_rune(str,last_pos - 1,byte_count);

    if(!RuneUtil::is_whitespace(rune)) { break; }

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

std::string StrUtil::wrap_words(std::string_view str,std::size_t max_len) {
  if(max_len == 0) { max_len = 1; }
  if(str.size() <= max_len) { return std::string{str}; }

  std::ostringstream result{};
  const auto it_end = RuneIterator::end(str);
  char32_t rune = 0;

  bool is_line_start = true;
  std::size_t line_len = 0;
  std::size_t word_i = 0;
  std::size_t word_len = 0;
  std::string_view word{};

  // Bunch of funky logic here, just to avoid calling next_rune() more than necessary.
  for(auto it = RuneIterator::begin(str); it != it_end;) {
    // First, read/eat all spaces.
    do {
      rune = it.rune();

      if(rune == '\n' || rune == '\r') { goto handle_newlines; }
      if(!RuneUtil::is_whitespace(rune)) { break; }

      if(is_line_start) {
        result << it.substr_view();
        ++line_len;
      }
    } while((++it) != it_end);

    if(it == it_end) { break; }

    // Second, gather word.
    word_i = it.index();
    word_len = 1;

    for(++it; it != it_end; ++it) {
      rune = it.rune();

      if(rune == '\n' || rune == '\r') { break; } // Append word.
      if(RuneUtil::is_whitespace(rune)) { break; }

      ++word_len;
    }

    word = str.substr(word_i,it.index() - word_i);

    // Third, append word appropriately.
    if(is_line_start) {
      result << word;
      is_line_start = false;
      line_len += word_len;
    } else {
      line_len += 1 + word_len; // +1 for space between words.

      if(line_len <= max_len) {
        result << ' '; // Space between words, if not first word.
      } else {
        result << '\n';
        line_len = word_len;
      }

      result << word;
    }

    if(it == it_end) { break; }

    // Fourth, check for newlines.
  handle_newlines:
    while(rune == '\n' || rune == '\r') {
      // Check for CRLF.
      if(rune == '\r') {
        const auto peek_it = it + 1;

        if(peek_it != it_end && peek_it.rune() == '\n') {
          it = peek_it;
        }
      }

      result << '\n';
      is_line_start = true;
      line_len = 0;

      if((++it) == it_end) { break; }
      rune = it.rune();
    }
  }

  return result.str();
}

} // namespace cybel::utf8
