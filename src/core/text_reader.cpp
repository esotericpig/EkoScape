/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "text_reader.h"

namespace ekoscape {

TextReader::TextReader(const std::string& file,std::size_t buffer_size)
    : buf_(file,buffer_size),in_(&buf_) {}

bool TextReader::read_line(std::string& line) {
  line.clear(); // Match user expectations.

  if(in_.eof()) { return false; }

  for(char c; in_.get(c);) {
    if(c == '\n') { break; }
    if(c == '\r') {
      if(!in_.eof() && static_cast<char>(in_.peek()) == '\n') { in_.get(c); }
      break;
    }

    line += c;
  }

  return true;
}

bool TextReader::read(char& data) {
  data = 0;

  return in_.get(data) ? true : false;
}

bool TextReader::read(float& data) { return read_imp(data,0.0f); }

bool TextReader::read(int& data) { return read_imp(data,0); }

bool TextReader::seek_and_destroy(char target) {
  if(in_.eof()) { return false; }

  for(char c; in_.get(c);) {
    if(c == target) { return true; }
  }

  return false;
}

bool TextReader::consume_lines_if_empty(int line_count) {
  for(; line_count >= 1; --line_count) {
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
  }

  return true;
}

bool TextReader::eof() const { return in_.eof(); }

} // Namespace.
