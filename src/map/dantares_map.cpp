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
  std::vector<int> int_spaces(width_ * height_,0);

  // Explicitly casting to ensure that `const void*` overload is used.
  id_ = dantares_.AddMap(static_cast<const void*>(int_spaces.data()),width_,height_);

  if(id_ == -1 || !dantares_.IsMap(id_)) {
    throw EkoScapeError{Util::build_string("Failed to add map [",id_,':',title_,"].")};
  }

  // Temporarily set this map as the current map.
  int curr_map_id = dantares_.GetCurrentMap();

  if(!dantares_.SetCurrentMap(id_)) {
    // Reset back.
    if(curr_map_id != -1 && dantares_.IsMap(curr_map_id)) {
      dantares_.SetCurrentMap(curr_map_id);
    }

    throw EkoScapeError{Util::build_string("Failed to make map [",id_,':',title_,"] current.")};
  }

  for(int y = 0; y < height_; ++y) {
    for(int x = 0; x < width_; ++x) {
      Space& space = raw_space(x,y);
      SpaceType type = space.type();

      change_square(x,y,type);

      if(on_space) { on_space(x,y,space,type); }
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
    throw EkoScapeError{Util::build_string("Invalid map ID [",id_,':',title_
        ,"]. Add map to Dantares first before making it current.")};
  }
  if(!dantares_.SetCurrentMap(id_)) {
    throw EkoScapeError{Util::build_string("Failed to make map [",id_,':',title_,"] current.")};
  }

  int dan_facing = Facings::value_of(player_init_facing_);

  if(!dantares_.SetPlayerPosition(player_init_x_,player_init_y_,dan_facing)) {
    throw EkoScapeError{Util::build_string("Failed to set player pos [",dan_facing,":("
        ,player_init_x_,',',player_init_y_,")] for map [",id_,':',title_,"].")};
  }
  if(!dantares_.SetTurningSpeed(turning_speed_)) {
    std::cerr << "[WARN] Failed to set turning speed [" << turning_speed_ << "] for map["
        << id_ << ':' << title_ << "]." << std::endl;
    // Don't fail; game is still playable.
  }
  if(!dantares_.SetWalkingSpeed(walking_speed_)) {
    std::cerr << "[WARN] Failed to set walking speed [" << walking_speed_ << "] for map["
        << id_ << ':' << title_ << "]." << std::endl;
    // Don't fail; game is still playable.
  }

  return *this;
}

DantaresMap& DantaresMap::generate_in_dantares() {
  if(!dantares_.GenerateMap()) {
    throw EkoScapeError{Util::build_string("Failed to generate map [",id_,':',title_,"].")};
  }

  return *this;
}

bool DantaresMap::move_thing(int from_x,int from_y,int to_x,int to_y) {
  if(!Base::move_thing(from_x,from_y,to_x,to_y)) { return false; }

  change_square(from_x,from_y,raw_space(from_x,from_y).empty_type());
  change_square(to_x,to_y,raw_space(to_x,to_y).thing_type());

  return true;
}

bool DantaresMap::remove_thing(int x,int y) {
  if(!Base::remove_thing(x,y)) { return false; }

  change_square(x,y,raw_space(x,y).empty_type());

  return true;
}

bool DantaresMap::place_thing(SpaceType type,int x,int y) {
  if(!Base::place_thing(type,x,y)) { return false; }

  change_square(x,y,type);

  return true;
}

bool DantaresMap::unlock_cell(int x,int y) {
  if(!Base::unlock_cell(x,y)) { return false; }

  change_square(x,y,raw_space(x,y).empty_type());

  return true;
}

bool DantaresMap::set_space(int x,int y,SpaceType empty_type,SpaceType thing_type) {
  if(!Base::set_space(x,y,empty_type,thing_type)) { return false; }

  change_square(x,y,raw_space(x,y).type()); // Use type() to determine if empty or thing.

  return true;
}

bool DantaresMap::set_empty(int x,int y,SpaceType type) {
  if(!Base::set_empty(x,y,type)) { return false; }

  change_square(x,y,raw_space(x,y).type()); // Use type() in case there is a thing.

  return true;
}

bool DantaresMap::set_thing(int x,int y,SpaceType type) {
  if(!Base::set_thing(x,y,type)) { return false; }

  change_square(x,y,raw_space(x,y).type()); // Use type() in case thing is kNil.

  return true;
}

int DantaresMap::id() const { return id_; }

int DantaresMap::player_x() const { return dantares_.GetPlayerX(); }

int DantaresMap::player_y() const { return dantares_.GetPlayerY(); }

const Space* DantaresMap::player_space() const { return space(player_x(),player_y()); }

SpaceType DantaresMap::player_space_type() const {
  return SpaceTypes::to_space_type(dantares_.GetCurrentSpace());
}

Facing DantaresMap::player_facing() const {
  return Facings::to_facing(dantares_.GetPlayerFacing());
}

void DantaresMap::change_square(int x,int y,SpaceType type) {
  dantares_.ChangeSquare(x,y,SpaceTypes::value_of(type));

  // Walkability must always be updated after changing the square.
  if(SpaceTypes::is_walkable(type)) {
    dantares_.MakeSpaceWalkable(x,y);
  } else {
    dantares_.MakeSpaceNonWalkable(x,y);
  }
}

} // Namespace.
