/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rune_iterator.h"

namespace cybel::utf8 {

RuneIterator RuneIterator::begin(std::string_view str,std::size_t next_rune_count) {
  auto it = RuneIterator{str,true};

  for(; next_rune_count > 0; --next_rune_count) {
    it.next_rune();
  }

  return it;
}

RuneIterator RuneIterator::end(std::string_view str,std::size_t prev_rune_count) {
  auto it = RuneIterator{str,false};

  for(; prev_rune_count > 0; --prev_rune_count) {
    it.prev_rune();
  }

  return it;
}

RuneIterator::reverse_iterator RuneIterator::rbegin(std::string_view str,std::size_t prev_rune_count) {
  return reverse_iterator(end(str,prev_rune_count));
}

RuneIterator::reverse_iterator RuneIterator::rend(std::string_view str,std::size_t next_rune_count) {
  return reverse_iterator(begin(str,next_rune_count));
}

RuneIterator::RuneIterator(std::string_view str,bool is_begin)
  : str_(str),index_(is_begin ? 0 : str_.size()) {
  if(is_begin) {
    rune_ = RuneUtil::next_rune(str_,index_,byte_count_);
  }
}

bool RuneIterator::operator!=(const RuneIterator& other) const {
  return !(*this == other);
}

bool RuneIterator::operator==(const RuneIterator& other) const {
  return str_.data() == other.str_.data() &&
         str_.size() == other.str_.size() &&
         index_ == other.index_;
}

std::strong_ordering RuneIterator::operator<=>(const RuneIterator& other) const {
  auto order = str_.data() <=> other.str_.data();
  if(order != std::strong_ordering::equal) { return order; }

  order = str_.size() <=> other.str_.size();
  if(order != std::strong_ordering::equal) { return order; }

  return index_ <=> other.index_;
}

char32_t RuneIterator::operator*() const { return rune_; }

RuneIterator& RuneIterator::operator++() {
  next_rune();

  return *this;
}

RuneIterator RuneIterator::operator++(int) {
  const auto orig_it = *this;
  next_rune();

  return orig_it;
}

RuneIterator RuneIterator::operator+(std::size_t count) const {
  auto it = *this;

  for(; count > 0; --count) {
    it.next_rune();
  }

  return it;
}

RuneIterator& RuneIterator::operator+=(std::size_t count) {
  for(; count > 0; --count) {
    next_rune();
  }

  return *this;
}

RuneIterator& RuneIterator::operator--() {
  prev_rune();

  return *this;
}

RuneIterator RuneIterator::operator--(int) {
  const auto orig_it = *this;
  prev_rune();

  return orig_it;
}

RuneIterator RuneIterator::operator-(std::size_t count) const {
  auto it = *this;

  for(; count > 0; --count) {
    it.prev_rune();
  }

  return it;
}

RuneIterator& RuneIterator::operator-=(std::size_t count) {
  for(; count > 0; --count) {
    prev_rune();
  }

  return *this;
}

void RuneIterator::next_rune() {
  index_ += byte_count_;

  if(index_ < str_.size()) {
    rune_ = RuneUtil::next_rune(str_,index_,byte_count_);
  } else {
    index_ = str_.size();
    rune_ = RuneUtil::kInvalidRune;
  }
}

void RuneIterator::prev_rune() {
  if(index_ > 0) {
    --index_; // RuneUtil::prev_rune(...) will search backwards for us.
  } else {
    rune_ = RuneUtil::kInvalidRune;
    return;
  }

  rune_ = RuneUtil::prev_rune(str_,index_,byte_count_);

  if(byte_count_ <= index_) {
    index_ = index_ - byte_count_ + 1; // next_rune() & prev_rune() need to use the same index.
  } else {
    index_ = 0;
  }
}

std::string_view RuneIterator::str() const { return str_; }

std::size_t RuneIterator::index() const { return index_; }

std::uint8_t RuneIterator::byte_count() const { return byte_count_; }

char32_t RuneIterator::rune() const { return rune_; }

std::string RuneIterator::substr() const { return std::string{substr_view()}; }

std::string_view RuneIterator::substr_view() const { return str_.substr(index_,byte_count_); }

std::string RuneIterator::pack_rune() const { return RuneUtil::pack(rune_); }

char RuneIterator::byte() const { return str_[index_]; }

} // namespace cybel::utf8
