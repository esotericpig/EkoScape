/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_GFX_RENDERER_GLES_H_
#define CYBEL_GFX_RENDERER_GLES_H_

#include "cybel/common.h"

#if defined(CYBEL_RENDERER_GLES)

#include "cybel/gfx/renderer.h"

// - https://github.com/g-truc/glm/blob/master/manual.md#-12-using-separated-headers
#include <glm/mat4x4.hpp>

namespace cybel {

class RendererGles : public Renderer {
public:
  explicit RendererGles(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  void resize(const Size2i& size) override;

  Renderer& begin_2d_scene() override;
  Renderer& begin_3d_scene() override;

  Renderer& begin_color(const Color4f& color) override;

  Renderer& begin_tex(const Texture& tex) override;
  Renderer& end_tex() override;

  Renderer& wrap_rotate(const Pos3i& pos,float angle,const WrapCallback& callback) override;

  Renderer& draw_quad(const Pos3i& pos,const Size2i& size) override;
  Renderer& draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) override;

private:
  enum class InfoLogType { kShader,kProgram };

  class Shader {
  public:
    explicit Shader() noexcept = default;
    void init(GLenum type,const std::string& src);

    Shader(const Shader& other) = delete;
    Shader(Shader&& other) noexcept = delete;
    virtual ~Shader() noexcept;

    Shader& operator=(const Shader& other) = delete;
    Shader& operator=(Shader&& other) noexcept = delete;

    GLuint object() const;

  private:
    GLuint object_ = 0;

    void destroy() noexcept;
  };

  class Program {
  public:
    explicit Program() noexcept = default;
    void init(GLuint vert_shader,GLuint frag_shader);

    Program(const Program& other) = delete;
    Program(Program&& other) noexcept = delete;
    virtual ~Program() noexcept;

    Program& operator=(const Program& other) = delete;
    Program& operator=(Program&& other) noexcept = delete;

    GLuint object() const;

  private:
    GLuint object_ = 0;

    void destroy() noexcept;
  };

  class QuadBuffer {
  public:
    explicit QuadBuffer() noexcept = default;
    void init();

    QuadBuffer(const QuadBuffer& other) = delete;
    QuadBuffer(QuadBuffer&& other) noexcept = delete;
    virtual ~QuadBuffer() noexcept;

    QuadBuffer& operator=(const QuadBuffer& other) = delete;
    QuadBuffer& operator=(QuadBuffer&& other) noexcept = delete;

    void draw();

    void set_vertex_data(const Pos4f& src,const Pos5f& pos);

  private:
    static constexpr std::size_t kVertexDataColCount = 5;
    static constexpr std::size_t kVertexDataRowCount = 4;
    static constexpr std::size_t kVertexDataCount = kVertexDataColCount * kVertexDataRowCount;
    static constexpr std::size_t kVertexDataByteCount = kVertexDataCount * sizeof(GLfloat);

    static inline const std::array<GLuint,6> kIndices = {
      0,1,2, // Top triangle.
      2,3,0, // Bottom triangle.
    };

    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;

    std::array<GLfloat,kVertexDataCount> vertex_data_ = {
      // Vertex.       TexCoord.
      0.0f,0.0f,0.0f,  0.0f,0.0f,
      1.0f,0.0f,0.0f,  1.0f,0.0f,
      1.0f,1.0f,0.0f,  1.0f,1.0f,
      0.0f,1.0f,0.0f,  0.0f,1.0f,
    };

    void destroy() noexcept;
  };

  static constexpr auto kIdentityMat = glm::mat4(1.0f);

  Program prog_{};
  GLint proj_mat_loc_ = -1;
  GLint model_mat_loc_ = -1;
  GLint vertex_pos_loc_ = -1;
  GLint tex_coord_loc_ = -1;
  GLint color_loc_ = -1;
  GLint use_tex_loc_ = -1;
  GLint tex_2d_loc_ = -1;

  glm::mat4 ortho_proj_mat_ = kIdentityMat;
  glm::mat4 pers_proj_mat_ = kIdentityMat;
  glm::mat4 model_mat_ = kIdentityMat;

  QuadBuffer quad_buffer_{};

  static std::string fetch_info_log(GLuint object,InfoLogType type);

  void init_prog();
};

} // Namespace.
#endif // CYBEL_RENDERER_GLES.
#endif
