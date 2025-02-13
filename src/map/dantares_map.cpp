/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dantares_map.h"

#include "cybel/types/cybel_error.h"

namespace ekoscape {

DantaresMap::DantaresMap(Dantares2& dantares,const TexturesSetter& set_texs)
  : dantares_(dantares),set_texs_(set_texs) {}

Map& DantaresMap::clear_grids() {
  Map::clear_grids();

  for(const auto id : grid_ids_) {
    dantares_.DeleteMap(id);
  }
  grid_ids_.clear();

  return *this;
}

Map& DantaresMap::add_to_bridge() {
  if(grids_.empty()) { throw CybelError{"No grids in map [",title_,"]."}; }
  if(grid_index_ < 0 || grid_index_ >= static_cast<int>(grids_.size())) {
    throw CybelError{"Invalid grid index [",grid_index_,"] for map [",title_,"] of size [",
                     grids_.size(),"]."};
  }
  if(player_init_pos_.z < 0 || player_init_pos_.z >= static_cast<int>(grids_.size())) {
    throw CybelError{"Invalid player Z [",player_init_pos_.z,"] for map [",title_,"] of size [",
                     grids_.size(),"]."};
  }

  grid_ids_.resize(grids_.size(),-1);

  for(int z = 0; z < static_cast<int>(grids_.size()); ++z) {
    const auto& grid = grids_[z];
    const Size2i& size = grid->size();
    std::vector<int> int_spaces(size.w * size.h,0);

    // Explicitly casting to ensure use of `const void*` overload.
    const int id = dantares_.AddMap(static_cast<const void*>(int_spaces.data()),size.w,size.h);

    if(id == -1 || !dantares_.IsMap(id)) {
      throw CybelError{"Failed to add map grid [",z,',',id,':',title_,"] to Dantares."};
    }
    if(!dantares_.SetCurrentMap(id)) {
      throw CybelError{"Failed to make map grid [",z,',',id,':',title_,"] current in Dantares."};
    }

    grid_ids_[z] = id;

    for(Pos2i pos{0,0}; pos.y < size.h; ++pos.y) {
      for(pos.x = 0; pos.x < size.w; ++pos.x) {
        Space& space = grid->unsafe_space(pos);
        update_bridge_space(pos.x,pos.y,space.type());
      }
    }

    if(set_texs_) { set_texs_(dantares_,z,id); }

    // Must be called after setting the textures.
    if(!dantares_.GenerateMap()) {
      throw CybelError{"Failed to generate map grid [",z,',',id,':',title_,"] in Dantares."};
    }
  }

  const int z = player_init_pos_.z;
  const int id = grid_ids_[z]; // Bounds checked at top of method.
  const int dan_facing = Facings::value_of(player_init_facing_);

  if(!change_grid(z,true)) {
    throw CybelError{"Failed to change to map grid [",z,',',id,':',title_,"] in Dantares."};
  }
  if(!dantares_.SetPlayerPosition(player_init_pos_.x,player_init_pos_.y,dan_facing)) {
    throw CybelError{"Failed to set player pos [",dan_facing,":(",player_init_pos_.x,',',player_init_pos_.y,
                     ")] for map grid [",z,',',id,':',title_,"] in Dantares."};
  }
  if(!dantares_.SetTurningSpeed(turning_speed_)) {
    std::cerr << "[WARN] Failed to set turning speed [" << turning_speed_ << "] for map grid ["
              << z << ',' << id << ':' << title_ << "] in Dantares." << std::endl;
    // Don't fail; map is still playable.
  }
  if(!dantares_.SetWalkingSpeed(walking_speed_)) {
    std::cerr << "[WARN] Failed to set walking speed [" << walking_speed_ << "] for map grid ["
              << z << ',' << id << ':' << title_ << "] in Dantares." << std::endl;
    // Don't fail; map is still playable.
  }

  return *this;
}

bool DantaresMap::move_player(const Pos3i& pos) {
  if(!Map::move_player(pos)) { return false; } // Calls change_grid(z) if necessary.

  return dantares_.SetPlayerPosition(pos.x,pos.y);
}

bool DantaresMap::sync_player_pos() {
  const Pos3i player_pos = this->player_pos();

  return dantares_.SetPlayerPosition(player_pos.x,player_pos.y);
}

bool DantaresMap::change_grid(int z) { return change_grid(z,false); }

bool DantaresMap::change_grid(int z,bool force) {
  if(!force && z == grid_index_) { return true; }
  if(!Map::change_grid(z)) { return false; }

  if(z < 0 || z >= static_cast<int>(grid_ids_.size())) {
    std::cerr << "[ERROR] Invalid Z [" << z << "] for map [" << title_ << "] with IDs size ["
              << grid_ids_.size() << "]." << std::endl;
    return false;
  }

  const int id = grid_ids_[z];

  if(id == -1 || !dantares_.IsMap(id)) {
    std::cerr << "[ERROR] Invalid map grid ID [" << z << ',' << id << ':' << title_
              << "] for Dantares; call add_to_bridge() at least once before changing grids." << std::endl;
    return false;
  }
  if(!dantares_.SetCurrentMap(id)) {
    std::cerr << "[ERROR] Failed to make map grid [" << z << ',' << id << ':' << title_
              << "] current in Dantares." << std::endl;
    return false;
  }

  const Size2i& grid_size = grids_[z]->size();
  const Pos3i player_pos = this->player_pos();

  if((player_pos.x < 0 || player_pos.x >= grid_size.w) ||
     (player_pos.y < 0 || player_pos.y >= grid_size.h)) {
    dantares_.SetPlayerPosition(0,0);
  }

  return true;
}

void DantaresMap::update_bridge_space(const Pos3i& pos,SpaceType type) {
  if(pos.z == grid_index_) {
    update_bridge_space(pos.x,pos.y,type);
    return;
  }
  if(pos.z < 0 || pos.z >= static_cast<int>(grid_ids_.size())) {
    std::cerr << "[WARN] Failed to update Dantares space with invalid Z [" << pos.z << "] and IDs size ["
              << grid_ids_.size() << "] for map [" << title_ << "]." << std::endl;
    return;
  }

  const int z_id = grid_ids_[pos.z];
  const int curr_id = dantares_.GetCurrentMap();

  // Luckily, changing the map in Dantares isn't an expensive operation.
  if(z_id == -1 || !dantares_.SetCurrentMap(z_id)) {
    std::cerr << "[WARN] Failed to update Dantares space with invalid map grid ID [" << z_id << "] from Z ["
              << pos.z << "] for map [" << title_ << "]." << std::endl;
    return;
  }

  update_bridge_space(pos.x,pos.y,type);

  if(curr_id != -1) { dantares_.SetCurrentMap(curr_id); }
}

void DantaresMap::update_bridge_space(int x,int y,SpaceType type) {
  dantares_.ChangeSquare(x,y,SpaceTypes::value_of(type));

  // Must always update walkability after changing the square.
  if(SpaceTypes::is_walkable(type)) {
    dantares_.MakeSpaceWalkable(x,y);
  } else {
    dantares_.MakeSpaceNonWalkable(x,y);
  }
}

Pos3i DantaresMap::player_pos() const {
  return Pos3i{dantares_.GetPlayerX(),dantares_.GetPlayerY(),grid_index_};
}

const Space* DantaresMap::player_space() const { return space(player_pos()); }

SpaceType DantaresMap::player_space_type() const {
  return SpaceTypes::to_space_type(dantares_.GetCurrentSpace());
}

Facing DantaresMap::player_facing() const {
  return Facings::to_facing(dantares_.GetPlayerFacing());
}

} // Namespace.
