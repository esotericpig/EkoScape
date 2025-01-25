/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "text_reader_buf.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

TextReaderBuf::TextReaderBuf(const std::filesystem::path& file,std::size_t buffer_size)
  : buffer_(std::max<std::size_t>(buffer_size,1),0) {
  const std::u8string file_str = file.u8string();
  auto file_cstr = reinterpret_cast<const char*>(file_str.c_str());

  context_ = SDL_RWFromFile(file_cstr,"r");

  if(context_ == NULL) {
    throw CybelError{"Failed to open file [",file_cstr,"] for reading: ",Util::get_sdl_error(),'.'};
  }

  // Init to needing data to read (begin=end).
  setg(buffer_.data(),buffer_.data(),buffer_.data()); // (begin,current,end)
}

TextReaderBuf::TextReaderBuf(TextReaderBuf&& other) noexcept
  : Base(std::move(other)) {
  move_from(std::move(other));
}

void TextReaderBuf::move_from(TextReaderBuf&& other) noexcept {
  close();

  context_ = other.context_;
  other.context_ = NULL;

  buffer_ = std::move(other.buffer_);
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
    move_from(std::move(other));
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
  std::ranges::fill(buffer_,0);
  SDL_ClearError();

  if(SDL_RWread(context_,buffer_.data(),sizeof(char_type) * read_count,1) == 0) {
    close();

    std::string error = Util::get_sdl_error();

    if(!error.empty()) {
      std::cerr << "[WARN] Failed to read data: " << error << '.' << std::endl;
      // Don't fail; maybe it still read some data.
    }

    // Find the last non-zero value.
    for(; read_count > 0; --read_count) { // Unsigned loop.
      if(buffer_.at(read_count - 1) != 0) { break; }
    }

    if(read_count == 0) { return traits_type::eof(); }
  }

  setg(buffer_.data(),buffer_.data(),buffer_.data() + read_count);

  return traits_type::to_int_type(*gptr());
}

TextReaderBuf::int_type TextReaderBuf::pbackfail(int_type) { return traits_type::eof(); }

TextReaderBuf::int_type TextReaderBuf::overflow(int_type) { return traits_type::eof(); }

bool TextReaderBuf::is_open() const { return context_ != NULL; }

} // Namespace.
