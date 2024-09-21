/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "map.h"

namespace ekoscape {

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

bool Map::parse_header(const std::string& line,int& version,bool warn) {
  std::smatch matches{};

  if(!std::regex_match(line,matches,kHeaderRegex) || matches.size() != 2) {
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

Map& Map::clear_grids() {
  grid_index_ = -1;
  total_cells_ = 0;
  total_rescues_ = 0;
  player_init_pos_ = {};

  grids_.clear();

  return *this;
}

Map& Map::load_file(const std::filesystem::path& file,const SpaceCallback& on_space
    ,const DefaultEmptyCallback& on_def_empty,bool meta_only) {
  TextReader reader{file};

  load_metadata(reader,file);
  if(meta_only) { return *this; }
  load_grids(reader,on_space,on_def_empty,file);

  return *this;
}

void Map::load_metadata(TextReader& reader,const std::string& file) {
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

  // Finish consuming previous line.
  if(!reader.read_line(line)) {
    throw CybelError{Util::build_str("Missing a grid in map [",file,"].")};
  }
}

void Map::load_grids(TextReader& reader,const SpaceCallback& on_space
    ,const DefaultEmptyCallback& on_def_empty,const std::string& file) {
  std::string line{};
  std::vector<std::string> lines{};
  Size2i size{};
  bool has_player = false;
  bool has_end = false;

  clear_grids();

  for(int i = 0; i < Dantares::MAXMAPS; ++i) {
    if(!reader.consume_empty_lines()) { break; }

    lines.clear();
    size.set(0,0);

    while(reader.read_line(line) && !line.empty()) {
      lines.push_back(line);

      const int len = static_cast<int>(line.length());
      if(len > size.w) { size.w = len; }
    }

    size.h = static_cast<int>(lines.size());
    if(size.w <= 0 || size.h <= 0) { continue; }

    parse_grid(lines,size,[&](const auto& pos,SpaceType type) {
      if(on_space) { type = on_space(pos,type); }

      if(type == SpaceType::kEnd) {
        has_end = true;
      } else if(SpaceTypes::is_player(type)) {
        has_player = true;
      }

      return type;
    },on_def_empty,file);
  }

  shrink_grids_to_fit();

  if(grids_.empty()) {
    throw CybelError{Util::build_str("Missing a grid in map [",file,"].")};
  }
  if(!has_player) {
    throw CybelError{Util::build_str(
      "Missing a Player space {"
      ,SpaceTypes::value_of(SpaceType::kPlayerNorth)
      ,',',SpaceTypes::value_of(SpaceType::kPlayerSouth)
      ,',',SpaceTypes::value_of(SpaceType::kPlayerEast)
      ,',',SpaceTypes::value_of(SpaceType::kPlayerWest)
      ,"} in a grid of map [",file,"]."
    )};
  }
  if(!has_end) {
    throw CybelError{Util::build_str(
      "Missing an End space ["
      ,SpaceTypes::value_of(SpaceType::kEnd)
      ,"] in a grid of map [",file,"]."
    )};
  }
}

Map& Map::load_file_meta(const std::filesystem::path& file) {
  return load_file(file,nullptr,nullptr,true);
}

Map& Map::parse_grid(const std::vector<std::string>& lines,const SpaceCallback& on_space
    ,const DefaultEmptyCallback& on_def_empty) {
  return parse_grid(lines,{},on_space,on_def_empty);
}

Map& Map::parse_grid(const std::vector<std::string>& lines,Size2i size,const SpaceCallback& on_space
    ,const DefaultEmptyCallback& on_def_empty,std::string file) {
  if(file.empty()) { file = title_; }

  if(grids_.size() >= Dantares::MAXMAPS) {
    throw CybelError{Util::build_str("Too many grids in map [",file,"]; max is ",Dantares::MAXMAPS,'.')};
  }

  if(size.w <= 0) {
    // Find max line length for width.
    size.w = 0;

    for(const auto& line: lines) {
      const int len = static_cast<int>(line.length());
      if(len > size.w) { size.w = len; }
    }
  }

  const int line_count = static_cast<int>(lines.size());
  if(size.h <= 0) { size.h = line_count; }

  if(size.w <= 0 || size.h <= 0) {
    throw CybelError{Util::build_str("Grid size [",size.w,'x',size.h,"] in map [",file
        ,"] must at least be 1x1.")};
  }

  auto grid = std::make_unique<MapGrid>(size);
  const int z = static_cast<int>(grids_.size());
  bool has_player = false;

  // Dantares expects a map where the origin (0,0) is from the bottom left,
  //    instead of the top left, so we match this internally.
  // Therefore, we use `dan_pos` to flip it vertically.
  for(Pos2i pos{0,0}; pos.y < size.h; ++pos.y) {
    Pos3i dan_pos{0,size.h - 1 - pos.y,z};
    const std::string* line = nullptr;
    int line_len = 0;

    if(pos.y < line_count) {
      line = &lines.at(pos.y);
      line_len = static_cast<int>(line->length());
    }

    for(pos.x = 0; pos.x < size.w; ++pos.x) {
      dan_pos.x = pos.x;
      auto empty_type = SpaceType::kDeadSpace;
      auto thing_type = SpaceType::kNil;

      if(line != nullptr && pos.x < line_len) {
        SpaceType type = SpaceTypes::to_space_type(line->at(pos.x));

        if(on_space) { type = on_space(dan_pos,type); }

        if(SpaceTypes::is_player(type)) {
          empty_type = default_empty_;
          player_init_pos_ = dan_pos;
          player_init_facing_ = SpaceTypes::to_player_facing(type);
          has_player = true;

          if(on_def_empty) { on_def_empty(dan_pos,empty_type); }
        } else if(SpaceTypes::is_thing(type)) {
          empty_type = default_empty_;
          thing_type = type;

          if(on_def_empty) { on_def_empty(dan_pos,empty_type); }
        } else {
          empty_type = type;
        }

        switch(type) {
          case SpaceType::kCell:
            ++total_cells_;
            break;

          default: break;
        }
      }

      grid->raw_space(dan_pos).set(empty_type,thing_type);
    }
  }

  grids_.push_back(std::move(grid));

  if(has_player) {
    grid_index_ = z;
  } else if(grid_index_ < 0) {
    grid_index_ = 0;
  }

  return *this;
}

Map& Map::shrink_grids_to_fit() {
  grids_.shrink_to_fit();
  return *this;
}

bool Map::change_grid(int z) {
  if(z < 0 || z >= static_cast<int>(grids_.size())) { return false; }

  grid_index_ = z;
  return true;
}

bool Map::move_thing(const Pos3i& from_pos,const Pos3i& to_pos) {
  Space* from_space = this->mutable_space(from_pos);
  if(from_space == nullptr || !from_space->has_thing()) { return false; }

  Space* to_space = this->mutable_space(to_pos);
  if(to_space == nullptr || to_space->has_thing()) { return false; }

  SpaceType thing_type = from_space->remove_thing();
  to_space->set_thing(thing_type);
  return true;
}

bool Map::remove_thing(const Pos3i& pos) {
  Space* space = this->mutable_space(pos);

  if(space == nullptr) { return false; }
  if(!space->has_thing()) { return true; } // Unlike move_thing(), this returns true.

  switch(space->remove_thing()) {
    case SpaceType::kCell:
      ++total_rescues_;
      break;

    default: break;
  }

  return true;
}

bool Map::place_thing(SpaceType type,const Pos3i& pos) {
  Space* space = this->mutable_space(pos);
  if(space == nullptr || space->has_thing()) { return false; }

  space->set_thing(type);
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
  // 0 uses Dantares' default value.
  turning_speed_ = (speed >= 0.0f) ? speed : 0.0f;
  return *this;
}

Map& Map::set_walking_speed(float speed) {
  // 0 uses Dantares' default value.
  walking_speed_ = (speed >= 0.0f) ? speed : 0.0f;
  return *this;
}

Map& Map::set_default_empty(SpaceType type) {
  // Cannot have 2+ things on a single space.
  if(type == SpaceType::kNil || SpaceTypes::is_player(type) || SpaceTypes::is_thing(type)) {
    default_empty_ = SpaceType::kEmpty;
  } else {
    default_empty_ = type;
  }

  return *this;
}

Map& Map::set_robot_delay(Duration duration) {
  robot_delay_ = (duration >= kMinRobotDelay) ? duration : kMinRobotDelay;
  return *this;
}

bool Map::set_space(const Pos3i& pos,SpaceType empty_type,SpaceType thing_type) {
  Space* space = this->mutable_space(pos);
  if(space == nullptr) { return false; }

  space->set_empty(empty_type);
  space->set_thing(thing_type);
  return true;
}

bool Map::set_empty(const Pos3i& pos,SpaceType type) {
  Space* space = this->mutable_space(pos);
  if(space == nullptr) { return false; }

  space->set_empty(type);
  return true;
}

bool Map::set_thing(const Pos3i& pos,SpaceType type) {
  Space* space = this->mutable_space(pos);
  if(space == nullptr) { return false; }

  space->set_thing(type);
  return true;
}

std::string Map::build_header() const {
  // FIXME: Temporary solution, because my compiler doesn't have <format>.
  const std::string placeholder = "{}";
  std::string header = kHeaderFmt;
  std::size_t i = header.find(placeholder);

  if(i == std::string::npos) {
    throw CybelError{"Invalid kHeaderFmt: " + kHeaderFmt + "."};
  }

  return header.replace(i,placeholder.length(),std::to_string(version_));
}

int Map::version() const { return version_; }

const std::string& Map::title() const { return title_; }

const std::string& Map::author() const { return author_; }

float Map::turning_speed() const { return turning_speed_; }

float Map::walking_speed() const { return walking_speed_; }

SpaceType Map::default_empty() const { return default_empty_; }

const Duration& Map::robot_delay() const { return robot_delay_; }

int Map::grid_z() const { return grid_index_; }

Size2i Map::size() const { return size(grid_index_); }

Size2i Map::size(int z) const {
  if(z < 0 || z >= static_cast<int>(grids_.size())) { return {}; }
  return grids_[z]->size();
}

Space* Map::mutable_space(const Pos3i& pos) {
  if(pos.z < 0 || pos.z >= static_cast<int>(grids_.size())) { return nullptr; }
  return grids_[pos.z]->space(pos);
}

const Space* Map::space(const Pos3i& pos) const {
  if(pos.z < 0 || pos.z >= static_cast<int>(grids_.size())) { return nullptr; }
  return grids_[pos.z]->space(pos);
}

Space& Map::raw_space(const Pos3i& pos) { return grids_.at(pos.z)->raw_space(pos); }

const Space& Map::raw_space(const Pos3i& pos) const { return grids_.at(pos.z)->raw_space(pos); }

int Map::total_cells() const { return total_cells_; }

int Map::total_rescues() const { return total_rescues_; }

const Pos3i& Map::player_init_pos() const { return player_init_pos_; }

Facing Map::player_init_facing() const { return player_init_facing_; }

std::ostream& Map::print(bool rstrip_dead_spaces) const { return print(std::cout,rstrip_dead_spaces); }

std::ostream& Map::print(std::ostream& out,bool rstrip_dead_spaces) const {
  out << build_header() << '\n'
      << title_  << '\n'
      << author_ << '\n'
      << '\n'
      << turning_speed_ << ' ' << walking_speed_ << '\n'
      << '\n'
      <<  "'" << SpaceTypes::value_of(default_empty_) << "'\n"
      << robot_delay_.round_millis();

  for(int z = 0; z < static_cast<int>(grids_.size()); ++z) {
    out << '\n';

    const auto& grid = grids_[z];

    // Flip vertically, since internally, we match Dantares where
    //     the origin (0,0) is from the bottom left, instead of the top left.
    for(Pos2i pos{0,grid->size().h - 1}; pos.y >= 0; --pos.y) {
      out << '\n';

      int width = grid->size().w;

      if(rstrip_dead_spaces) {
        // Find the last non-dead space to avoid printing trailing x's.
        for(pos.x = width - 1; pos.x >= 0; --pos.x) {
          SpaceType type = grid->raw_space(pos).type();

          if(type != SpaceType::kDeadSpace) {
            width = pos.x + 1;
            break;
          }
        }
      }

      // Avoid a completely blank line, which would indicate the end of this grid.
      if(width <= 0) { width = 1; }

      for(pos.x = 0; pos.x < width; ++pos.x) {
        auto type = SpaceType::kDeadSpace;

        if(z == player_init_pos_.z && pos.x == player_init_pos_.x && pos.y == player_init_pos_.y) {
          type = SpaceTypes::to_player(player_init_facing_);
        } else if(pos.x < grid->size().w) { // True width might be 0, but our adjusted width is 1.
          type = grid->raw_space(pos).type();
        }

        out << SpaceTypes::value_of(type);
      }
    }
  }

  return out;
}

std::ostream& operator<<(std::ostream& out,const Map& map) { return map.print(out); }

} // Namespace.
