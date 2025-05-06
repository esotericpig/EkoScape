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

#include <set>
#include <stack>

namespace cybel {

// NOTE: There are inner classes that have an init() function that must be called, like a constructor.
//       This design pattern was chosen for implementing WebGL context restored.
class RendererGles : public Renderer {
public:
  explicit RendererGles(const Size2i& size,const Size2i& target_size,const Color4f& clear_color);

  void on_context_lost() override;
  void on_context_restored() override;
  void resize(const Size2i& size) override;

  Renderer& begin_2d_scene() override;
  Renderer& begin_3d_scene() override;

  Renderer& begin_color(const Color4f& color) override;

  Renderer& begin_tex(const Texture& tex) override;
  Renderer& end_tex() override;

  Renderer& draw_quad(const Pos3i& pos,const Size2i& size) override;
  Renderer& draw_quad(const Pos4f& src,const Pos3i& pos,const Size2i& size) override;

  void translate_model_matrix(const Pos3f& pos) override;
  void rotate_model_matrix(float angle,const Pos3f& axis) override;
  void update_model_matrix() override;
  void push_model_matrix() override;
  void pop_model_matrix() override;

  GLuint gen_quad_buffers(int count) override;
  void delete_quad_buffers(GLuint id,int count) override;
  void compile_quad_buffer(GLuint id,int index,const QuadBufferData& data) override;
  void draw_quad_buffer(GLuint id,int index) override;

private:
  enum class InfoLogType { kShader,kProgram };

  class Shader {
  public:
    explicit Shader(GLenum type,const std::string& src);

    Shader(const Shader& other) = delete;
    Shader(Shader&& other) noexcept = delete;
    virtual ~Shader() noexcept;

    Shader& operator=(const Shader& other) = delete;
    Shader& operator=(Shader&& other) noexcept = delete;

    GLuint handle() const;

  private:
    GLuint handle_ = 0;

    void destroy() noexcept;
  };

  class Program {
  public:
    explicit Program() = default;
    void init(const Shader& vert_shader,const Shader& frag_shader);

    Program(const Program& other) = delete;
    Program(Program&& other) noexcept = delete;
    virtual ~Program() noexcept;

    Program& operator=(const Program& other) = delete;
    Program& operator=(Program&& other) noexcept = delete;

    void zombify();

    GLuint handle() const;

  private:
    GLuint handle_ = 0;

    void destroy() noexcept;
  };

  class QuadBuffer {
  public:
    explicit QuadBuffer() = default;
    void init();

    QuadBuffer(const QuadBuffer& other) = delete;
    QuadBuffer(QuadBuffer&& other) noexcept;
    virtual ~QuadBuffer() noexcept;

    QuadBuffer& operator=(const QuadBuffer& other) = delete;
    QuadBuffer& operator=(QuadBuffer&& other) noexcept;

    void zombify();
    void draw();

    void set_data(const QuadBufferData& data);
    void set_vertex_data(const Pos4f& src,const Pos5f& pos);

    GLuint tex_handle() const;

  private:
    static constexpr std::size_t kVertexDataColCount = 5;
    static constexpr std::size_t kVertexDataRowCount = 4;
    static constexpr std::size_t kVertexDataCount = kVertexDataColCount * kVertexDataRowCount;
    static constexpr std::size_t kVertexDataByteCount = kVertexDataCount * sizeof(GLfloat);

    static inline const std::array<GLuint,6> kIndices = {
      0,1,2, // Top triangle.
      2,3,0, // Bottom triangle.
    };

    GLuint tex_handle_ = 0;
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

    void move_from(QuadBuffer&& other) noexcept;
    void destroy() noexcept;

    void update_vertex_data();
  };

  class QuadBufferBag {
  public:
    explicit QuadBufferBag(int count);
    void init();

    void zombify();

    QuadBuffer* buffer(int index);
    std::size_t size() const;

  private:
    std::vector<QuadBuffer> buffers_;
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
  std::stack<glm::mat4> model_mats_{};

  QuadBuffer quad_buffer_{};
  // `unordered_set` is more efficient, but using `set` as it's better for debugging.
  std::set<GLuint> free_quad_buffer_ids_{};
  std::vector<std::unique_ptr<QuadBufferBag>> quad_buffer_bags_{};

  static std::string fetch_info_log(GLuint handle,InfoLogType type);

  void init();
  void init_prog();

  Renderer& begin_tex(GLuint handle);

  QuadBufferBag* quad_buffer_bag(GLuint id);
  QuadBuffer* quad_buffer(GLuint id,int index);
};

} // namespace cybel
#endif // CYBEL_RENDERER_GLES
#endif
