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

void UiFlexGrid::add(std::shared_ptr<UiComp> comp,const CellStyles& styles) {
  cells_.push_back(Cell{
    .comp = std::move(comp),
    .styles = styles,
  });
}

void UiFlexGrid::resize(const Pos3i& pos,const Size2i& size) {
  const auto new_layout = (size.w > size.h) ? Layout::kWide : Layout::kTall;
  const bool layout_changed = (new_layout != layout_);

  if(layout_changed) {
    layout_ = new_layout;
    update_styles();
  }

  std::vector<Cell*> grid(grid_style_.cols * grid_style_.rows,nullptr);
  std::vector row_data_bag(grid_style_.rows,RowData{});

  build_grid(grid,row_data_bag);

  auto base_cell_pos = pos;
  const auto base_cell_size = calc_base_cell_size(size);

  for(int row = 0; row < grid_style_.rows; ++row) {
    const auto& row_data = row_data_bag[row];

    for(int col = 0; col < grid_style_.cols; ++col) {
      auto* cell = grid[col + (row * grid_style_.cols)];

      if(cell != nullptr) {
        if(cell->is_dirty) {
          cell->is_dirty = false;

          const int top_cell_i = col + ((row - 1) * grid_style_.cols);
          const Cell* top_cell = (top_cell_i >= 0) ? grid[top_cell_i] : nullptr;
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
          if(layout_changed || (cell_pos != cell->pos || cell_size != cell->size)) {
            resize_cell(*cell,cell_pos,cell_size);
          }
        }

        base_cell_pos.x += cell->size.w;
        col += (cell->style.colspan - 1);
      } else {
        base_cell_pos.x += base_cell_size.w;
      }
    }

    base_cell_pos.x = pos.x;
  }
}

void UiFlexGrid::build_grid(std::vector<Cell*>& grid,std::vector<RowData>& row_data_bag) {
  std::size_t free_cell_i = 0;

  for(int row = 0; row < grid_style_.rows; ++row) {
    auto& row_data = row_data_bag[row];

    for(int col = 0; col < grid_style_.cols; ++col) {
      auto* cell = grid[col + (row * grid_style_.cols)];

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
        if(cell == nullptr) { continue; }

        cell->is_dirty = true;

        // Set all grid cells in subregion (colspan * rowspan) to cell.
        const int end_sub_col = std::min(col + cell->style.colspan,grid_style_.cols);
        const int end_sub_row = std::min(row + cell->style.rowspan,grid_style_.rows);

        for(int sub_row = row; sub_row < end_sub_row; ++sub_row) {
          for(int sub_col = col; sub_col < end_sub_col; ++sub_col) {
            grid[sub_col + (sub_row * grid_style_.cols)] = cell;
          }
        }
      }

      row_data.total_width_slices += (static_cast<float>(cell->style.colspan) * cell->style.hflex);
      col += (cell->style.colspan - 1);
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

  auto comp_pos = cell.pos;
  auto comp_size = cell.size;

  comp_pos.x += hpadding;
  comp_pos.y += vpadding;
  comp_size.w -= (hpadding * 2);
  comp_size.h -= (vpadding * 2);

  if(cell.style.aspect_ratio != 1.0f) {
    if(comp_size.aspect_ratio() <= cell.style.aspect_ratio) {
      comp_size.h = static_cast<int>(std::round(static_cast<float>(comp_size.w) / cell.style.aspect_ratio));
    } else {
      comp_size.w = static_cast<int>(std::round(static_cast<float>(comp_size.h) * cell.style.aspect_ratio));
    }
  }

  if(comp_size.w < 1) { comp_size.w = 1; }
  if(comp_size.h < 1) { comp_size.h = 1; }

  comp_pos.x += static_cast<int>(
    std::round(static_cast<float>(cell.size.w - comp_size.w) * cell.style.halign)
  );
  comp_pos.y += static_cast<int>(
    std::round(static_cast<float>(cell.size.h - comp_size.h) * cell.style.valign)
  );

  cell.comp->resize(comp_pos,comp_size);
}

void UiFlexGrid::draw(Renderer& ren) {
  for(auto& cell : cells_) {
    if(cell.style.visible == 1) { cell.comp->draw(ren); }
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
  const auto& gs = (layout_ == Layout::kWide) ? grid_styles.wide : grid_styles.tall;

  const int cols = pick_from2(all.cols,gs.cols,1,1);
  int rows = pick_from2(all.rows,gs.rows,0,1);

  if(rows <= 0) {
    // This doesn't take into account rowspan. Use explicit `rows` instead.
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
  const auto& ds = (layout_ == Layout::kWide) ? default_cell_styles.wide : default_cell_styles.tall;

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
  const auto& cs = (layout_ == Layout::kWide) ? cell_styles.wide : cell_styles.tall;

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

UiFlexGrid::boolish UiFlexGrid::pick_from_boolish2(boolish base_opt,boolish alt_opt,boolish fallback_opt) {
  return pick_from2<boolish>(base_opt,alt_opt,fallback_opt,0);
}

UiFlexGrid::boolish UiFlexGrid::pick_from_boolish3(boolish base_opt,boolish alt_opt1,boolish alt_opt2,
                                                   boolish fallback_opt) {
  return pick_from3<boolish>(base_opt,alt_opt1,alt_opt2,fallback_opt,0);
}

const UiFlexGrid::Cell* UiFlexGrid::cell(std::size_t index) const {
  return (index < cells_.size()) ? &cells_[index] : nullptr;
}

} // namespace cybel
