/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_MAP_H_
#define EKOSCAPE_MAP_MAP_H_

#include "common.h"

#include "cybel/io/text_reader.h"
#include "cybel/types/duration.h"
#include "cybel/types/pos.h"
#include "cybel/types/range.h"
#include "cybel/types/size.h"

#include "map/facing.h"
#include "map/map_grid.h"
#include "map/space.h"
#include "map/space_type.h"

#include <filesystem>
#include <functional>
#include <regex>
#include <vector>

namespace ekoscape {

/**
 * Dantares expects a map where the origin (0,0) is from the bottom left, instead of the top left.
 * Because of this, the internal Spaces (Xs & Ys) are stored like this as well.
 * When parsing a Map Grid (and on output), the lines are flipped vertically to accommodate this.
 *
 * Besides loading & parsing a Map file, this class can also be used for creating a Map file:
 *   @code
 *   std::vector<std::string> grid1 = {
 *     "xxxxxxxxxxxx",
 *     "x>         x",
 *     "x#####@### x",
 *     "x          x",
 *     "x ####@####x",
 *     "x     !   0x",
 *     "xxxxxxxxxxxx"
 *   };
 *   std::vector<std::string> grid2 = {
 *     "xxxxxx",
 *     "x0  $x",
 *     "xxxxxx"
 *   };
 *
 *   Map map{};
 *   map.set_title("EZ")
 *      .set_author("Bradz")
 *      .set_turning_speed(5.0f)
 *      .set_walking_speed(15.0f)
 *      .set_default_empty(SpaceType::kWhiteFloor)
 *      .set_robot_delay(Duration::from_millis(2000))
 *      // Parse the grids last! Else, the default empty will be wrong.
 *      .parse_grid(grid1)
 *      .parse_grid(grid2);
 *
 *   std::cout << map << std::endl;
 *
 *   map.add_to_bridge(); // Call this first before using in the game.
 *   @endcode
 */
class Map {
public:
  using SpaceCallback = std::function<SpaceType(const Pos3i&,SpaceType)>;
  using DefaultEmptyCallback = std::function<void(const Pos3i&,SpaceType)>;

  static inline const Range2i kSupportedVersions{1,1};
  static inline const Duration kMinRobotDelay = Duration::from_millis(110);

  static bool is_map_file(const std::filesystem::path& file);

  virtual ~Map() noexcept = default;
  virtual Map& clear_grids();

  Map& load_file(const std::filesystem::path& file,const SpaceCallback& on_space = nullptr
      ,const DefaultEmptyCallback& on_def_empty = nullptr,bool meta_only = false);
  Map& load_file_meta(const std::filesystem::path& file);
  Map& parse_grid(const std::vector<std::string>& lines,const SpaceCallback& on_space = nullptr
      ,const DefaultEmptyCallback& on_def_empty = nullptr);
  Map& parse_grid(const std::vector<std::string>& lines,Size2i size,const SpaceCallback& on_space = nullptr
      ,const DefaultEmptyCallback& on_def_empty = nullptr,std::string file = "");
  Map& shrink_grids_to_fit();
  virtual Map& add_to_bridge() { return *this; }

  bool move_thing(const Pos3i& from_pos,const Pos3i& to_pos);
  bool remove_thing(const Pos3i& pos);
  bool place_thing(SpaceType type,const Pos3i& pos);

  virtual bool move_player(const Pos3i& pos);
  virtual bool sync_player_pos();
  virtual bool change_grid(int z);

  Map& set_title(const std::string& title);
  Map& set_author(const std::string& author);
  Map& set_turning_speed(float speed);
  Map& set_walking_speed(float speed);
  Map& set_default_empty(SpaceType type);
  Map& set_robot_delay(const Duration& duration);

  std::string build_header() const;
  int version() const;
  const std::string& title() const;
  const std::string& author() const;
  float turning_speed() const;
  float walking_speed() const;
  SpaceType default_empty() const;
  const Duration& robot_delay() const;

  int grid_count() const;
  int grid_z() const;
  Size2i size() const;
  Size2i size(int z) const;
  const Space* space(const Pos3i& pos) const;

  int total_cells() const;
  int total_rescues() const;
  const Pos3i& player_init_pos() const;
  Facing player_init_facing() const;

  virtual Pos3i player_pos() const { return Pos3i{}; }
  virtual const Space* player_space() const { return nullptr; }
  virtual SpaceType player_space_type() const { return SpaceType::kNil; }
  virtual Facing player_facing() const { return Facing::kSouth; }

  std::ostream& print(bool rstrip = false) const;
  std::ostream& print(std::ostream& out,bool rstrip = false) const;
  friend std::ostream& operator<<(std::ostream& out,const Map& map);

protected:
  static inline const std::string kHeaderFmt = "[EkoScape/v{}]";
  static inline const std::regex kHeaderRegex{R"(^\s*\[EkoScape/v(\d+)\]\s*$)",std::regex::icase};

  int version_ = kSupportedVersions.max;
  std::string title_{};
  std::string author_{};
  float turning_speed_ = 10.0f;
  float walking_speed_ = 5.0f;
  SpaceType default_empty_ = SpaceType::kEmpty;
  Duration robot_delay_ = Duration::from_millis(900);

  std::vector<std::unique_ptr<MapGrid>> grids_{};
  int grid_index_ = -1;

  int total_cells_ = 0;
  int total_rescues_ = 0;
  Pos3i player_init_pos_{};
  Facing player_init_facing_ = Facing::kSouth;

  static bool parse_header(const std::string& line,int& version,bool warn = true);

  void load_metadata(TextReader& reader,const std::string& file);
  void load_grids(TextReader& reader,const SpaceCallback& on_space,const DefaultEmptyCallback& on_def_empty
      ,const std::string& file);

  virtual void update_bridge_space(const Pos3i& /*pos*/,SpaceType /*type*/) {}

  Space* mutable_space(const Pos3i& pos); // Can't use the name `space`, unfortunately.
  Space& raw_space(const Pos3i& pos);
  const Space& raw_space(const Pos3i& pos) const;
};

} // Namespace.
#endif
