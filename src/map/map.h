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

#include <functional>
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
 *       .set_empty_cell(SpaceType::kWhiteFloor)
 *       .set_empty_player(SpaceType::kWhiteFloor)
 *       .set_empty_robot(SpaceType::kEmpty)
 *       .set_robot_delay(Duration::from_millis(2000))
 *       // Do this last! Else, the empty*s will be wrong.
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
  Map& clear_spaces();

  Map& add_to(Dantares& dantares,const std::function<void(int,int,Space&,SpaceType)>& handle_space = nullptr);
  Map& make_current_in(Dantares& dantares);
  Map& generate_in(Dantares& dantares);

  bool move_thing(int from_x,int from_y,int to_x,int to_y,Dantares& dantares);
  bool remove_thing(int x,int y,Dantares& dantares);
  bool place_thing(SpaceType type,int x,int y,Dantares& dantares);
  bool unlock_cell(int x,int y,Dantares& dantares);

  Map& set_title(const std::string& title);
  Map& set_author(const std::string& author);

  Map& set_turning_speed(float speed);
  Map& set_walking_speed(float speed);

  Map& set_empty_cell(SpaceType type);
  Map& set_empty_player(SpaceType type);
  Map& set_empty_robot(SpaceType type);
  Map& set_robot_delay(Duration duration);

  std::string build_header() const;
  int version() const;
  const std::string& title() const;
  const std::string& author() const;

  float turning_speed() const;
  float walking_speed() const;

  SpaceType empty_cell() const;
  SpaceType empty_player() const;
  SpaceType empty_robot() const;
  const Duration& robot_delay() const;

  int width() const;
  int height() const;
  Space* space(int x,int y);
  const Space* space(int x,int y) const;

  int total_cells() const;
  int total_rescues() const;

  int player_init_x() const;
  int player_init_y() const;
  Facing player_init_facing() const;
  int id() const;

  friend std::ostream& operator<<(std::ostream& out,const Map& map);

private:
  int version_ = kMaxSupportedVersion;
  std::string title_{};
  std::string author_{};

  float turning_speed_ = 10.0f;
  float walking_speed_ = 5.0f;

  SpaceType empty_cell_ = SpaceType::kEmpty;
  SpaceType empty_player_ = SpaceType::kEmpty;
  SpaceType empty_robot_ = SpaceType::kEmpty;
  Duration robot_delay_ = Duration::from_millis(900);

  int width_ = 0;
  int height_ = 0;
  std::vector<Space> spaces_{};

  int total_cells_ = 0;
  int total_rescues_ = 0;

  int player_init_x_ = 0;
  int player_init_y_ = 0;
  Facing player_init_facing_ = Facing::kSouth;
  int id_ = -1;

  static bool parse_header(const std::string& line,int& version,bool warn = true);

  void update_square(int x,int y,SpaceType type,Dantares& dantares);

  void set_space_imp(int x,int y,Space&& space);

  Space& space_imp(int x,int y);
  const Space& space_imp(int x,int y) const;
};

} // Namespace.
#endif
