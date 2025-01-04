/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "text_reader.h"

namespace cybel {

TextReader::TextReader(const std::filesystem::path& file)
    : buf_(file),in_(&buf_) {}

TextReader::TextReader(const std::filesystem::path& file,std::size_t buffer_size)
    : buf_(file,buffer_size),in_(&buf_) {}

bool TextReader::read_line(std::string& line) {
  line.clear(); // Match user expectations.

  if(in_.eof()) { return false; }

  for(char c = 0; in_.get(c);) {
    if(c == '\n') { break; }
    if(c == '\r') {
      if(!in_.eof() && static_cast<char>(in_.peek()) == '\n') { in_.get(c); }
      break;
    }

    line += c;
  }

  return true;
}

bool TextReader::read(float& data) { return read_imp(data,0.0f); }

bool TextReader::read(int& data) { return read_imp(data,0); }

bool TextReader::get(char& data) {
  data = 0;

  return in_.get(data) ? true : false;
}

bool TextReader::seek_and_destroy(char target) {
  if(in_.eof()) { return false; }

  for(char c = 0; in_.get(c);) {
    if(c == target) { return true; }
  }

  return false;
}

bool TextReader::consume_empty_lines() { return consume_empty_lines(-1); }

bool TextReader::consume_empty_lines(int max_line_count) {
  while(max_line_count == -1 || max_line_count >= 1) {
    if(in_.eof()) { return false; }

    char c = static_cast<char>(in_.peek());

    if(c == '\n') {
      in_.get(c);
    } else if(c == '\r') {
      in_.get(c);
      if(!in_.eof() && static_cast<char>(in_.peek()) == '\n') { in_.get(c); }
    } else {
      break;
    }

    if(max_line_count != -1) { --max_line_count; }
  }

  return !in_.eof();
}

bool TextReader::eof() const { return in_.eof(); }

} // Namespace.
