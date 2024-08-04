/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EKOSCAPE_MAP_MAP_H_
#define EKOSCAPE_MAP_MAP_H_

#include "core/common.h"

#include "core/duration.h"
#include "core/ekoscape_error.h"
#include "core/text_reader.h"
#include "core/util.h"

#include "facing.h"
#include "space.h"
#include "space_type.h"

#include <regex>
#include <vector>

namespace ekoscape {

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
  static const int kMinSupportedVersion = 1;
  static const int kMaxSupportedVersion = 1;
  static const Duration kMinRobotDelay;

  static bool is_map_file(const std::string& file);

  Map& load_file(const std::string& file);
  Map& parse_grid(const std::vector<std::string>& lines,int width = 0,int height = 0);
  virtual Map& clear_spaces();

  virtual bool move_thing(int from_x,int from_y,int to_x,int to_y);
  virtual bool remove_thing(int x,int y);
  virtual bool place_thing(SpaceType type,int x,int y);
  virtual bool unlock_cell(int x,int y);

  Map& set_title(const std::string& title);
  Map& set_author(const std::string& author);

  Map& set_turning_speed(float speed);
  Map& set_walking_speed(float speed);

  Map& set_default_empty(SpaceType type);
  Map& set_robot_delay(Duration duration);

  virtual bool set_space(int x,int y,SpaceType empty_type,SpaceType thing_type);
  virtual bool set_empty(int x,int y,SpaceType type);
  virtual bool set_thing(int x,int y,SpaceType type);

  std::string build_header() const;
  int version() const;
  const std::string& title() const;
  const std::string& author() const;

  float turning_speed() const;
  float walking_speed() const;

  SpaceType default_empty() const;
  const Duration& robot_delay() const;

  int width() const;
  int height() const;
  const Space* space(int x,int y) const;

  int total_cells() const;
  int total_rescues() const;

  int player_init_x() const;
  int player_init_y() const;
  Facing player_init_facing() const;

  friend std::ostream& operator<<(std::ostream& out,const Map& map);

protected:
  int version_ = kMaxSupportedVersion;
  std::string title_{};
  std::string author_{};

  float turning_speed_ = 10.0f;
  float walking_speed_ = 5.0f;

  SpaceType default_empty_ = SpaceType::kEmpty;
  Duration robot_delay_ = Duration::from_millis(900);

  int width_ = 0;
  int height_ = 0;
  std::vector<Space> spaces_{};

  int total_cells_ = 0;
  int total_rescues_ = 0;

  int player_init_x_ = 0;
  int player_init_y_ = 0;
  Facing player_init_facing_ = Facing::kSouth;

  static bool parse_header(const std::string& line,int& version,bool warn = true);

  void set_raw_space(int x,int y,Space&& space);

  Space* mutable_space(int x,int y);
  Space& raw_space(int x,int y);
  const Space& raw_space(int x,int y) const;
};

} // Namespace.
#endif
