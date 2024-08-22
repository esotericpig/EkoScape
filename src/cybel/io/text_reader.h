/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_IO_TEXT_READER_H_
#define CYBEL_IO_TEXT_READER_H_

#include "cybel/common.h"

#include "text_reader_buf.h"

#include <filesystem>
#include <istream>

namespace cybel {

/**
 * All line functions are platform-independent.
 * Every line can be '\r', '\n', or "\r\n", instead of only one.
 */
// NOTE: See limitations of TextReaderBuf before modifying this class.
class TextReader {
public:
  explicit TextReader(const std::filesystem::path& file
      ,std::size_t buffer_size = TextReaderBuf::kDefaultInitBufferSize);

  bool read_line(std::string& line);

  bool read(float& data);
  bool read(int& data);

  bool get(char& data);

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
