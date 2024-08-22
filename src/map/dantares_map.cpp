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

Map& DantaresMap::clear_spaces() {
  Base::clear_spaces();
  id_ = -1;

  return *this;
}

DantaresMap& DantaresMap::add_to_dantares(const SpaceCallback& on_space) {
  std::vector<int> int_spaces(size_.w * size_.h,0);

  // Explicitly casting to ensure that `const void*` overload is used.
  id_ = dantares_.AddMap(static_cast<const void*>(int_spaces.data()),size_.w,size_.h);

  if(id_ == -1 || !dantares_.IsMap(id_)) {
    throw CybelError{Util::build_string("Failed to add map [",id_,':',title_,"].")};
  }

  // Temporarily set this map as the current map.
  int curr_map_id = dantares_.GetCurrentMap();

  if(!dantares_.SetCurrentMap(id_)) {
    // Reset back.
    if(curr_map_id != -1 && dantares_.IsMap(curr_map_id)) {
      dantares_.SetCurrentMap(curr_map_id);
    }

    throw CybelError{Util::build_string("Failed to make map [",id_,':',title_,"] current.")};
  }

  for(Pos2i pos{0,0}; pos.y < size_.h; ++pos.y) {
    for(pos.x = 0; pos.x < size_.w; ++pos.x) {
      Space& space = raw_space(pos);
      SpaceType type = space.type();

      change_square(pos,type);

      if(on_space) { on_space(pos,space,type); }
    }
  }

  // Reset back to actual current map.
  if(curr_map_id != -1 && dantares_.IsMap(curr_map_id)) {
    dantares_.SetCurrentMap(curr_map_id); // Ignore any errors.
  }

  return *this;
}

DantaresMap& DantaresMap::delete_from_dantares() {
  if(id_ == -1) { return *this; }

  if(dantares_.IsMap(id_)) { dantares_.DeleteMap(id_); }
  id_ = -1;

  return *this;
}

DantaresMap& DantaresMap::make_current_in_dantares() {
  if(id_ == -1 || !dantares_.IsMap(id_)) {
    throw CybelError{Util::build_string("Invalid map ID [",id_,':',title_
        ,"]. Add map to Dantares first before making it current.")};
  }
  if(!dantares_.SetCurrentMap(id_)) {
    throw CybelError{Util::build_string("Failed to make map [",id_,':',title_,"] current.")};
  }

  int dan_facing = Facings::value_of(player_init_facing_);

  if(!dantares_.SetPlayerPosition(player_init_pos_.x,player_init_pos_.y,dan_facing)) {
    throw CybelError{Util::build_string("Failed to set player pos [",dan_facing,":("
        ,player_init_pos_.x,',',player_init_pos_.y,")] for map [",id_,':',title_,"].")};
  }
  if(!dantares_.SetTurningSpeed(turning_speed_)) {
    std::cerr << "[WARN] Failed to set turning speed [" << turning_speed_ << "] for map ["
        << id_ << ':' << title_ << "]." << std::endl;
    // Don't fail; game is still playable.
  }
  if(!dantares_.SetWalkingSpeed(walking_speed_)) {
    std::cerr << "[WARN] Failed to set walking speed [" << walking_speed_ << "] for map ["
        << id_ << ':' << title_ << "]." << std::endl;
    // Don't fail; game is still playable.
  }

  return *this;
}

DantaresMap& DantaresMap::generate_in_dantares() {
  if(!dantares_.GenerateMap()) {
    throw CybelError{Util::build_string("Failed to generate map [",id_,':',title_,"].")};
  }

  return *this;
}

bool DantaresMap::move_thing(const Pos2i& from_pos,const Pos2i& to_pos) {
  if(!Base::move_thing(from_pos,to_pos)) { return false; }

  change_square(from_pos,raw_space(from_pos).empty_type());
  change_square(to_pos,raw_space(to_pos).thing_type());
  return true;
}

bool DantaresMap::remove_thing(const Pos2i& pos) {
  if(!Base::remove_thing(pos)) { return false; }

  change_square(pos,raw_space(pos).empty_type());
  return true;
}

bool DantaresMap::place_thing(SpaceType type,const Pos2i& pos) {
  if(!Base::place_thing(type,pos)) { return false; }

  change_square(pos,type);
  return true;
}

bool DantaresMap::unlock_cell(const Pos2i& pos) {
  if(!Base::unlock_cell(pos)) { return false; }

  change_square(pos,raw_space(pos).empty_type());
  return true;
}

bool DantaresMap::set_space(const Pos2i& pos,SpaceType empty_type,SpaceType thing_type) {
  if(!Base::set_space(pos,empty_type,thing_type)) { return false; }

  change_square(pos,raw_space(pos).type()); // Use type() to determine if empty or thing.
  return true;
}

bool DantaresMap::set_empty(const Pos2i& pos,SpaceType type) {
  if(!Base::set_empty(pos,type)) { return false; }

  change_square(pos,raw_space(pos).type()); // Use type() in case there is a thing.
  return true;
}

bool DantaresMap::set_thing(const Pos2i& pos,SpaceType type) {
  if(!Base::set_thing(pos,type)) { return false; }

  change_square(pos,raw_space(pos).type()); // Use type() in case thing is kNil.
  return true;
}

int DantaresMap::id() const { return id_; }

Pos2i DantaresMap::player_pos() const { return {player_x(),player_y()}; }

int DantaresMap::player_x() const { return dantares_.GetPlayerX(); }

int DantaresMap::player_y() const { return dantares_.GetPlayerY(); }

const Space* DantaresMap::player_space() const { return space(player_pos()); }

SpaceType DantaresMap::player_space_type() const {
  return SpaceTypes::to_space_type(dantares_.GetCurrentSpace());
}

Facing DantaresMap::player_facing() const {
  return Facings::to_facing(dantares_.GetPlayerFacing());
}

void DantaresMap::change_square(const Pos2i& pos,SpaceType type) {
  dantares_.ChangeSquare(pos.x,pos.y,SpaceTypes::value_of(type));

  // Walkability must always be updated after changing the square.
  if(SpaceTypes::is_walkable(type)) {
    dantares_.MakeSpaceWalkable(pos.x,pos.y);
  } else {
    dantares_.MakeSpaceNonWalkable(pos.x,pos.y);
  }
}

} // Namespace.
