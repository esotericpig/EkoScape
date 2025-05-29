/*
 * This file is part of EkoScape.
 * Copyright (c) 2025 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ui_flex_grid.h"

namespace cybel {

UiFlexGrid::UiFlexGrid(const GridStyles& grid_styles,const DefaultCellStyles& default_cell_styles) noexcept
  : grid_styles(grid_styles),default_cell_styles(default_cell_styles) {}

void UiFlexGrid::add(std::shared_ptr<UiNode> node,const CellStyles& styles) {
  cells_.push_back(Cell{
    .node = std::move(node),
    .styles = styles,
  });
}

void UiFlexGrid::resize(const Pos3i& pos,const Size2i& size) {
  const auto new_layout = (size.w > size.h) ? UiLayout::kWide : UiLayout::kTall;
  layout_changed_ = (new_layout != layout_);

  if(layout_changed_) {
    layout_ = new_layout;
    update_styles();
  }

  const auto cols = static_cast<std::size_t>(grid_style_.cols);
  const auto rows = static_cast<std::size_t>(grid_style_.rows);
  std::vector<Cell*> grid(cols * rows,nullptr);
  std::vector row_data_bag(rows,RowData{});

  build_grid(grid,row_data_bag);

  auto base_cell_pos = pos;
  const auto base_cell_size = calc_base_cell_size(size);

  for(std::size_t row = 0; row < rows; ++row) {
    const auto& row_data = row_data_bag[row];

    for(std::size_t col = 0; col < cols; ++col) {
      auto* cell = grid[col + (row * cols)];

      if(cell != nullptr) {
        if(cell->is_dirty) {
          cell->is_dirty = false;

          const Cell* top_cell = (row == 0) ? nullptr : grid[col + ((row - 1) * cols)];
          const float width_slice = static_cast<float>(cell->style.colspan) * cell->style.hflex;
          const float width_ratio = width_slice / row_data.total_width_slices;

          const Pos3i cell_pos{
            base_cell_pos.x,
            // Set y using the grid cell above the current cell.
            (top_cell == nullptr) ? base_cell_pos.y : (top_cell->pos.y + top_cell->size.h),
            base_cell_pos.z
          };
          const Size2i cell_size{
            std::max(static_cast<int>(std::round(static_cast<float>(size.w) * width_ratio)),1),
            std::max(base_cell_size.h * cell->style.rowspan,1)
          };

          // Did the layout/pos/size change?
          if(layout_changed_ || (cell_pos != cell->pos || cell_size != cell->size)) {
            resize_cell(*cell,cell_pos,cell_size);
          }
        }

        base_cell_pos.x += cell->size.w;
        // `colspan` is always >= 1, due to merge_cell_styles().
        col += (static_cast<std::size_t>(cell->style.colspan) - 1);
      } else {
        base_cell_pos.x += base_cell_size.w;
      }
    }

    base_cell_pos.x = pos.x;
  }
}

void UiFlexGrid::build_grid(std::vector<Cell*>& grid,std::vector<RowData>& row_data_bag) {
  const auto cols = static_cast<std::size_t>(grid_style_.cols);
  const auto rows = static_cast<std::size_t>(grid_style_.rows);
  std::size_t free_cell_i = 0;

  for(std::size_t row = 0; row < rows; ++row) {
    auto& row_data = row_data_bag[row];

    for(std::size_t col = 0; col < cols && free_cell_i < cells_.size(); ++col) {
      auto* cell = grid[col + (row * cols)];

      // Owner cell? (not part of a subregion)
      if(cell == nullptr) {
        // Grab the next available cell.
        for(; free_cell_i < cells_.size(); ++free_cell_i) {
          auto& free_cell = cells_[free_cell_i];

          if(free_cell.style.visible == 1) {
            cell = &free_cell;
            ++free_cell_i;
            break;
          }
        }

        // Out of cells.
        if(cell == nullptr) { break; }

        cell->is_dirty = true;

        // Set all grid cells in subregion (colspan * rowspan) to cell.
        const auto end_sub_col = std::min(col + static_cast<std::size_t>(cell->style.colspan),cols);
        const auto end_sub_row = std::min(row + static_cast<std::size_t>(cell->style.rowspan),rows);

        for(std::size_t sub_row = row; sub_row < end_sub_row; ++sub_row) {
          for(std::size_t sub_col = col; sub_col < end_sub_col; ++sub_col) {
            grid[sub_col + (sub_row * cols)] = cell;
          }
        }
      }

      row_data.total_width_slices += (static_cast<float>(cell->style.colspan) * cell->style.hflex);
      // `colspan` is always >= 1, due to merge_cell_styles().
      col += (static_cast<std::size_t>(cell->style.colspan) - 1);
    }

    if(row_data.total_width_slices < kMinFloat) { row_data.total_width_slices = kMinFloat; }
  }
}

void UiFlexGrid::resize_cell(Cell& cell,const Pos3i& cell_pos,const Size2i& cell_size) {
  cell.pos = cell_pos;
  cell.size = cell_size;

  // If the available width/height is too small, decrease the padding.
  // Don't round to allow 0 padding.
  static constexpr float kMinPaddingRatio = 0.25f;
  const int hpadding = std::min(cell.style.padding,
                                static_cast<int>(static_cast<float>(cell.size.w) * kMinPaddingRatio));
  const int vpadding = std::min(cell.style.padding,
                                static_cast<int>(static_cast<float>(cell.size.h) * kMinPaddingRatio));

  Pos3i node_pos{
    cell.pos.x + hpadding,
    cell.pos.y + vpadding,
    cell.pos.z
  };
  Size2i node_size{
    std::max(cell.size.w - (hpadding << 1),1),
    std::max(cell.size.h - (vpadding << 1),1)
  };
  const auto inner_cell_size = node_size;

  if(cell.style.aspect_ratio != 1.0f) {
    if(node_size.aspect_ratio() <= cell.style.aspect_ratio) {
      node_size.h = static_cast<int>(std::round(static_cast<float>(node_size.w) / cell.style.aspect_ratio));
      if(node_size.h < 1) { node_size.h = 1; }
    } else {
      node_size.w = static_cast<int>(std::round(static_cast<float>(node_size.h) * cell.style.aspect_ratio));
      if(node_size.w < 1) { node_size.w = 1; }
    }
  }

  node_pos.x += static_cast<int>(
    std::round(static_cast<float>(inner_cell_size.w - node_size.w) * cell.style.halign)
  );
  node_pos.y += static_cast<int>(
    std::round(static_cast<float>(inner_cell_size.h - node_size.h) * cell.style.valign)
  );

  cell.node->resize(node_pos,node_size);
}

void UiFlexGrid::draw(Renderer& ren) {
  for(auto& cell : cells_) {
    if(cell.style.visible == 1) { cell.node->draw(ren); }
  }
}

void UiFlexGrid::update_styles() {
  grid_style_ = merge_grid_styles();
  default_cell_style_ = merge_default_cell_styles();

  for(auto& cell : cells_) {
    cell.style = merge_cell_styles(cell.styles);
  }
}

UiFlexGrid::GridStyle UiFlexGrid::merge_grid_styles() const {
  const auto& all = grid_styles.all;
  const auto& gs = (layout_ == UiLayout::kTall) ? grid_styles.tall : grid_styles.wide;

  const int cols = pick_from2(all.cols,gs.cols,1,1);
  int rows = pick_from2(all.rows,gs.rows,0,1);

  if(rows <= 0) {
    // This doesn't take into account rowspan; use explicit `rows` instead.
    const int guessed_rows = static_cast<int>(
      std::ceil(static_cast<float>(cells_.size()) / static_cast<float>(cols))
    );
    rows = std::max(guessed_rows,1);
  }

  return GridStyle{
    .cols = cols,
    .rows = rows,
  };
}

UiFlexGrid::DefaultCellStyle UiFlexGrid::merge_default_cell_styles() const {
  const auto& all = default_cell_styles.all;
  const auto& ds = (layout_ == UiLayout::kTall) ? default_cell_styles.tall : default_cell_styles.wide;

  const float align = pick_from2(all.align,ds.align,0.0f,0.0f);

  return DefaultCellStyle{
    .visible = pick_from_boolish2(all.visible,ds.visible,1),
    .padding = pick_from2(all.padding,ds.padding,0,0),
    .align = align,
    .halign = pick_from3(align,all.halign,ds.halign,0.0f,0.0f),
    .valign = pick_from3(align,all.valign,ds.valign,0.0f,0.0f),
  };
}

UiFlexGrid::CellStyle UiFlexGrid::merge_cell_styles(const CellStyles& cell_styles) const {
  const auto& ds = default_cell_style_;
  const auto& all = cell_styles.all;
  const auto& cs = (layout_ == UiLayout::kTall) ? cell_styles.tall : cell_styles.wide;

  const float align = pick_from3(ds.align,all.align,cs.align,0.0f,0.0f);

  return CellStyle{
    .visible = pick_from_boolish3(ds.visible,all.visible,cs.visible,1),
    .colspan = pick_from2(all.colspan,cs.colspan,1,1),
    .rowspan = pick_from2(all.rowspan,cs.rowspan,1,1),
    .padding = pick_from3(ds.padding,all.padding,cs.padding,0,0),
    .hflex = pick_from2(all.hflex,cs.hflex,1.0f,kMinFloat),
    .aspect_ratio = pick_from2(all.aspect_ratio,cs.aspect_ratio,1.0f,kMinFloat),
    .align = align,
    .halign = pick_from3(align,all.halign,cs.halign,0.0f,0.0f),
    .valign = pick_from3(align,all.valign,cs.valign,0.0f,0.0f),
  };
}

Size2i UiFlexGrid::calc_base_cell_size(const Size2i& view_size) const {
  const int w = static_cast<int>(
    std::round(static_cast<float>(view_size.w) / static_cast<float>(grid_style_.cols))
  );
  const int h = static_cast<int>(
    std::round(static_cast<float>(view_size.h) / static_cast<float>(grid_style_.rows))
  );

  return Size2i{std::max(w,1),std::max(h,1)};
}

UiFlexGrid::boolish_t UiFlexGrid::pick_from_boolish2(boolish_t base_opt,boolish_t alt_opt,
                                                     boolish_t fallback_opt) {
  return pick_from2<boolish_t>(base_opt,alt_opt,fallback_opt,0);
}

UiFlexGrid::boolish_t UiFlexGrid::pick_from_boolish3(boolish_t base_opt,boolish_t alt_opt1,boolish_t alt_opt2,
                                                     boolish_t fallback_opt) {
  return pick_from3<boolish_t>(base_opt,alt_opt1,alt_opt2,fallback_opt,0);
}

UiLayout UiFlexGrid::layout() const { return layout_; }

bool UiFlexGrid::layout_changed() const { return layout_changed_; }

const UiFlexGrid::Cell* UiFlexGrid::cell(std::size_t index) const {
  return (index < cells_.size()) ? &cells_[index] : nullptr;
}

} // namespace cybel
