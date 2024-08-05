/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_TEXT_READER_H_
#define EKOSCAPE_CORE_TEXT_READER_H_

#include "common.h"

#include "text_reader_buf.h"

#include <istream>

namespace ekoscape {

/**
 * See limitations of TextReaderBuf before modifying this class.
 *
 * All line functions are platform-independent.
 * Every line can be '\r', '\n', or "\r\n", instead of only one.
 */
class TextReader {
public:
  TextReader(const std::string& file,std::size_t buffer_size = TextReaderBuf::kDefaultInitBufferSize);

  bool read_line(std::string& line);

  /**
   * This doesn't ignore white space.
   */
  bool read(char& data);

  bool read(float& data);
  bool read(int& data);

  bool seek_and_destroy(char target);
  bool consume_lines_if_empty(int line_count);

  bool eof() const;

private:
  TextReaderBuf buf_;
  std::istream in_;

  template <typename T>
  bool read_imp(T& data,T init_value);
};

template <typename T>
bool TextReader::read_imp(T& data,T init_value) {
  data = init_value;

  return (in_ >> data) ? true : false;
}

} // Namespace.
#endif
