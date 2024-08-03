/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "map.h"

namespace ekoscape {

const Duration Map::kMinRobotDelay = Duration::from_millis(110);

bool Map::is_map_file(const std::string& file) {
  try {
    TextReader reader{file};
    std::string line{};
    int version = -1;

    if(!reader.read_line(line)) { return false; }
    if(!parse_header(line,version,false)) { return false; }

    return version >= kMinSupportedVersion && version <= kMaxSupportedVersion;
  } catch(const EkoScapeError& e) {
    std::cerr << "[WARN] " << e.what() << std::endl;
    return false;
  }
}

Map& Map::load_file(const std::string& file) {
  TextReader reader{file};
  std::string line{};
  char data_c = 0;
  float data_f = 0.0f;
  int data_i = 0;

  if(!reader.read_line(line)) {
    throw EkoScapeError{Util::build_string("Missing header in map [",file,"].")};
  }
  if(!parse_header(line,data_i)) {
    throw EkoScapeError{Util::build_string("Invalid header [",line,"] in map [",file,"].")};
  }
  if(data_i < kMinSupportedVersion || data_i > kMaxSupportedVersion) {
    throw EkoScapeError{Util::build_string("Invalid version [",data_i,"] in map [",file,"].")};
  }
  version_ = data_i;

  if(!reader.read_line(line)) {
    throw EkoScapeError{Util::build_string("Missing title in map [",file,"].")};
  }
  set_title(line);

  if(!reader.read_line(line)) {
    throw EkoScapeError{Util::build_string("Missing author in map [",file,"].")};
  }
  set_author(line);

  if(!reader.read(data_f)) {
    throw EkoScapeError{Util::build_string("Missing turning speed in map [",file,"].")};
  }
  set_turning_speed(data_f);

  if(!reader.read(data_f)) {
    throw EkoScapeError{Util::build_string("Missing walking speed in map [",file,"].")};
  }
  set_walking_speed(data_f);

  if(!reader.seek_and_destroy('\'')
      || !reader.read(data_c)
      || !reader.seek_and_destroy('\'')) {
    throw EkoScapeError{Util::build_string("Missing empty cell space in map [",file,"].")};
  }
  set_empty_cell(SpaceTypes::to_space_type(data_c));

  if(!reader.seek_and_destroy('\'')
      || !reader.read(data_c)
      || !reader.seek_and_destroy('\'')) {
    throw EkoScapeError{Util::build_string("Missing empty player space in map [",file,"].")};
  }
  set_empty_player(SpaceTypes::to_space_type(data_c));

  if(!reader.seek_and_destroy('\'')
      || !reader.read(data_c)
      || !reader.seek_and_destroy('\'')) {
    throw EkoScapeError{Util::build_string("Missing empty robot space in map [",file,"].")};
  }
  set_empty_robot(SpaceTypes::to_space_type(data_c));

  if(!reader.read(data_i)) {
    throw EkoScapeError{Util::build_string("Missing robot delay in map [",file,"].")};
  }
  set_robot_delay(Duration::from_millis(data_i));

  if(!reader.read_line(line) // Finish consuming previous line.
      || !reader.consume_lines_if_empty(1)) {
    throw EkoScapeError{Util::build_string("Missing map grid in map [",file,"].")};
  }

  std::vector<std::string> lines{};
  int width = 0;

  while(reader.read_line(line)) {
    // Ignore last blank line, if there is one.
    if(reader.eof() && line.empty()) { break; }

    lines.push_back(line);

    int len = static_cast<int>(line.length());
    if(len > width) { width = len; }
  }

  if(lines.empty() || width <= 0) {
    throw EkoScapeError{Util::build_string("Missing map grid in map [",file,"].")};
  }

  return parse_grid(lines,width,static_cast<int>(lines.size()));
}

bool Map::parse_header(const std::string& line,int& version,bool warn) {
  std::regex re(R"(^\s*\[EkoScape/v(\d+)\]\s*$)",std::regex::icase);
  std::smatch matches;

  if(!std::regex_match(line,matches,re) || matches.size() != 2) {
    return false;
  }

  try {
    version = std::stoi(matches[1]);
  } catch(const std::invalid_argument& e) {
    if(warn) { std::cerr << "[WARN] " << e.what() << std::endl; }
    return false;
  } catch(const std::out_of_range& e) {
    if(warn) { std::cerr << "[WARN] " << e.what() << std::endl; }
    return false;
  }

  return true;
}

std::string Map::build_header() const {
  return Util::build_string("[EkoScape/v",version_,']');
}

Map& Map::parse_grid(const std::vector<std::string>& lines,int width,int height) {
  clear_spaces();

  if(width <= 0) {
    // Find max line length (width).
    width = 0;

    for(const auto& line: lines) {
      const int len = static_cast<int>(line.length());
      if(len > width) { width = len; }
    }
  }

  const int lines_len = static_cast<int>(lines.size());

  if(height <= 0) { height = lines_len; }
  if(width <= 0 || height <= 0) { return *this; }

  spaces_.resize(width * height);
  width_ = width;
  height_ = height;

  bool has_player = false;
  bool has_end = false;

  // Dantares expects a map where the origin (0,0) is from the bottom left,
  //    instead of the top left, so we match this internally.
  // Therefore, we use `dan_y` to flip it vertically,
  //    and `dan_x` is to just match it visually.
  for(int y = 0; y < height_; ++y) {
    int dan_y = height_ - 1 - y;
    const std::string* line = (y < lines_len) ? &lines.at(y) : nullptr;
    const int line_len = static_cast<int>(line->length());

    for(int x = 0; x < width_; ++x) {
      int dan_x = x;
      SpaceType empty_type = SpaceType::kDeadSpace;
      SpaceType thing_type = SpaceType::kNil;

      if(line != nullptr && x < line_len) {
        SpaceType type = SpaceTypes::to_space_type(line->at(x));

        if(type == SpaceType::kCell) {
          empty_type = empty_cell_;
          thing_type = type;
          ++total_cells_;
        } else if(SpaceTypes::is_player(type)) {
          empty_type = empty_player_;
          player_init_x_ = dan_x;
          player_init_y_ = dan_y;
          player_init_facing_ = SpaceTypes::to_player_facing(type);
          has_player = true;
        } else if(SpaceTypes::is_robot(type)) {
          empty_type = empty_robot_;
          thing_type = type;
        } else {
          if(type == SpaceType::kEnd) { has_end = true; }

          empty_type = type;
        }
      }

      set_space_imp(dan_x,dan_y,Space{empty_type,thing_type});
    }
  }

  if(!has_player) {
    throw EkoScapeError{Util::build_string("Missing a Player space {"
        ,SpaceTypes::value_of(SpaceType::kPlayerNorth)
        ,',',SpaceTypes::value_of(SpaceType::kPlayerSouth)
        ,',',SpaceTypes::value_of(SpaceType::kPlayerEast)
        ,',',SpaceTypes::value_of(SpaceType::kPlayerWest)
        ,"} in the grid of map [",title_,"].")};
  }
  if(!has_end) {
    throw EkoScapeError{Util::build_string("Missing an End space ["
        ,SpaceTypes::value_of(SpaceType::kEnd)
        ,"] in the grid of map [",title_,"].")};
  }

  return *this;
}

Map& Map::clear_spaces() {
  width_ = 0;
  height_ = 0;
  spaces_.clear();
  total_cells_ = 0;
  total_rescues_ = 0;
  player_init_x_ = 0;
  player_init_y_ = 0;
  id_ = -1;

  return *this;
}

Map& Map::add_to(Dantares& dantares,const std::function<void(int,int,Space&,SpaceType)>& handle_space) {
  std::vector<int> int_spaces(width_ * height_,0);

  // Explicitly casting to ensure that `const void*` overload is used.
  id_ = dantares.AddMap(static_cast<const void*>(int_spaces.data()),width_,height_);

  if(id_ == -1 || !dantares.IsMap(id_)) {
    throw EkoScapeError{Util::build_string("Failed to add map [",id_,':',title_,"].")};
  }

  // Temporarily set this map as the current map.
  int curr_map_id = dantares.GetCurrentMap();

  if(!dantares.SetCurrentMap(id_)) {
    if(curr_map_id != -1 && dantares.IsMap(curr_map_id)) {
      dantares.SetCurrentMap(curr_map_id);
    }

    throw EkoScapeError{Util::build_string("Failed to make map [",id_,':',title_,"] current.")};
  }

  for(int y = 0; y < height_; ++y) {
    for(int x = 0; x < width_; ++x) {
      Space& space = space_imp(x,y);
      SpaceType type = space.type();

      update_square(x,y,type,dantares);

      if(handle_space) { handle_space(x,y,space,type); }
    }
  }

  // Reset back to actual current map.
  if(curr_map_id != -1 && dantares.IsMap(curr_map_id)) {
    dantares.SetCurrentMap(curr_map_id); // Ignore any errors.
  }

  return *this;
}

Map& Map::make_current_in(Dantares& dantares) {
  if(id_ == -1 || !dantares.IsMap(id_)) {
    throw EkoScapeError{Util::build_string("Invalid map ID [",id_,':',title_,"]. Call add_to() first.")};
  }
  if(!dantares.SetCurrentMap(id_)) {
    throw EkoScapeError{Util::build_string("Failed to make map [",id_,':',title_,"] current.")};
  }

  int dan_facing = Facings::value_of(player_init_facing_);

  if(!dantares.SetPlayerPosition(player_init_x_,player_init_y_,dan_facing)) {
    throw EkoScapeError{Util::build_string("Failed to set player pos [",dan_facing,":("
        ,player_init_x_,',',player_init_y_,")] for map [",id_,':',title_,"].")};
  }
  if(!dantares.SetTurningSpeed(turning_speed_)) {
    std::cerr << "[WARN] Failed to set turning speed [" << turning_speed_ << "] for map["
        << id_ << ':' << title_ << "]." << std::endl;
    // Don't fail; game is still playable.
  }
  if(!dantares.SetWalkingSpeed(walking_speed_)) {
    std::cerr << "[WARN] Failed to set walking speed [" << walking_speed_ << "] for map["
        << id_ << ':' << title_ << "]." << std::endl;
    // Don't fail; game is still playable.
  }

  return *this;
}

Map& Map::generate_in(Dantares& dantares) {
  if(!dantares.GenerateMap()) {
    throw EkoScapeError{Util::build_string("Failed to generate map [",id_,':',title_,"].")};
  }

  return *this;
}

bool Map::move_thing(int from_x,int from_y,int to_x,int to_y,Dantares& dantares) {
  Space* from_space = this->space(from_x,from_y);

  if(from_space == nullptr || !from_space->has_thing()) { return false; }

  Space* to_space = this->space(to_x,to_y);

  if(to_space == nullptr || to_space->has_thing()) { return false; }

  SpaceType thing_type = from_space->remove_thing();
  update_square(from_x,from_y,from_space->empty_type(),dantares);

  to_space->place_thing(thing_type);
  update_square(to_x,to_y,thing_type,dantares);

  return true;
}

bool Map::remove_thing(int x,int y,Dantares& dantares) {
  Space* space = this->space(x,y);

  if(space == nullptr) { return false; }
  if(!space->has_thing()) { return true; } // This is why move_thing() can't use this method.

  space->remove_thing();
  update_square(x,y,space->empty_type(),dantares);

  return true;
}

bool Map::place_thing(SpaceType type,int x,int y,Dantares& dantares) {
  Space* space = this->space(x,y);

  if(space == nullptr || space->has_thing()) { return false; }

  space->place_thing(type);
  update_square(x,y,type,dantares);

  return true;
}

bool Map::unlock_cell(int x,int y,Dantares& dantares) {
  Space* space = this->space(x,y);

  if(space == nullptr || space->thing_type() != SpaceType::kCell) { return false; }

  space->remove_thing();
  update_square(x,y,space->empty_type(),dantares);
  ++total_rescues_;

  return true;
}

Map& Map::set_title(const std::string& title) {
  title_ = title;

  return *this;
}

Map& Map::set_author(const std::string& author) {
  author_ = author;

  return *this;
}

Map& Map::set_turning_speed(float speed) {
  // 0 uses Dantares' default.
  turning_speed_ = (speed >= 0.0f) ? speed : 0.0f;

  return *this;
}

Map& Map::set_walking_speed(float speed) {
  // 0 uses Dantares' default.
  walking_speed_ = (speed >= 0.0f) ? speed : 0.0f;

  return *this;
}

Map& Map::set_empty_cell(SpaceType type) {
  empty_cell_ = (type != SpaceType::kNil) ? type : SpaceType::kEmpty;

  return *this;
}

Map& Map::set_empty_player(SpaceType type) {
  empty_player_ = (type != SpaceType::kNil) ? type : SpaceType::kEmpty;

  return *this;
}

Map& Map::set_empty_robot(SpaceType type) {
  empty_robot_ = (type != SpaceType::kNil) ? type : SpaceType::kEmpty;

  return *this;
}

Map& Map::set_robot_delay(Duration duration) {
  robot_delay_ = (duration >= kMinRobotDelay) ? duration : kMinRobotDelay;

  return *this;
}

int Map::version() const { return version_; }

const std::string& Map::title() const { return title_; }

const std::string& Map::author() const { return author_; }

float Map::turning_speed() const { return turning_speed_; }

float Map::walking_speed() const { return walking_speed_; }

SpaceType Map::empty_cell() const { return empty_cell_; }

SpaceType Map::empty_player() const { return empty_player_; }

SpaceType Map::empty_robot() const { return empty_robot_; }

const Duration& Map::robot_delay() const { return robot_delay_; }

int Map::width() const { return width_; }

int Map::height() const { return height_; }

Space* Map::space(int x,int y) {
  if(x < 0 || x >= width_ || y < 0 || y >= height_) { return nullptr; }

  return &space_imp(x,y);
}

const Space* Map::space(int x,int y) const {
  if(x < 0 || x >= width_ || y < 0 || y >= height_) { return nullptr; }

  return &space_imp(x,y);
}

int Map::total_cells() const { return total_cells_; }

int Map::total_rescues() const { return total_rescues_; }

int Map::player_init_x() const { return player_init_x_; }

int Map::player_init_y() const { return player_init_y_; }

Facing Map::player_init_facing() const { return player_init_facing_; }

int Map::id() const { return id_; }

void Map::update_square(int x,int y,SpaceType type,Dantares& dantares) {
  dantares.ChangeSquare(x,y,SpaceTypes::value_of(type));

  // Walkability must always be updated after changing the square.
  if(SpaceTypes::is_walkable(type)) {
    dantares.MakeSpaceWalkable(x,y);
  } else {
    dantares.MakeSpaceNonWalkable(x,y);
  }
}

void Map::set_space_imp(int x,int y,Space&& space) {
  spaces_.at(x + (y * width_)) = std::move(space);
}

Space& Map::space_imp(int x,int y) { return spaces_.at(x + (y * width_)); }

const Space& Map::space_imp(int x,int y) const { return spaces_.at(x + (y * width_)); }

std::ostream& operator<<(std::ostream& out,const Map& map) {
  out << map.build_header() << '\n'
      << map.title_  << '\n'
      << map.author_ << '\n'
      << '\n'
      << map.turning_speed_ << ' ' << map.walking_speed_ << '\n'
      << '\n'
      <<  "'" << SpaceTypes::value_of(map.empty_cell_)   << "'"
      << " '" << SpaceTypes::value_of(map.empty_player_) << "'"
      << " '" << SpaceTypes::value_of(map.empty_robot_)  << "'"
      << '\n'
      << map.robot_delay_.round_millis() << '\n';

  // Flip vertically, since internally, we match Dantares where
  //     the origin (0,0) is from the bottom left, instead of the top left.
  for(int y = map.height_ - 1; y >= 0; --y) {
    out << '\n';

    for(int x = 0; x < map.width_; ++x) {
      SpaceType type;

      if(x == map.player_init_x_ && y == map.player_init_y_) {
        type = SpaceTypes::to_player(map.player_init_facing_);
      } else {
        type = map.space_imp(x,y).type();
      }

      out << SpaceTypes::value_of(type);
    }
  }

  return out;
}

} // Namespace.
