/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_CORE_TEXT_READER_BUF_H_
#define EKOSCAPE_CORE_TEXT_READER_BUF_H_

#include "common.h"

#include "ekoscape_error.h"
#include "util.h"

#include <algorithm>
#include <filesystem>
#include <streambuf>
#include <vector>

namespace ekoscape {

/**
 * Implemented:
 * - Reading.              - underflow()
 * - peek()                - underflow()
 *
 * Not implemented:
 * - Seeking.              - seekoff(), seekpos()
 * - Writing.              - overflow()
 * - unget(), putback()    - pbackfail()
 *   - This is kind of implemented until the buffer is empty, but not recommended to be used.
 */
class TextReaderBuf : public std::streambuf {
public:
  /**
   * Default initial buffer size; only used on init.
   */
  static const std::size_t kDefaultInitBufferSize = 1024;

  explicit TextReaderBuf(const std::filesystem::path& file,std::size_t buffer_size = kDefaultInitBufferSize);
  TextReaderBuf(const TextReaderBuf& other) = delete;
  TextReaderBuf(TextReaderBuf&& other) noexcept;
  virtual ~TextReaderBuf() noexcept;

  TextReaderBuf& operator=(const TextReaderBuf& other) = delete;
  TextReaderBuf& operator=(TextReaderBuf&& other) noexcept;

  /**
   * Mainly used for testing purposes.
   *
   * This can return false, yet there is still data in the buffer to read.
   */
  bool is_open() const;

protected:
  virtual int_type underflow() override;

  /**
   * Not implemented, since it could require seeking, which might not be supported by SDL_RWseek().
   * However, you can still do unget()/putback() until the buffer is empty.
   * After the buffer is empty, I believe it returns EOF (or the behavior is undefined).
   *
   * Always returns EOF.
   */
  virtual int_type pbackfail(int_type c = traits_type::eof()) override;

  /**
   * Not implemented, since this class is for reading only.
   *
   * Always returns EOF.
   */
  virtual int_type overflow(int_type c = traits_type::eof()) override;

private:
  using Base = std::streambuf;

  SDL_RWops* context_ = NULL;
  std::vector<char_type> buffer_;

  void init(std::size_t buffer_size) noexcept;
  void move_from(TextReaderBuf&& other) noexcept;
  void close() noexcept;
};

} // Namespace.
#endif
