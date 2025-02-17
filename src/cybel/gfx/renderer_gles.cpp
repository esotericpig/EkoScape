/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "renderer_gles.h"

#if defined(CYBEL_RENDERER_GLES)

#include "cybel/types/cybel_error.h"
#include "cybel/util/util.h"

// - https://github.com/g-truc/glm/blob/master/manual.md#-12-using-separated-headers
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace cybel {

std::string RendererGles::fetch_info_log(GLuint object,InfoLogType type) {
  GLint len = 0;

  switch(type) {
    case InfoLogType::kShader:
      glGetShaderiv(object,GL_INFO_LOG_LENGTH,&len);
      break;

    case InfoLogType::kProgram:
      glGetProgramiv(object,GL_INFO_LOG_LENGTH,&len);
      break;
  }

  if(len <= 0) { return ""; }

  std::string msg(len,0);

  switch(type) {
    case InfoLogType::kShader:
      glGetShaderInfoLog(object,len,NULL,msg.data());
      break;

    case InfoLogType::kProgram:
      glGetProgramInfoLog(object,len,NULL,msg.data());
      break;
  }

  return msg;
}

RendererGles::RendererGles(const Size2i& size,const Size2i& target_size,const Color4f& clear_color)
  : Renderer(size,target_size,clear_color) {
  init_prog();

  glDepthRangef(0.0f,1.0f);
  glClearDepthf(1.0f);

  const auto error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{"Failed to init GLES renderer: ",Util::get_gl_error(error),'.'};
  }
}

void RendererGles::init_prog() {
  Shader vert_shader{};
  Shader frag_shader{};

  // NOTE: For WebGL, `#version` must be on the very first line (no spaces/newlines before it).
  vert_shader.init(GL_VERTEX_SHADER,R"(#version 300 es
    precision highp float; // Best for positions & physics.

    uniform mat4 proj_mat;
    uniform mat4 model_mat;

    // Use layout location so don't need to call glGetAttribLocation(), glBindAttribLocation(), etc.
    layout(location = 0) in vec3 vertex_pos;
    layout(location = 1) in vec2 tex_coord;

    out vec2 frag_tex_coord;

    void main() {
        gl_Position = proj_mat * model_mat * vec4(vertex_pos,1.0);
        frag_tex_coord = tex_coord;
    }
  )");
  frag_shader.init(GL_FRAGMENT_SHADER,R"(#version 300 es
    precision mediump float; // Best for colors, normals, & general math.

    in vec2 frag_tex_coord;

    uniform vec4 color;
    uniform bool use_tex;
    uniform sampler2D tex_2d;

    layout(location = 0) out vec4 out_color;

    void main() {
      if(use_tex) {
          out_color = texture(tex_2d,frag_tex_coord) * color;
      } else {
          out_color = color;
      }

      // This "could" be slightly faster by avoiding branching, but I dunno.
      //out_color = mix(color,texture(tex_2d,frag_tex_coord) * color,float(use_tex));
    }
  )");

  prog_.init(vert_shader.object(),frag_shader.object());

  GLenum error = GL_NO_ERROR;

  glUseProgram(prog_.object());
  error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{"Failed to use GLES program: ",Util::get_gl_error(error),'.'};
  }

  proj_mat_loc_ = glGetUniformLocation(prog_.object(),"proj_mat");
  model_mat_loc_ = glGetUniformLocation(prog_.object(),"model_mat");
  vertex_pos_loc_ = glGetUniformLocation(prog_.object(),"vertex_pos");
  tex_coord_loc_ = glGetUniformLocation(prog_.object(),"tex_coord");
  color_loc_ = glGetUniformLocation(prog_.object(),"color");
  use_tex_loc_ = glGetUniformLocation(prog_.object(),"use_tex");
  tex_2d_loc_ = glGetUniformLocation(prog_.object(),"tex_2d");

  // Init Vertex & Fragment shaders' vars.
  RendererGles::end_color();
  RendererGles::end_tex();

  // Init Projection & Model matrices.
  RendererGles::resize(dimens_.size);

  error = glGetError();

  if(error != GL_NO_ERROR) {
    throw CybelError{"Failed to init GLES program: ",Util::get_gl_error(error),'.'};
  }

  quad_buffer_.init();
}

void RendererGles::resize(const Size2i& size) {
  Renderer::resize(size);

  const auto w = static_cast<float>(dimens_.size.w);
  const auto h = static_cast<float>(dimens_.size.h);

  ortho_proj_mat_ = glm::ortho(0.0f,w,h,0.0f,-5.0f,5.0f);
  pers_proj_mat_ = glm::perspective(glm::radians(45.0f),w / h,0.01f,5.0f);
}

Renderer& RendererGles::begin_2d_scene() {
  model_mat_ = kIdentityMat;

  glUniformMatrix4fv(proj_mat_loc_,1,GL_FALSE,value_ptr(ortho_proj_mat_));
  glUniformMatrix4fv(model_mat_loc_,1,GL_FALSE,value_ptr(model_mat_));

  return *this;
}

Renderer& RendererGles::begin_3d_scene() {
  model_mat_ = kIdentityMat;

  glUniformMatrix4fv(proj_mat_loc_,1,GL_FALSE,value_ptr(pers_proj_mat_));
  glUniformMatrix4fv(model_mat_loc_,1,GL_FALSE,value_ptr(model_mat_));

  return *this;
}

Renderer& RendererGles::begin_color(const Color4f& color) {
  glUniform4f(color_loc_,color.r,color.g,color.b,color.a);

  return *this;
}

Renderer& RendererGles::begin_tex(const Texture& tex) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,tex.gl_id());

  glUniform1i(tex_2d_loc_,0); // GL_TEXTURE0.
  glUniform1i(use_tex_loc_,GL_TRUE);

  return *this;
}

Renderer& RendererGles::end_tex() {
  glUniform1i(use_tex_loc_,GL_FALSE);
  glUniform1i(tex_2d_loc_,0); // GL_TEXTURE0.

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,0); // Unbind.

  return *this;
}

Renderer& RendererGles::wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback) {
  const auto pos_vec = glm::vec3(
    offset_.x + (static_cast<GLfloat>(pos.x) * scale_.x),
    offset_.y + (static_cast<GLfloat>(pos.y) * scale_.y),
    static_cast<GLfloat>(pos.z)
  );
  auto temp_model_mat = model_mat_;

  temp_model_mat = translate(temp_model_mat,pos_vec);
  temp_model_mat = rotate(temp_model_mat,glm::radians(angle),glm::vec3(0.0f,0.0f,1.0f));
  temp_model_mat = translate(temp_model_mat,-pos_vec);
  glUniformMatrix4fv(model_mat_loc_,1,GL_FALSE,value_ptr(temp_model_mat));

  callback();

  glUniformMatrix4fv(model_mat_loc_,1,GL_FALSE,value_ptr(model_mat_));

  return *this;
}

Renderer& RendererGles::draw_quad(const Pos3i& pos,const Size2i& size) {
  return draw_quad(kDefaultSrc,pos,size);
}

Renderer& RendererGles::draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) {
  quad_buffer_.set_vertex_data(src,build_dest_pos5f(pos,size));
  quad_buffer_.draw();

  return *this;
}

void RendererGles::Shader::init(GLenum type,const std::string& src) {
  object_ = glCreateShader(type);

  if(object_ == 0) {
    throw CybelError{"Failed to create GLES shader [",type,"]: ",Util::get_gl_error(glGetError()),'.'};
  }

  GLenum error = GL_NO_ERROR;

  const auto* src_cstr = reinterpret_cast<const GLchar*>(src.c_str());
  const auto len = static_cast<GLint>(src.size());

  glShaderSource(object_,1,&src_cstr,&len);
  error = glGetError();

  if(error != GL_NO_ERROR) {
    destroy();
    throw CybelError{"Failed to set source of GLES shader [",type,"]: ",Util::get_gl_error(error),'.'};
  }

  glCompileShader(object_);
  error = glGetError();

  if(error != GL_NO_ERROR) {
    destroy();
    throw CybelError{"Failed to compile GLES shader [",type,"]: ",Util::get_gl_error(error),'.'};
  }

  GLint compiled = GL_FALSE;
  glGetShaderiv(object_,GL_COMPILE_STATUS,&compiled);

  if(compiled == GL_FALSE) {
    const auto log = fetch_info_log(object_,InfoLogType::kShader);

    destroy();
    throw CybelError{"Failed to compile GLES shader [",type,"]: ",Util::get_gl_error(glGetError()),
                     ".\n> ",log};
  }
}

RendererGles::Shader::~Shader() noexcept {
  destroy();
}

void RendererGles::Shader::destroy() noexcept {
  if(object_ != 0) {
    glDeleteShader(object_);
    object_ = 0;
  }
}

GLuint RendererGles::Shader::object() const { return object_; }

void RendererGles::Program::init(GLuint vert_shader,GLuint frag_shader) {
  object_ = glCreateProgram();

  if(object_ == 0) {
    throw CybelError{"Failed to create GLES program: ",Util::get_gl_error(glGetError()),'.'};
  }

  GLenum error = GL_NO_ERROR;

  glAttachShader(object_,vert_shader);
  glAttachShader(object_,frag_shader);
  error = glGetError();

  if(error != GL_NO_ERROR) {
    destroy();
    throw CybelError{"Failed to attach shaders to GLES program: ",Util::get_gl_error(error),'.'};
  }

  glLinkProgram(object_);
  error = glGetError();

  if(error != GL_NO_ERROR) {
    destroy();
    throw CybelError{"Failed to link GLES program: ",Util::get_gl_error(error),'.'};
  }

  GLint linked = GL_FALSE;
  glGetProgramiv(object_,GL_LINK_STATUS,&linked);

  if(linked == GL_FALSE) {
    const auto log = fetch_info_log(object_,InfoLogType::kProgram);

    destroy();
    throw CybelError{"Failed to link GLES program: ",Util::get_gl_error(glGetError()),
                     ".\n> ",log};
  }
}

RendererGles::Program::~Program() noexcept {
  destroy();
}

void RendererGles::Program::destroy() noexcept {
  if(object_ != 0) {
    glDeleteProgram(object_);
    object_ = 0;
  }
}

GLuint RendererGles::Program::object() const { return object_; }

void RendererGles::QuadBuffer::init() {
  constexpr std::size_t row_byte_count = kVertexDataColCount * sizeof(GLfloat);
  const auto* tex_coord_offset = reinterpret_cast<const void*>(3 * sizeof(GLfloat));

  // VAO.
  glGenVertexArrays(1,&vao_);
  glBindVertexArray(vao_);

  // VBO.
  glGenBuffers(1,&vbo_);
  glBindBuffer(GL_ARRAY_BUFFER,vbo_);
  // - Dynamic since `vertex_data_` will be modified repeatedly and used many times.
  glBufferData(GL_ARRAY_BUFFER,kVertexDataByteCount,vertex_data_.data(),GL_DYNAMIC_DRAW);

  // EBO.
  glGenBuffers(1,&ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
  // - Static since `kIndices` will be modified once and used many times.
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,kIndices.size() * sizeof(GLuint),kIndices.data(),GL_STATIC_DRAW);

  // Vertex pos: `layout(location = 0) in vec3 vertex_pos;`.
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,row_byte_count,0);
  glEnableVertexAttribArray(0);

  // TexCoord: `layout(location = 1) in vec2 tex_coord;`.
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,row_byte_count,tex_coord_offset);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0); // Unbind VAO.

  const auto error = glGetError();

  if(error != GL_NO_ERROR) {
    destroy();
    throw CybelError{"Failed to init GLES QuadBuffer: ",Util::get_gl_error(error),'.'};
  }
}

RendererGles::QuadBuffer::~QuadBuffer() noexcept {
  destroy();
}

void RendererGles::QuadBuffer::destroy() noexcept {
  if(ebo_ != 0) {
    glDeleteBuffers(1,&ebo_);
    ebo_ = 0;
  }
  if(vbo_ != 0) {
    glDeleteBuffers(1,&vbo_);
    vbo_ = 0;
  }
  if(vao_ != 0) {
    glDeleteVertexArrays(1,&vao_);
    vao_ = 0;
  }
}

void RendererGles::QuadBuffer::draw() {
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES,kIndices.size(),GL_UNSIGNED_INT,0);
  glBindVertexArray(0); // Unbind VAO.
}

void RendererGles::QuadBuffer::set_vertex_data(const Pos4f& src,const Pos5f& pos) {
  const auto [x1,y1,x2,y2,z] = pos;

  vertex_data_ = {
    // Vertex.  TexCoord.
    x1,y1,z,    src.x1,src.y1,
    x2,y1,z,    src.x2,src.y1,
    x2,y2,z,    src.x2,src.y2,
    x1,y2,z,    src.x1,src.y2,
  };

  glBindBuffer(GL_ARRAY_BUFFER,vbo_);
  glBufferSubData(GL_ARRAY_BUFFER,0,kVertexDataByteCount,vertex_data_.data());
}

} // Namespace.
#endif // CYBEL_RENDERER_GLES.
