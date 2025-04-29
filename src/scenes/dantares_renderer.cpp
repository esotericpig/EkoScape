/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dantares_renderer.h"

namespace ekoscape {

DantaresRenderer::DantaresRenderer(Renderer& renderer) noexcept
  : renderer_(renderer) {}

void DantaresRenderer::BeginDraw() {}

void DantaresRenderer::EndDraw() {}

void DantaresRenderer::TranslateModelMatrix(float x,float y,float z) {
  renderer_.translate_model_matrix(Pos3f{x,y,z});
}

void DantaresRenderer::RotateModelMatrix(float angle,float x,float y,float z) {
  renderer_.rotate_model_matrix(angle,Pos3f{x,y,z});
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
  return renderer_.gen_quad_buffers(count);
}

void DantaresRenderer::DeleteQuadLists(GLuint id,int count) {
  renderer_.delete_quad_buffers(id,count);
}

void DantaresRenderer::CompileQuadList(GLuint id,int index,const QuadListData& data) {
  renderer_.compile_quad_buffer(id,index,Renderer::QuadBufferData{
    .tex_handle = data.TextureID,
    .normal = Pos3f{data.Normal.X,data.Normal.Y,data.Normal.Z},
    .vertices = {
      Pos3f{data.Vertices[0].X,data.Vertices[0].Y,data.Vertices[0].Z},
      Pos3f{data.Vertices[1].X,data.Vertices[1].Y,data.Vertices[1].Z},
      Pos3f{data.Vertices[2].X,data.Vertices[2].Y,data.Vertices[2].Z},
      Pos3f{data.Vertices[3].X,data.Vertices[3].Y,data.Vertices[3].Z},
    },
  });
}

void DantaresRenderer::DrawQuadList(GLuint id,int index) {
  renderer_.draw_quad_buffer(id,index);
}

} // namespace ekoscape
