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

Texture::Texture(Image& img,bool make_weird) {
  const auto bpp = img.bpp();
  bool is_red_first = img.is_red_first();
  GLenum img_format = GL_RGBA;

  if(make_weird) { is_red_first = !is_red_first; }

  switch(bpp) {
    case 4:
      img_format = is_red_first ? GL_RGBA : GL_BGRA;
      break;

    case 3:
      img_format = is_red_first ? GL_RGB : GL_BGR;
      break;

    default:
      throw CybelError{"Unsupported BPP [",static_cast<int>(bpp),"] for image [",img.id(),"]."};
  }

  glGenTextures(1,&gl_id_);
  glBindTexture(GL_TEXTURE_2D,gl_id_);

  // I didn't have any problems without this, but could be needed.
  // See: https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
  if(bpp == 3) {
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  } else {
    glPixelStorei(GL_UNPACK_ALIGNMENT,4); // Should be the default.
  }

  // See: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);

  img.lock();
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img.size().w,img.size().h,0,img_format,img.gl_type(),img.pixels());
  img.unlock();

  // See: https://open.gl/textures
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    destroy();
    throw CybelError{"Failed to gen/bind texture for image [",img.id(),"]; error [",error,"]: "
        ,Util::get_gl_error(error),'.'};
  }

  size_ = img.size();
}

Texture::Texture(Image&& img,bool make_weird)
    : Texture(img,make_weird) {}

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

  const int width = 2;
  const int height = 2;
  const std::uint8_t bpp = 4;
  const int size = width * height * bpp;
  GLubyte pixels[size]{};

  for(auto* p = pixels; p < (pixels + size); p += bpp) {
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = a;
  }

  glGenTextures(1,&gl_id_);
  glBindTexture(GL_TEXTURE_2D,gl_id_);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    destroy();
    throw CybelError{"Failed to gen/bind texture for color ("
        ,static_cast<int>(r),',',static_cast<int>(g),',',static_cast<int>(b),',',static_cast<int>(a)
        ,"); error [",error,"]: ",Util::get_gl_error(error),'.'};
  }

  size_.w = width;
  size_.h = height;
}

Texture::Texture(Texture&& other) noexcept {
  move_from(std::move(other));
}

void Texture::move_from(Texture&& other) noexcept {
  destroy();

  gl_id_ = std::exchange(other.gl_id_,0);
  size_ = std::exchange(other.size_,Size2i{});
}

Texture::~Texture() noexcept {
  destroy();
}

void Texture::destroy() noexcept {
  if(gl_id_ != 0) {
    glDeleteTextures(1,&gl_id_);
    gl_id_ = 0;
  }
}

Texture& Texture::operator=(Texture&& other) noexcept {
  if(this != &other) { move_from(std::move(other)); }

  return *this;
}

GLuint Texture::gl_id() const { return gl_id_; }

const Size2i& Texture::size() const { return size_; }

} // Namespace.
