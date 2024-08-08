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
    TextReader reader{file,24}; // Buffer size based on: "[EkoScape/v1999]\r\n"
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
    throw EkoScapeError{Util::build_string("Unsupported version [",data_i,"] in map [",file,"].")};
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
    throw EkoScapeError{Util::build_string("Missing default empty space in map [",file,"].")};
  }
  set_default_empty(SpaceTypes::to_space_type(data_c));

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

  const int row_count = static_cast<int>(lines.size());

  if(height <= 0) { height = row_count; }
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
    const std::string* line = (y < row_count) ? &lines.at(y) : nullptr;
    const int column_count = static_cast<int>(line->length());

    for(int x = 0; x < width_; ++x) {
      int dan_x = x;
      SpaceType empty_type = SpaceType::kDeadSpace;
      SpaceType thing_type = SpaceType::kNil;

      if(line != nullptr && x < column_count) {
        SpaceType type = SpaceTypes::to_space_type(line->at(x));

        if(type == SpaceType::kCell) {
          empty_type = default_empty_;
          thing_type = type;
          ++total_cells_;
        } else if(SpaceTypes::is_player(type)) {
          empty_type = default_empty_;
          player_init_x_ = dan_x;
          player_init_y_ = dan_y;
          player_init_facing_ = SpaceTypes::to_player_facing(type);
          has_player = true;
        } else if(SpaceTypes::is_robot(type)) {
          empty_type = default_empty_;
          thing_type = type;
        } else {
          if(type == SpaceType::kEnd) { has_end = true; }

          empty_type = type;
        }
      }

      set_raw_space(dan_x,dan_y,Space{empty_type,thing_type});
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

  return *this;
}

bool Map::move_thing(int from_x,int from_y,int to_x,int to_y) {
  Space* from_space = mutable_space(from_x,from_y);

  if(from_space == nullptr || !from_space->has_thing()) { return false; }

  Space* to_space = mutable_space(to_x,to_y);

  if(to_space == nullptr || to_space->has_thing()) { return false; }

  SpaceType thing_type = from_space->remove_thing();
  to_space->set_thing(thing_type);

  return true;
}

bool Map::remove_thing(int x,int y) {
  Space* space = mutable_space(x,y);

  if(space == nullptr) { return false; }
  if(!space->has_thing()) { return true; } // This is why move_thing() can't use this method.

  space->remove_thing();

  return true;
}

bool Map::place_thing(SpaceType type,int x,int y) {
  Space* space = mutable_space(x,y);

  if(space == nullptr || space->has_thing()) { return false; }

  space->set_thing(type);

  return true;
}

bool Map::unlock_cell(int x,int y) {
  Space* space = mutable_space(x,y);

  if(space == nullptr || space->thing_type() != SpaceType::kCell) { return false; }

  space->remove_thing();
  ++total_rescues_;

  return true;
}

Map& Map::set_title(const std::string& title) {
  title_ = Util::strip(title);
  return *this;
}

Map& Map::set_author(const std::string& author) {
  author_ = Util::strip(author);
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

Map& Map::set_default_empty(SpaceType type) {
  default_empty_ = (type != SpaceType::kNil) ? type : SpaceType::kEmpty;
  return *this;
}

Map& Map::set_robot_delay(Duration duration) {
  robot_delay_ = (duration >= kMinRobotDelay) ? duration : kMinRobotDelay;
  return *this;
}

bool Map::set_space(int x,int y,SpaceType empty_type,SpaceType thing_type) {
  Space* space = mutable_space(x,y);

  if(space == nullptr) { return false; }

  space->set_empty(empty_type);
  space->set_thing(thing_type);

  return true;
}

bool Map::set_empty(int x,int y,SpaceType type) {
  Space* space = mutable_space(x,y);

  if(space == nullptr) { return false; }

  space->set_empty(type);

  return true;
}

bool Map::set_thing(int x,int y,SpaceType type) {
  Space* space = mutable_space(x,y);

  if(space == nullptr) { return false; }

  space->set_thing(type);

  return true;
}

int Map::version() const { return version_; }

const std::string& Map::title() const { return title_; }

const std::string& Map::author() const { return author_; }

float Map::turning_speed() const { return turning_speed_; }

float Map::walking_speed() const { return walking_speed_; }

SpaceType Map::default_empty() const { return default_empty_; }

const Duration& Map::robot_delay() const { return robot_delay_; }

int Map::width() const { return width_; }

int Map::height() const { return height_; }

const Space* Map::space(int x,int y) const {
  if(x < 0 || x >= width_ || y < 0 || y >= height_) { return nullptr; }

  return &raw_space(x,y);
}

int Map::total_cells() const { return total_cells_; }

int Map::total_rescues() const { return total_rescues_; }

int Map::player_init_x() const { return player_init_x_; }

int Map::player_init_y() const { return player_init_y_; }

Facing Map::player_init_facing() const { return player_init_facing_; }

void Map::set_raw_space(int x,int y,Space&& space) {
  spaces_.at(x + (y * width_)) = std::move(space);
}

Space* Map::mutable_space(int x,int y) {
  if(x < 0 || x >= width_ || y < 0 || y >= height_) { return nullptr; }

  return &raw_space(x,y);
}

Space& Map::raw_space(int x,int y) { return spaces_.at(x + (y * width_)); }

const Space& Map::raw_space(int x,int y) const { return spaces_.at(x + (y * width_)); }

std::ostream& operator<<(std::ostream& out,const Map& map) {
  out << map.build_header() << '\n'
      << map.title_  << '\n'
      << map.author_ << '\n'
      << '\n'
      << map.turning_speed_ << ' ' << map.walking_speed_ << '\n'
      << '\n'
      <<  "'" << SpaceTypes::value_of(map.default_empty_) << "'\n"
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
        type = map.raw_space(x,y).type();
      }

      out << SpaceTypes::value_of(type);
    }
  }

  return out;
}

} // Namespace.
