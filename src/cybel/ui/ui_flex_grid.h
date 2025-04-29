/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CYBEL_UI_UI_FLEX_GRID_H_
#define CYBEL_UI_UI_FLEX_GRID_H_

#include "cybel/common.h"

#include "cybel/ui/ui_node.h"

namespace cybel {

class UiFlexGrid : public UiNode {
public:
  using boolish_t = std::int8_t; /// -1, 0, or 1.

  struct GridStyle {
    int cols = -1;
    int rows = -1;
  };

  struct GridStyles {
    GridStyle all{};
    GridStyle wide{};
    GridStyle tall{};
  };

  struct DefaultCellStyle {
    boolish_t visible = -1;
    int padding = -1;
    float align = -1.0f;
    float halign = -1.0f;
    float valign = -1.0f;
  };

  struct DefaultCellStyles {
    DefaultCellStyle all{};
    DefaultCellStyle wide{};
    DefaultCellStyle tall{};
  };

  struct CellStyle {
    boolish_t visible = -1;
    int colspan = -1;
    int rowspan = -1;
    int padding = -1;
    float hflex = -1.0f;
    float aspect_ratio = -1.0f;
    float align = -1.0f;
    float halign = -1.0f;
    float valign = -1.0f;
  };

  struct CellStyles {
    CellStyle all{};
    CellStyle wide{};
    CellStyle tall{};
  };

  struct Cell {
    std::shared_ptr<UiNode> node{};
    CellStyles styles{};

    CellStyle style{};
    bool is_dirty = true;
    Pos3i pos{};
    Size2i size{};
  };

  GridStyles grid_styles{};
  DefaultCellStyles default_cell_styles{};

  using UiNode::resize;

  explicit UiFlexGrid() noexcept = default;
  explicit UiFlexGrid(const GridStyles& grid_styles,const DefaultCellStyles& default_cell_styles) noexcept;

  void add(std::shared_ptr<UiNode> node,const CellStyles& styles);

  void resize(const Pos3i& pos,const Size2i& size) override;
  void draw(Renderer& ren) override;

  void update_styles();

  /**
   * This is just for debugging purposes.
   */
  const Cell* cell(std::size_t index) const;

private:
  enum class Layout : std::uint8_t {
    kUnknown,
    kWide,
    kTall,
  };

  struct RowData {
    float total_width_slices = 0.0f;
  };

  static constexpr float kMinFloat = 0.01f; // NOTE: Must not be 0.

  std::vector<Cell> cells_{};

  Layout layout_ = Layout::kUnknown;
  GridStyle grid_style_{};
  DefaultCellStyle default_cell_style_{};

  void build_grid(std::vector<Cell*>& grid,std::vector<RowData>& row_data_bag);
  void resize_cell(Cell& cell,const Pos3i& cell_pos,const Size2i& cell_size);

  GridStyle merge_grid_styles() const;
  DefaultCellStyle merge_default_cell_styles() const;
  CellStyle merge_cell_styles(const CellStyles& cell_styles) const;
  Size2i calc_base_cell_size(const Size2i& view_size) const;

  template <typename T>
  static T pick_from2(T base_opt,T alt_opt,T fallback_opt,T min_opt);
  template <typename T>
  static T pick_from3(T base_opt,T alt_opt1,T alt_opt2,T fallback_opt,T min_opt);
  static boolish_t pick_from_boolish2(boolish_t base_opt,boolish_t alt_opt,boolish_t fallback_opt);
  static boolish_t pick_from_boolish3(boolish_t base_opt,boolish_t alt_opt1,boolish_t alt_opt2,
                                      boolish_t fallback_opt);
};

template <typename T>
T UiFlexGrid::pick_from2(T base_opt,T alt_opt,T fallback_opt,T min_opt) {
  auto opt = (alt_opt >= min_opt) ? alt_opt : base_opt;

  if(opt < min_opt) { opt = fallback_opt; }

  return opt;
}

template <typename T>
T UiFlexGrid::pick_from3(T base_opt,T alt_opt1,T alt_opt2,T fallback_opt,T min_opt) {
  auto opt = (alt_opt2 >= min_opt) ? alt_opt2
                                   : ((alt_opt1 >= min_opt) ? alt_opt1 : base_opt);

  if(opt < min_opt) { opt = fallback_opt; }

  return opt;
}

} // namespace cybel
#endif
