/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "map.h"

namespace ekoscape {

const Range2i Map::kSupportedVersions{1,1};
const Duration Map::kMinRobotDelay = Duration::from_millis(110);

bool Map::is_map_file(const std::filesystem::path& file) {
  try {
    TextReader reader{file,24}; // Buffer size based on: "[EkoScape/v1999]\r\n"
    std::string line{};
    int version = -1;

    if(!reader.read_line(line)) { return false; }
    if(!parse_header(line,version,false)) { return false; }

    return kSupportedVersions.in_range(version);
  } catch(const CybelError& e) {
    std::cerr << "[WARN] " << e.what() << std::endl;
    return false;
  }
}

Map& Map::load_file(const std::filesystem::path& file,bool meta_only) {
  TextReader reader{file};
  std::string line{};
  char data_c = 0;
  float data_f = 0.0f;
  int data_i = 0;

  if(!reader.read_line(line)) {
    throw CybelError{Util::build_str("Missing header in map [",file,"].")};
  }
  if(!parse_header(line,data_i)) {
    throw CybelError{Util::build_str("Invalid header [",line,"] in map [",file,"].")};
  }
  if(!kSupportedVersions.in_range(data_i)) {
    throw CybelError{Util::build_str("Unsupported version [",data_i,"] in map [",file,"].")};
  }
  version_ = data_i;

  if(!reader.read_line(line)) {
    throw CybelError{Util::build_str("Missing title in map [",file,"].")};
  }
  set_title(line);

  if(!reader.read_line(line)) {
    throw CybelError{Util::build_str("Missing author in map [",file,"].")};
  }
  set_author(line);

  if(!reader.read(data_f)) {
    throw CybelError{Util::build_str("Missing turning speed in map [",file,"].")};
  }
  set_turning_speed(data_f);

  if(!reader.read(data_f)) {
    throw CybelError{Util::build_str("Missing walking speed in map [",file,"].")};
  }
  set_walking_speed(data_f);

  if(!reader.seek_and_destroy('\'')
      || !reader.get(data_c)
      || !reader.seek_and_destroy('\'')) {
    throw CybelError{Util::build_str("Missing default empty space in map [",file,"].")};
  }
  set_default_empty(SpaceTypes::to_space_type(data_c));

  if(!reader.read(data_i)) {
    throw CybelError{Util::build_str("Missing robot delay in map [",file,"].")};
  }
  set_robot_delay(Duration::from_millis(data_i));

  if(meta_only) { return *this; }

  if(!reader.read_line(line) // Finish consuming previous line.
      || !reader.consume_lines_if_empty(1)) {
    throw CybelError{Util::build_str("Missing map grid in map [",file,"].")};
  }

  std::vector<std::string> lines{};
  Size2i size{};

  while(reader.read_line(line)) {
    // Ignore last blank line, if there is one.
    if(reader.eof() && line.empty()) { break; }

    lines.emplace_back(line);

    int len = static_cast<int>(line.length());
    if(len > size.w) { size.w = len; }
  }

  if(lines.empty() || size.w <= 0) {
    throw CybelError{Util::build_str("Missing map grid in map [",file,"].")};
  }
  size.h = static_cast<int>(lines.size());

  return parse_grid(lines,size);
}

Map& Map::load_file_meta(const std::filesystem::path& file) {
  return load_file(file,true);
}

Map& Map::parse_grid(const std::vector<std::string>& lines,Size2i size) {
  clear_spaces();

  if(size.w <= 0) {
    // Find max line length (width).
    size.w = 0;

    for(const auto& line: lines) {
      const int len = static_cast<int>(line.length());
      if(len > size.w) { size.w = len; }
    }
  }

  const int row_count = static_cast<int>(lines.size());
  if(size.h <= 0) { size.h = row_count; }

  if(size.w <= 0 || size.h <= 0) { return *this; }

  spaces_.resize(size.w * size.h);
  size_ = size;

  bool has_player = false;
  bool has_end = false;

  // Dantares expects a map where the origin (0,0) is from the bottom left,
  //    instead of the top left, so we match this internally.
  // Therefore, we use `dan_pos` to flip it vertically.
  for(Pos2i pos{0,0}; pos.y < size_.h; ++pos.y) {
    Pos2i dan_pos{0,size_.h - 1 - pos.y};
    const std::string* line;
    int col_count;

    if(pos.y < row_count) {
      line = &lines.at(pos.y);
      col_count = static_cast<int>(line->length());
    } else {
      line = nullptr;
      col_count = 0;
    }

    for(pos.x = 0; pos.x < size_.w; ++pos.x) {
      dan_pos.x = pos.x;
      auto empty_type = SpaceType::kDeadSpace;
      auto thing_type = SpaceType::kNil;

      if(line != nullptr && pos.x < col_count) {
        SpaceType type = SpaceTypes::to_space_type(line->at(pos.x));

        if(type == SpaceType::kCell) {
          empty_type = default_empty_;
          thing_type = type;
          ++total_cells_;
        } else if(SpaceTypes::is_player(type)) {
          empty_type = default_empty_;
          player_init_pos_ = dan_pos;
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

      set_raw_space(dan_pos,Space{empty_type,thing_type});
    }
  }

  if(!has_player) {
    throw CybelError{Util::build_str(
        "Missing a Player space {"
        ,SpaceTypes::value_of(SpaceType::kPlayerNorth)
        ,',',SpaceTypes::value_of(SpaceType::kPlayerSouth)
        ,',',SpaceTypes::value_of(SpaceType::kPlayerEast)
        ,',',SpaceTypes::value_of(SpaceType::kPlayerWest)
        ,"} in the grid of map [",title_,"]."
    )};
  }
  if(!has_end) {
    throw CybelError{Util::build_str(
        "Missing an End space ["
        ,SpaceTypes::value_of(SpaceType::kEnd)
        ,"] in the grid of map [",title_,"]."
    )};
  }

  return *this;
}

Map& Map::clear_spaces() {
  size_.w = 0;
  size_.h = 0;
  spaces_.clear();
  total_cells_ = 0;
  total_rescues_ = 0;
  player_init_pos_.x = 0;
  player_init_pos_.y = 0;

  return *this;
}

bool Map::move_thing(const Pos2i& from_pos,const Pos2i& to_pos) {
  Space* from_space = mutable_space(from_pos);

  if(from_space == nullptr || !from_space->has_thing()) { return false; }

  Space* to_space = mutable_space(to_pos);

  if(to_space == nullptr || to_space->has_thing()) { return false; }

  SpaceType thing_type = from_space->remove_thing();
  to_space->set_thing(thing_type);
  return true;
}

bool Map::remove_thing(const Pos2i& pos) {
  Space* space = mutable_space(pos);

  if(space == nullptr) { return false; }
  if(!space->has_thing()) { return true; } // This is why move_thing() can't use this method.

  space->remove_thing();
  return true;
}

bool Map::place_thing(SpaceType type,const Pos2i& pos) {
  Space* space = mutable_space(pos);

  if(space == nullptr || space->has_thing()) { return false; }

  space->set_thing(type);
  return true;
}

bool Map::unlock_cell(const Pos2i& pos) {
  Space* space = mutable_space(pos);

  if(space == nullptr || space->thing_type() != SpaceType::kCell) { return false; }

  space->remove_thing();
  ++total_rescues_;
  return true;
}

Map& Map::set_title(const std::string& title) {
  title_ = Util::strip_str(title);
  return *this;
}

Map& Map::set_author(const std::string& author) {
  author_ = Util::strip_str(author);
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

bool Map::set_space(const Pos2i& pos,SpaceType empty_type,SpaceType thing_type) {
  Space* space = mutable_space(pos);

  if(space == nullptr) { return false; }

  space->set_empty(empty_type);
  space->set_thing(thing_type);
  return true;
}

bool Map::set_empty(const Pos2i& pos,SpaceType type) {
  Space* space = mutable_space(pos);

  if(space == nullptr) { return false; }

  space->set_empty(type);
  return true;
}

bool Map::set_thing(const Pos2i& pos,SpaceType type) {
  Space* space = mutable_space(pos);

  if(space == nullptr) { return false; }

  space->set_thing(type);
  return true;
}

std::string Map::build_header() const {
  return Util::build_str("[EkoScape/v",version_,']');
}

int Map::version() const { return version_; }

const std::string& Map::title() const { return title_; }

const std::string& Map::author() const { return author_; }

float Map::turning_speed() const { return turning_speed_; }

float Map::walking_speed() const { return walking_speed_; }

SpaceType Map::default_empty() const { return default_empty_; }

const Duration& Map::robot_delay() const { return robot_delay_; }

const Size2i& Map::size() const { return size_; }

const Space* Map::space(const Pos2i& pos) const {
  if(!size_.in_bounds(pos.x,pos.y)) { return nullptr; }
  return &raw_space(pos);
}

int Map::total_cells() const { return total_cells_; }

int Map::total_rescues() const { return total_rescues_; }

const Pos2i& Map::player_init_pos() const { return player_init_pos_; }

Facing Map::player_init_facing() const { return player_init_facing_; }

bool Map::parse_header(const std::string& line,int& version,bool warn) {
  std::regex re(R"(^\s*\[EkoScape/v(\d+)\]\s*$)",std::regex::icase);
  std::smatch matches{};

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

void Map::set_raw_space(const Pos2i& pos,Space&& space) {
  spaces_.at(pos.x + (pos.y * size_.w)) = std::move(space);
}

Space* Map::mutable_space(const Pos2i& pos) {
  if(!size_.in_bounds(pos.x,pos.y)) { return nullptr; }
  return &raw_space(pos);
}

Space& Map::raw_space(const Pos2i& pos) { return spaces_.at(pos.x + (pos.y * size_.w)); }

const Space& Map::raw_space(const Pos2i& pos) const { return spaces_.at(pos.x + (pos.y * size_.w)); }

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
  for(Pos2i pos{0,map.size_.h - 1}; pos.y >= 0; --pos.y) {
    out << '\n';

    // Find the last non-dead space, so can avoid printing a bunch of trailing x's,
    //     which is how the map files are usually written.
    int width = map.size_.w;

    for(pos.x = width - 1; pos.x >= 0; --pos.x) {
      SpaceType type = map.raw_space(pos).type();

      if(type != SpaceType::kDeadSpace) {
        width = pos.x + 1;
        break;
      }
    }

    // Avoid a completely blank line.
    if(width <= 0 && map.size_.w > 0) { width = 1; }

    for(pos.x = 0; pos.x < width; ++pos.x) {
      SpaceType type;

      if(pos.x == map.player_init_pos_.x && pos.y == map.player_init_pos_.y) {
        type = SpaceTypes::to_player(map.player_init_facing_);
      } else {
        type = map.raw_space(pos).type();
      }

      out << SpaceTypes::value_of(type);
    }
  }

  return out;
}

} // Namespace.
