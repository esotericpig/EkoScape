/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_MAP_H_
#define EKOSCAPE_MAP_MAP_H_

#include "cybel/common.h"

#include "cybel/io/text_reader.h"
#include "cybel/util/cybel_error.h"
#include "cybel/util/duration.h"
#include "cybel/util/util.h"
#include "cybel/types.h"

#include "facing.h"
#include "space.h"
#include "space_type.h"

#include <filesystem>
#include <functional>
#include <regex>
#include <vector>

namespace ekoscape {

using namespace cybel;

/**
 * Dantares expects a map where the origin (0,0) is from the bottom left, instead of the top left.
 * Because of this, the internal Spaces, Xs, and Ys, are stored like this as well.
 * When parsing a Map Grid (and on output), the lines are flipped vertically to accommodate this.
 *
 * Besides loading & parsing a Map file, this class can also be used for creating a Map file:
 *   std::vector<std::string> lines = {
 *      "xxxxxxxxxxxx",
 *      "x>         x",
 *      "x#####@### x",
 *      "x          x",
 *      "x ####@####x",
 *      "x     !   $x",
 *      "xxxxxxxxxxxx"
 *    };
 *
 *    Map map{};
 *    map.set_title("EZ")
 *       .set_author("Bradz")
 *       .set_turning_speed(5.0f)
 *       .set_walking_speed(15.0f)
 *       .set_default_empty(SpaceType::kWhiteFloor)
 *       .set_robot_delay(Duration::from_millis(2000))
 *       // Do this last! Else, the default empty will be wrong.
 *       .parse_grid(lines);
 *
 *    std::cout << map << std::endl;
 */
class Map {
public:
  using SpaceCallback = std::function<SpaceType(const Pos2i&,SpaceType)>;

  static const Range2i kSupportedVersions;
  static const Duration kMinRobotDelay;

  static bool is_map_file(const std::filesystem::path& file);

  virtual ~Map() noexcept = default;

  Map& load_file(const std::filesystem::path& file,const SpaceCallback& on_space = nullptr
      ,bool meta_only = false);
  Map& load_file_meta(const std::filesystem::path& file);
  Map& parse_grid(const std::vector<std::string>& lines,Size2i size = {0,0}
      ,const SpaceCallback& on_space = nullptr);
  virtual Map& clear_spaces();

  virtual bool move_thing(const Pos2i& from_pos,const Pos2i& to_pos);
  virtual bool remove_thing(const Pos2i& pos);
  virtual bool place_thing(SpaceType type,const Pos2i& pos);
  virtual bool unlock_cell(const Pos2i& pos);

  Map& set_title(const std::string& title);
  Map& set_author(const std::string& author);

  Map& set_turning_speed(float speed);
  Map& set_walking_speed(float speed);

  Map& set_default_empty(SpaceType type);
  Map& set_robot_delay(Duration duration);

  virtual bool set_space(const Pos2i& pos,SpaceType empty_type,SpaceType thing_type);
  virtual bool set_empty(const Pos2i& pos,SpaceType type);
  virtual bool set_thing(const Pos2i& pos,SpaceType type);

  std::string build_header() const;
  int version() const;
  const std::string& title() const;
  const std::string& author() const;

  float turning_speed() const;
  float walking_speed() const;

  SpaceType default_empty() const;
  const Duration& robot_delay() const;

  const Size2i& size() const;
  const Space* space(const Pos2i& pos) const;

  int total_cells() const;
  int total_rescues() const;

  const Pos2i& player_init_pos() const;
  Facing player_init_facing() const;

  friend std::ostream& operator<<(std::ostream& out,const Map& map);

protected:
  static const std::string kHeaderFmt;
  static const std::regex kHeaderRegex;

  int version_ = kSupportedVersions.max;
  std::string title_{};
  std::string author_{};

  float turning_speed_ = 10.0f;
  float walking_speed_ = 5.0f;

  SpaceType default_empty_ = SpaceType::kEmpty;
  Duration robot_delay_ = Duration::from_millis(900);

  Size2i size_{};
  std::vector<Space> spaces_{};

  int total_cells_ = 0;
  int total_rescues_ = 0;

  Pos2i player_init_pos_{};
  Facing player_init_facing_ = Facing::kSouth;

  static bool parse_header(const std::string& line,int& version,bool warn = true);

  void set_raw_space(const Pos2i& pos,Space&& space);

  Space* mutable_space(const Pos2i& pos);
  Space& raw_space(const Pos2i& pos);
  const Space& raw_space(const Pos2i& pos) const;
};

} // Namespace.
#endif
