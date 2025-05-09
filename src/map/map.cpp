/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "map.h"

#include "cybel/str/utf8/str_util.h"
#include "cybel/types/cybel_error.h"

namespace ekoscape {

bool Map::is_map_file(const std::filesystem::path& file) {
  std::error_code err_code{}; // For noexcept overload.

  if(!is_regular_file(file,err_code)) { return false; }

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
  grid_z_ = -1;
  total_cells_ = 0;
  total_rescues_ = 0;
  player_init_pos_ = Pos3i{};

  grids_.clear();

  return *this;
}

Map& Map::load_file(const std::filesystem::path& file,const SpaceCallback& on_space,
                    const DefaultEmptyCallback& on_default_empty,bool meta_only) {
  TextReader reader{file};

  load_metadata(reader,file.string());
  if(meta_only) { return *this; }
  load_grids(reader,on_space,on_default_empty,file.string());

  return *this;
}

void Map::load_metadata(TextReader& reader,std::string_view file) {
  std::string line{};
  char data_c = 0;
  float data_f = 0.0f;
  int data_i = 0;

  if(!reader.read_line(line)) {
    throw CybelError{"Missing header in map [",file,"]."};
  }
  if(!parse_header(line,data_i)) {
    throw CybelError{"Invalid header [",line,"] in map [",file,"]."};
  }
  if(!kSupportedVersions.in_range(data_i)) {
    throw CybelError{"Unsupported version [",data_i,"] in map [",file,"]."};
  }
  version_ = data_i;

  if(!reader.read_line(line)) {
    throw CybelError{"Missing title in map [",file,"]."};
  }
  set_title(line);

  if(!reader.read_line(line)) {
    throw CybelError{"Missing author in map [",file,"]."};
  }
  set_author(line);

  if(!reader.read(data_f)) {
    throw CybelError{"Missing turning speed in map [",file,"]."};
  }
  set_turning_speed(data_f);

  if(!reader.read(data_f)) {
    throw CybelError{"Missing walking speed in map [",file,"]."};
  }
  set_walking_speed(data_f);

  if(!reader.seek_and_destroy('\'') ||
     !reader.get(data_c) ||
     !reader.seek_and_destroy('\'')) {
    throw CybelError{"Missing default empty space in map [",file,"]."};
  }
  set_default_empty(SpaceTypes::to_space_type(data_c));

  if(!reader.read(data_i)) {
    throw CybelError{"Missing robot delay in map [",file,"]."};
  }
  set_robot_delay(Duration::from_millis(data_i));

  // Finish consuming previous line.
  if(!reader.read_line(line)) {
    throw CybelError{"Missing a grid in map [",file,"]."};
  }
}

void Map::load_grids(TextReader& reader,const SpaceCallback& on_space,
                     const DefaultEmptyCallback& on_default_empty,std::string_view file) {
  std::string line{};
  std::vector<std::string> lines{};
  Size2i size{};
  bool has_player = false;
  bool has_end = false;

  clear_grids();

  for(int i = 0; i < Dantares2::MAXMAPS; ++i) {
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
    },on_default_empty,file);
  }

  shrink_grids_to_fit();

  if(grids_.empty()) {
    throw CybelError{"Missing a grid in map [",file,"]."};
  }
  if(!has_player) {
    throw CybelError{
      "Missing a Player space {",
      SpaceTypes::value_of(SpaceType::kPlayerNorth),',',
      SpaceTypes::value_of(SpaceType::kPlayerSouth),',',
      SpaceTypes::value_of(SpaceType::kPlayerEast),',',
      SpaceTypes::value_of(SpaceType::kPlayerWest),
      "} in a grid of map [",file,"]."
    };
  }
  if(!has_end) {
    throw CybelError{"Missing an End space [",SpaceTypes::value_of(SpaceType::kEnd),
                     "] in a grid of map [",file,"]."};
  }
}

Map& Map::load_file_meta(const std::filesystem::path& file) {
  return load_file(file,nullptr,nullptr,true);
}

Map& Map::parse_grid(const std::vector<std::string>& lines,const SpaceCallback& on_space,
                     const DefaultEmptyCallback& on_default_empty) {
  return parse_grid(lines,Size2i{},on_space,on_default_empty);
}

Map& Map::parse_grid(const std::vector<std::string>& lines,Size2i size,const SpaceCallback& on_space,
                     const DefaultEmptyCallback& on_default_empty,std::string_view file) {
  if(file.empty()) { file = title_; }

  if(grids_.size() >= Dantares2::MAXMAPS) {
    throw CybelError{"Too many grids in map [",file,"]; max is ",Dantares2::MAXMAPS,'.'};
  }

  if(size.w <= 0) {
    // Find max line length for width.
    size.w = 0;

    for(const auto& line : lines) {
      const int len = static_cast<int>(line.length());
      if(len > size.w) { size.w = len; }
    }
  }

  const int line_count = static_cast<int>(lines.size());
  if(size.h <= 0) { size.h = line_count; }

  if(size.w <= 0 || size.h <= 0) {
    throw CybelError{"Grid size [",size.w,'x',size.h,"] in map [",file,"] must at least be 1x1."};
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
      line = &lines[static_cast<std::size_t>(pos.y)];
      line_len = static_cast<int>(line->length());
    }

    for(pos.x = 0; pos.x < size.w; ++pos.x) {
      dan_pos.x = pos.x;
      auto empty_type = SpaceType::kVoid;
      auto thing_type = SpaceType::kNil;

      if(line != nullptr && pos.x < line_len) {
        auto type = SpaceTypes::to_space_type((*line)[static_cast<std::size_t>(pos.x)]);

        if(on_space) { type = on_space(dan_pos,type); }

        if(SpaceTypes::is_player(type)) {
          empty_type = default_empty_;
          player_init_pos_ = dan_pos;
          player_init_facing_ = SpaceTypes::to_player_facing(type);
          has_player = true;

          if(on_default_empty) { on_default_empty(dan_pos,empty_type); }
        } else if(SpaceTypes::is_thing(type)) {
          empty_type = default_empty_;
          thing_type = type;

          if(on_default_empty) { on_default_empty(dan_pos,empty_type); }
        } else {
          empty_type = type;
        }

        on_raw_thing_updated(SpaceType::kNil,thing_type);
      }

      grid->unsafe_space(dan_pos).set(empty_type,thing_type);
    }
  }

  grids_.push_back(std::move(grid));

  if(has_player) {
    grid_z_ = z;
  } else if(grid_z_ < 0) {
    grid_z_ = 0;
  }

  return *this;
}

Map& Map::shrink_grids_to_fit() {
  grids_.shrink_to_fit();

  return *this;
}

void Map::add_to_bridge() {}

void Map::on_context_restored() {}

bool Map::move_thing(const Pos3i& from_pos,const Pos3i& to_pos) {
  Space* from_space = mutable_space(from_pos);
  if(from_space == nullptr || !from_space->has_thing()) { return false; }

  Space* to_space = mutable_space(to_pos);
  if(to_space == nullptr || to_space->has_thing()) { return false; }

  const SpaceType thing_type = from_space->remove_thing();
  to_space->set_thing(thing_type);

  update_bridge_space(from_pos,from_space->empty_type());
  update_bridge_space(to_pos,thing_type);

  return true;
}

bool Map::remove_thing(const Pos3i& pos) {
  Space* space = mutable_space(pos);

  if(space == nullptr) { return false; }
  if(!space->has_thing()) { return true; } // Unlike move_thing(), this returns true.

  switch(space->remove_thing()) {
    case SpaceType::kCell:
      ++total_rescues_;
      break;

    default: break;
  }

  update_bridge_space(pos,space->empty_type());

  return true;
}

bool Map::place_thing(SpaceType thing,const Pos3i& pos) {
  if(!SpaceTypes::is_thing(thing)) { return false; }

  Space* space = mutable_space(pos);
  if(space == nullptr || space->has_thing()) { return false; }

  space->set_thing(thing);
  update_bridge_space(pos,thing);

  return true;
}

bool Map::move_player(const Pos3i& pos) {
  if(!change_grid(pos.z)) { return false; }

  return true;
}

bool Map::sync_player_pos() { return move_player(player_pos()); }

bool Map::change_grid(int z) {
  if(z < 0 || z >= static_cast<int>(grids_.size())) { return false; }

  grid_z_ = z;

  return true;
}

Map& Map::set_title(std::string_view title) {
  title_ = utf8::StrUtil::strip(title);

  return *this;
}

Map& Map::set_author(std::string_view author) {
  author_ = utf8::StrUtil::strip(author);

  return *this;
}

Map& Map::set_turning_speed(float speed) {
  turning_speed_ = std::min(std::abs(speed),90.0f);

  return *this;
}

Map& Map::set_walking_speed(float speed) {
  walking_speed_ = (speed >= 0.0f) ? speed : 0.0f;

  return *this;
}

Map& Map::set_default_empty(SpaceType empty) {
  if(SpaceTypes::is_valid_empty(empty)) {
    default_empty_ = empty;
  }

  return *this;
}

Map& Map::set_robot_delay(const Duration& duration) {
  robot_delay_ = (duration >= kMinRobotDelay) ? duration : kMinRobotDelay;

  return *this;
}

bool Map::set_raw_space(const Pos3i& pos,SpaceType empty,SpaceType thing) {
  if(!SpaceTypes::is_valid_empty(empty) || !SpaceTypes::is_thing(thing)) {
    return false;
  }

  Space* space = mutable_space(pos);
  if(space == nullptr) { return false; }

  const auto old_thing = space->thing_type();

  space->set(empty,thing);
  on_raw_thing_updated(old_thing,thing);

  return true;
}

bool Map::set_raw_empty(const Pos3i& pos,SpaceType empty) {
  if(!SpaceTypes::is_valid_empty(empty)) { return false; }

  Space* space = mutable_space(pos);
  if(space == nullptr) { return false; }

  space->set_empty(empty);

  return true;
}

bool Map::set_raw_thing(const Pos3i& pos,SpaceType thing) {
  if(!SpaceTypes::is_thing(thing)) { return false; }

  Space* space = mutable_space(pos);
  if(space == nullptr) { return false; }

  const auto old_thing = space->thing_type();

  space->set_thing(thing);
  on_raw_thing_updated(old_thing,thing);

  return true;
}

bool Map::remove_raw_thing(const Pos3i& pos) {
  Space* space = mutable_space(pos);
  if(space == nullptr) { return false; }

  const auto old_thing = space->remove_thing();
  on_raw_thing_updated(old_thing,space->thing_type());

  return true;
}

void Map::on_raw_thing_updated(SpaceType old_thing,SpaceType new_thing) {
  switch(old_thing) {
    case SpaceType::kCell:
      if(total_cells_ > 0) {
        --total_cells_;
      }
      break;

    default: break;
  }

  switch(new_thing) {
    case SpaceType::kCell:
      ++total_cells_;
      break;

    default: break;
  }
}

void Map::update_bridge_space(const Pos3i& /*pos*/,SpaceType /*type*/) {}

std::string Map::build_header() const {
  // FIXME: Temporary solution, because my compiler doesn't have <format>.
  const std::string placeholder = "{}";
  std::string header = kHeaderFmt;
  const std::size_t i = header.find(placeholder);

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

int Map::grid_count() const { return static_cast<int>(grids_.size()); }

int Map::grid_z() const { return grid_z_; }

Size2i Map::size() const { return size(grid_z_); }

Size2i Map::size(int z) const {
  if(z < 0 || z >= static_cast<int>(grids_.size())) { return Size2i{}; }

  return grids_[static_cast<std::size_t>(z)]->size();
}

Space* Map::mutable_space(const Pos3i& pos) {
  if(pos.z < 0 || pos.z >= static_cast<int>(grids_.size())) { return nullptr; }

  return grids_[static_cast<std::size_t>(pos.z)]->space(pos);
}

const Space* Map::space(const Pos3i& pos) const {
  if(pos.z < 0 || pos.z >= static_cast<int>(grids_.size())) { return nullptr; }

  return grids_[static_cast<std::size_t>(pos.z)]->space(pos);
}

Space& Map::unsafe_space(const Pos3i& pos) {
  return grids_[static_cast<std::size_t>(pos.z)]->unsafe_space(pos);
}

const Space& Map::unsafe_space(const Pos3i& pos) const {
  return grids_[static_cast<std::size_t>(pos.z)]->unsafe_space(pos);
}

int Map::total_cells() const { return total_cells_; }

int Map::total_rescues() const { return total_rescues_; }

const Pos3i& Map::player_init_pos() const { return player_init_pos_; }

Facing Map::player_init_facing() const { return player_init_facing_; }

Pos3i Map::player_pos() const { return Pos3i{}; }

const Space* Map::player_space() const { return nullptr; }

SpaceType Map::player_space_type() const { return SpaceType::kNil; }

Facing Map::player_facing() const { return Facings::kFallback; }

std::ostream& Map::print(bool rstrip) const { return print(std::cout,rstrip); }

std::ostream& Map::print(std::ostream& out,bool rstrip) const {
  out << build_header() << '\n'
      << title_  << '\n'
      << author_ << '\n'
      << '\n'
      << turning_speed_ << ' ' << walking_speed_ << '\n'
      << '\n'
      << "'" << SpaceTypes::value_of(default_empty_) << "'\n"
      << robot_delay_.round_millis();

  for(int z = 0; z < static_cast<int>(grids_.size()); ++z) {
    out << '\n';

    const auto& grid = grids_[static_cast<std::size_t>(z)];

    // Flip vertically, since internally, we match Dantares where
    //     the origin (0,0) is from the bottom left, instead of the top left.
    for(Pos2i pos{0,grid->size().h - 1}; pos.y >= 0; --pos.y) {
      out << '\n';

      int width = grid->size().w;

      if(rstrip) {
        // Find the last non-Void space to avoid printing trailing Voids.
        for(pos.x = width - 1; pos.x >= 0; --pos.x) {
          const SpaceType type = grid->unsafe_space(pos).type();

          if(type != SpaceType::kVoid) {
            width = pos.x + 1;
            break;
          }
        }
      }

      // Avoid a completely blank line, which would indicate the end of this grid.
      if(width <= 0) { width = 1; }

      for(pos.x = 0; pos.x < width; ++pos.x) {
        auto type = SpaceType::kVoid;

        if(z == player_init_pos_.z && pos.x == player_init_pos_.x && pos.y == player_init_pos_.y) {
          type = SpaceTypes::to_player(player_init_facing_);
        } else if(pos.x < grid->size().w) { // True width might be 0, but our adjusted width is 1.
          type = grid->unsafe_space(pos).type();
        }

        out << SpaceTypes::value_of(type);
      }
    }
  }

  return out;
}

std::ostream& operator<<(std::ostream& out,const Map& map) { return map.print(out); }

} // namespace ekoscape
