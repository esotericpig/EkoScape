/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "texture.h"

namespace ekoscape {

Texture::Texture(Image& image,bool make_weird) {
  const Uint8 bpp = image.bpp();
  bool is_red_first = image.is_red_first();
  GLenum image_format = GL_RGBA;

  if(make_weird) { is_red_first = !is_red_first; }

  switch(bpp) {
    case 4:
      image_format = is_red_first ? GL_RGBA : GL_BGRA;
      break;

    case 3:
      image_format = is_red_first ? GL_RGB : GL_BGR;
      break;

    default:
      throw EkoScapeError{Util::build_string("Unsupported BPP [",bpp,"] for image [",image.id(),"].")};
  }

  glGenTextures(1,&id_);
  glBindTexture(GL_TEXTURE_2D,id_);

  // I didn't have any problems without this, but could be needed.
  // - https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
  if(bpp == 3) {
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  } else {
    glPixelStorei(GL_UNPACK_ALIGNMENT,4); // Should be the default.
  }

  // See: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);

  image.lock();
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,image.width(),image.height(),0,image_format
      ,image.gl_type(),image.pixels());
  image.unlock();

  // See: https://open.gl/textures
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    if(id_ != 0) {
      glDeleteTextures(1,&id_);
      id_ = 0;
    }

    throw EkoScapeError{Util::build_string("Failed to gen/bind texture for image [",image.id()
        ,"]; error [",error,"]: ",Util::get_gl_error(error),'.')};
  }
}

Texture::Texture(GLubyte r,GLubyte g,GLubyte b,GLubyte a,bool make_weird) {
  if(make_weird) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }

  GLubyte pixels[] = {
    r,g,b,a, r,g,b,a,
    r,g,b,a, r,g,b,a
  };

  glGenTextures(1,&id_);
  glBindTexture(GL_TEXTURE_2D,id_);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_BASE_LEVEL,0);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  glBindTexture(GL_TEXTURE_2D,0); // Unbind texture.

  GLenum error = glGetError();

  if(error != GL_NO_ERROR) {
    if(id_ != 0) {
      glDeleteTextures(1,&id_);
      id_ = 0;
    }

    throw EkoScapeError{Util::build_string("Failed to gen/bind texture for color ("
        ,r,',',g,',',b,',',a,"); error [",error,"]: ",Util::get_gl_error(error),'.')};
  }
}

Texture::~Texture() noexcept {
  if(id_ != 0) {
    glDeleteTextures(1,&id_);
    id_ = 0;
  }
}

GLuint Texture::id() const { return id_; }

} // Namespace.
