/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dantares_renderer.h"

#include <cybel/math/pos.h>

#include <cassert>
#include <iterator>

namespace ekoscape {

DantaresRenderer::DantaresRenderer(Renderer& renderer) noexcept
  : renderer_{renderer} {}

void DantaresRenderer::BeginDraw() {}

void DantaresRenderer::EndDraw() {}

void DantaresRenderer::TranslateModelMatrix(float x,float y,float z) {
  renderer_.translate_model_matrix(Pos3f{x,y,z});
}

void DantaresRenderer::RotateModelMatrix(float angle_degrees,float x,float y,float z) {
  renderer_.rotate_model_matrix(angle_degrees,Pos3f{x,y,z});
}

void DantaresRenderer::UpdateModelMatrix() {
  renderer_.update_model_matrix();
}

void DantaresRenderer::PushModelMatrix() {
  renderer_.push_model_matrix();
}

void DantaresRenderer::PopModelMatrix() {
  renderer_.pop_model_matrix();
}

GLuint DantaresRenderer::GenerateQuadLists(int count) {
  return renderer_.gen_quad_commands(count);
}

void DantaresRenderer::DeleteQuadLists(GLuint id,int count) {
  renderer_.delete_quad_commands(id,count);
}

void DantaresRenderer::CompileQuadList(GLuint id,int index,const QuadListData& quad) {
  QuadCommand cybel_quad{
    .normal = Pos3f{quad.Normal.X,quad.Normal.Y,quad.Normal.Z},
    .tex_handle = quad.TextureID,
  };

  assert(std::size(quad.Vertices) >= 4);
  assert(std::size(cybel_quad.vertices) >= 4);

  for(std::size_t i = 0; i < 4; ++i) {
    const auto& v = quad.Vertices[i];

    cybel_quad.vertices[i] = Pos3f{v.X,v.Y,v.Z};
  }

  renderer_.compile_quad_command(id,index,cybel_quad);
}

void DantaresRenderer::DrawQuadList(GLuint id,int index) {
  renderer_.draw_quad_command(id,index);
}

} // namespace ekoscape
