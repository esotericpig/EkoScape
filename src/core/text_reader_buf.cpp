/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "text_reader_buf.h"

namespace ekoscape {

TextReaderBuf::TextReaderBuf(const std::string& file,std::size_t buffer_size) {
  init(buffer_size);

  context_ = SDL_RWFromFile(file.c_str(),"r");

  if(context_ == NULL) {
    throw EkoScapeError{Util::build_string("Failed to open file [",file,"] for reading: "
        ,Util::get_sdl_error(),'.')};
  }

  // Init to needing data to read (begin=end).
  setg(buffer_.data(),buffer_.data(),buffer_.data()); // (begin,current,end)
}

TextReaderBuf::TextReaderBuf(TextReaderBuf&& other) noexcept
    : Base(std::move(other)),buffer_(std::move(other.buffer_)) {
  // Can't use std::exchange(), because have to use NULL instead of nullptr.
  context_ = other.context_;
  other.context_ = NULL;
}

void TextReaderBuf::init(std::size_t buffer_size) noexcept {
  buffer_.resize(buffer_size,0);
}

TextReaderBuf::~TextReaderBuf() noexcept {
  close();
}

void TextReaderBuf::close() noexcept {
  if(context_ != NULL) {
    SDL_RWclose(context_);
    context_ = NULL;
  }
}

TextReaderBuf& TextReaderBuf::operator=(TextReaderBuf&& other) noexcept {
  if(this != &other) {
    Base::operator=(std::move(other));
    close();

    context_ = other.context_;
    other.context_ = NULL;
  }

  return *this;
}

TextReaderBuf::int_type TextReaderBuf::underflow() {
  if(gptr() < egptr()) { return traits_type::to_int_type(*gptr()); }

  // On failure/EOF, gptr() must equal egptr().
  setg(buffer_.data(),buffer_.data(),buffer_.data());

  if(context_ == NULL) { return traits_type::eof(); }

  std::size_t read_count = buffer_.size();

  // Unfortunately, SDL2's SDL_RWread() returns 0 for either EOF or error,
  //     with no info about how many bytes were read before EOF.
  // Because of this, we have to rely on the last value in `buffer_` that is not 0.
  //     Terrible design.
  std::fill(buffer_.begin(),buffer_.end(),0);
  SDL_ClearError();

  if(SDL_RWread(context_,buffer_.data(),sizeof(char_type) * read_count,1) == 0) {
    close();

    std::string error = Util::get_sdl_error();

    if(!error.empty()) {
      std::cerr << "[WARN] Failed to read data: " << error << '.' << std::endl;
      // Don't fail; maybe it still read some data.
    }

    // Find the last non-zero value.
    // - `read_count` is unsigned, so need >0 and -1 inside the loop, instead of >=0.
    for(; read_count > 0; --read_count) {
      if(buffer_.at(read_count - 1) != 0) { break; }
    }

    if(read_count <= 0) { return traits_type::eof(); }
  }

  setg(buffer_.data(),buffer_.data(),buffer_.data() + read_count);

  return traits_type::to_int_type(*gptr());
}

TextReaderBuf::int_type TextReaderBuf::pbackfail(int_type) { return traits_type::eof(); }

TextReaderBuf::int_type TextReaderBuf::overflow(TextReaderBuf::int_type) { return traits_type::eof(); }

bool TextReaderBuf::is_open() const { return context_ != NULL; }

} // Namespace.
