/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dantares_map.h"

namespace ekoscape {

DantaresMap::DantaresMap(Dantares& dantares)
    : dantares_(dantares) {}

Map& DantaresMap::clear_grids() {
  Base::clear_grids();
  grid_ids_.clear();

  return *this;
}

void DantaresMap::add_to_dantares(const TexturesSetter& set_textures) {
  if(grids_.empty()) { throw CybelError{Util::build_str("No grids in map [",title_,"].")}; }
  if(grid_index_ < 0 || grid_index_ >= static_cast<int>(grids_.size())) {
    throw CybelError{Util::build_str("Invalid grid index [",grid_index_,"] for map [",title_
        ,"] of size [",grids_.size(),"].")};
  }
  if(player_init_pos_.z < 0 || player_init_pos_.z >= static_cast<int>(grids_.size())) {
    throw CybelError{Util::build_str("Invalid player Z [",player_init_pos_.z,"] for map [",title_
        ,"] of size [",grids_.size(),"].")};
  }

  grid_ids_.resize(grids_.size(),-1);

  for(int z = 0; z < static_cast<int>(grids_.size()); ++z) {
    const auto& grid = grids_[z];
    const Size2i& size = grid->size();
    std::vector<int> int_spaces(size.w * size.h,0);

    // Explicitly casting to ensure use of `const void*` overload.
    const int id = dantares_.AddMap(static_cast<const void*>(int_spaces.data()),size.w,size.h);

    if(id == -1 || !dantares_.IsMap(id)) {
      throw CybelError{Util::build_str("Failed to add map grid [",z,',',id,':',title_,"] to Dantares.")};
    }
    if(!dantares_.SetCurrentMap(id)) {
      throw CybelError{Util::build_str("Failed to make map grid [",z,',',id,':',title_
          ,"] current in Dantares.")};
    }

    grid_ids_[z] = id;

    for(Pos2i pos{0,0}; pos.y < size.h; ++pos.y) {
      for(pos.x = 0; pos.x < size.w; ++pos.x) {
        Space& space = grid->raw_space(pos);
        change_square(pos,space.type());
      }
    }

    if(set_textures) { set_textures(dantares_,z,id); }

    // Must be called after setting the textures.
    if(!dantares_.GenerateMap()) {
      throw CybelError{Util::build_str("Failed to generate map grid [",z,',',id,':',title_
          ,"] in Dantares.")};
    }
  }

  const int z = player_init_pos_.z;
  const int id = grid_ids_[z]; // Bounds checked at top of method.
  const int dan_facing = Facings::value_of(player_init_facing_);

  if(!change_grid(z,true)) {
    throw CybelError{Util::build_str("Failed to change to map grid [",z,',',id,':',title_
        ,"] in Dantares.")};
  }
  if(!dantares_.SetPlayerPosition(player_init_pos_.x,player_init_pos_.y,dan_facing)) {
    throw CybelError{Util::build_str("Failed to set player pos [",dan_facing,":("
        ,player_init_pos_.x,',',player_init_pos_.y,")] for map grid [",z,',',id,':',title_
        ,"] in Dantares.")};
  }
  if(!dantares_.SetTurningSpeed(turning_speed_)) {
    std::cerr << "[WARN] Failed to set turning speed [" << turning_speed_ << "] for map grid ["
        << z << ',' << id << ':' << title_ << "] in Dantares." << std::endl;
    // Don't fail; game is still playable.
  }
  if(!dantares_.SetWalkingSpeed(walking_speed_)) {
    std::cerr << "[WARN] Failed to set walking speed [" << walking_speed_ << "] for map grid ["
        << z << ',' << id << ':' << title_ << "] in Dantares." << std::endl;
    // Don't fail; game is still playable.
  }
}

bool DantaresMap::change_grid(int z) { return change_grid(z,false); }

bool DantaresMap::change_grid(int z,bool force) {
  if(!force && z == grid_index_) { return true; }
  if(!Base::change_grid(z)) { return false; }

  if(z < 0 || z >= static_cast<int>(grid_ids_.size())) {
    std::cerr << "[ERROR] Invalid Z [" << z << "] for map [" << title_ << "] with IDs size ["
        << grid_ids_.size() << "]." << std::endl;
    return false;
  }

  const int id = grid_ids_[z];

  if(id == -1 || !dantares_.IsMap(id)) {
    std::cerr << "[ERROR] Invalid map grid ID [" << z << ',' << id << ':' << title_
        << "] for Dantares; add map to Dantares first before changing grids." << std::endl;
    return false;
  }
  if(!dantares_.SetCurrentMap(id)) {
    std::cerr << "[ERROR] Failed to make map grid [" << z << ',' << id << ':' << title_
        << "] current in Dantares." << std::endl;
    return false;
  }

  const Size2i& grid_size = grids_[z]->size();
  const Pos3i player_pos = this->player_pos();

  if(player_pos.x < 0 || player_pos.x >= grid_size.w
      || player_pos.y < 0 || player_pos.y >= grid_size.h) {
    dantares_.SetPlayerPosition(0,0);
  }

  return true;
}

bool DantaresMap::move_thing(const Pos3i& from_pos,const Pos3i& to_pos) {
  if(!Base::move_thing(from_pos,to_pos)) { return false; }

  change_square(from_pos,raw_space(from_pos).empty_type());
  change_square(to_pos,raw_space(to_pos).thing_type());
  return true;
}

bool DantaresMap::remove_thing(const Pos3i& pos) {
  if(!Base::remove_thing(pos)) { return false; }

  change_square(pos,raw_space(pos).empty_type());
  return true;
}

bool DantaresMap::place_thing(SpaceType type,const Pos3i& pos) {
  if(!Base::place_thing(type,pos)) { return false; }

  change_square(pos,type);
  return true;
}

bool DantaresMap::set_player_pos() {
  const Pos3i player_pos = this->player_pos();
  return dantares_.SetPlayerPosition(player_pos.x,player_pos.y);
}

bool DantaresMap::set_player_pos(const Pos3i& pos) {
  if(!change_grid(pos.z)) { return false; } // If same Z, no change.
  return dantares_.SetPlayerPosition(pos.x,pos.y);
}

bool DantaresMap::set_space(const Pos3i& pos,SpaceType empty_type,SpaceType thing_type) {
  if(!Base::set_space(pos,empty_type,thing_type)) { return false; }

  change_square(pos,raw_space(pos).type()); // Use type() to determine if empty or thing.
  return true;
}

bool DantaresMap::set_empty(const Pos3i& pos,SpaceType type) {
  if(!Base::set_empty(pos,type)) { return false; }

  change_square(pos,raw_space(pos).type()); // Use type() in case there is a thing.
  return true;
}

bool DantaresMap::set_thing(const Pos3i& pos,SpaceType type) {
  if(!Base::set_thing(pos,type)) { return false; }

  change_square(pos,raw_space(pos).type()); // Use type() in case thing is kNil.
  return true;
}

Pos3i DantaresMap::player_pos() const {
  return {dantares_.GetPlayerX(),dantares_.GetPlayerY(),grid_index_};
}

const Space* DantaresMap::player_space() const { return space(player_pos()); }

SpaceType DantaresMap::player_space_type() const {
  return SpaceTypes::to_space_type(dantares_.GetCurrentSpace());
}

Facing DantaresMap::player_facing() const {
  return Facings::to_facing(dantares_.GetPlayerFacing());
}

void DantaresMap::change_square(const Pos2i& pos,SpaceType type) {
  dantares_.ChangeSquare(pos.x,pos.y,SpaceTypes::value_of(type));

  // Must always update walkability after changing the square.
  if(SpaceTypes::is_walkable(type)) {
    dantares_.MakeSpaceWalkable(pos.x,pos.y);
  } else {
    dantares_.MakeSpaceNonWalkable(pos.x,pos.y);
  }
}

void DantaresMap::change_square(const Pos3i& pos,SpaceType type) {
  if(pos.z == grid_index_) {
    change_square(pos.to_pos2<int>(),type);
    return;
  }
  if(pos.z < 0 || pos.z >= static_cast<int>(grid_ids_.size())) { return; }

  const int z_id = grid_ids_[pos.z];
  const int curr_id = dantares_.GetCurrentMap();

  // Luckily, changing the map in Dantares isn't an expensive operation.
  if(z_id == -1 || !dantares_.SetCurrentMap(z_id)) { return; }

  change_square(pos.to_pos2<int>(),type);

  if(curr_id != -1) { dantares_.SetCurrentMap(curr_id); }
}

} // Namespace.
