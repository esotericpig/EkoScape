/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "texture.h"

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

namespace cybel {

Texture::Texture(Image& img) {
  const auto bypp = img.bytes_per_pixel();
  GLenum img_format = GL_RGBA;

  switch(bypp) {
    case 4:
      img_format = img.is_red_first() ? GL_RGBA : GL_BGRA;
      break;

    case 3:
      img_format = img.is_red_first() ? GL_RGB : GL_BGR;
      break;

    default:
      throw CybelError{"Unsupported Bytes Per Pixel [",static_cast<int>(bypp),"] for image [",img.id(),"]."};
  }

  glGenTextures(1,&handle_);
  glBindTexture(GL_TEXTURE_2D,handle_);

  // I didn't have any problems without this, but could be needed.
  // See: https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
  if(bypp <= 3) {
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  } else {
    glPixelStorei(GL_UNPACK_ALIGNMENT,4); // Should be the default.
  }

  if(GLEW_VERSION_3_0) {
    // See: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
  }

  // See: https://open.gl/textures
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

  img.lock();
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,img.size().w,img.size().h,0,img_format,img.gl_type(),img.pixels());
  img.unlock();

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  const GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    // Just eat error, so a blank texture is shown instead of crashing.
    std::cerr << "[WARN] Failed to gen/bind texture for image [" << img.id()
              << "]; error [" << error << "]: " << Util::get_gl_error(error) << '.' << std::endl;
    Util::clear_gl_errors();

    // destroy();
    // throw CybelError{"Failed to gen/bind texture for image [",img.id(),"]; error [",error,"]: "
    //     ,Util::get_gl_error(error),'.'};
  }

  size_ = img.size();
}

Texture::Texture(Image&& img)
  : Texture(img) {}

Texture::Texture(const Color4f& color,bool make_weird) {
  auto r = static_cast<GLubyte>(std::round(color.r * 255.0f));
  auto g = static_cast<GLubyte>(std::round(color.g * 255.0f));
  auto b = static_cast<GLubyte>(std::round(color.b * 255.0f));
  auto a = static_cast<GLubyte>(std::round(color.a * 255.0f));

  if(make_weird) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }

  constexpr int width = 2;
  constexpr int height = 2;
  constexpr std::uint8_t bypp = 4;
  constexpr int size = width * height * bypp;
  GLubyte pixels[size]{};

  for(auto* p = pixels; p < (pixels + size); p += bypp) {
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = a;
  }

  glGenTextures(1,&handle_);
  glBindTexture(GL_TEXTURE_2D,handle_);

  if(GLEW_VERSION_3_0) {
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
  }

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  const GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    // Just eat error, so a blank texture is shown instead of crashing.
    std::cerr << "[WARN] Failed to gen/bind texture for color ("
              << static_cast<int>(r) << ','
              << static_cast<int>(g) << ','
              << static_cast<int>(b) << ','
              << static_cast<int>(a)
              << "); error [" << error << "]: " << Util::get_gl_error(error) << '.' << std::endl;
    Util::clear_gl_errors();

    // destroy();
    // throw CybelError{"Failed to gen/bind texture for color ("
    //     ,static_cast<int>(r),','
    //     ,static_cast<int>(g),','
    //     ,static_cast<int>(b),','
    //     ,static_cast<int>(a)
    //     ,"); error [",error,"]: ",Util::get_gl_error(error),'.'};
  }

  size_.w = width;
  size_.h = height;
}

Texture::Texture(Texture&& other) noexcept {
  move_from(std::move(other));
}

void Texture::move_from(Texture&& other) noexcept {
  destroy();

  handle_ = std::exchange(other.handle_,0);
  size_ = std::exchange(other.size_,Size2i{});
}

Texture::~Texture() noexcept {
  destroy();
}

void Texture::destroy() noexcept {
  if(handle_ != 0) {
    glDeleteTextures(1,&handle_);
    handle_ = 0;
  }
}

Texture& Texture::operator=(Texture&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

void Texture::zombify() { handle_ = 0; }

GLuint Texture::handle() const { return handle_; }

const Size2i& Texture::size() const { return size_; }

} // namespace cybel
