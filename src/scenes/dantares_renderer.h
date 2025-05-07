/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_SCENES_DANTARES_RENDERER_H_
#define EKOSCAPE_SCENES_DANTARES_RENDERER_H_

#include "common.h"

#include "cybel/gfx/renderer.h"

namespace ekoscape {

class DantaresRenderer final : public Dantares2::RendererClass
{
public:
  explicit DantaresRenderer(Renderer& renderer) noexcept;

  void BeginDraw() override;
  void EndDraw() override;

  void TranslateModelMatrix(float x,float y,float z) override;
  void RotateModelMatrix(float angle,float x,float y,float z) override;
  void UpdateModelMatrix() override;
  void PushModelMatrix() override;
  void PopModelMatrix() override;

  GLuint GenerateQuadLists(int count) override;
  void DeleteQuadLists(GLuint id,int count) override;
  void CompileQuadList(GLuint id,int index,const QuadListData& data) override;
  void DrawQuadList(GLuint id,int index) override;

private:
  Renderer& renderer_;
};

} // namespace ekoscape
#endif
