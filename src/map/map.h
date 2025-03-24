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
 *
 * Note that any `raw` function is meant to be used before calling add_to_bridge()
 * and before using the Map in the game:
 * - set_raw_space()
 * - set_raw_empty()
 * - set_raw_thing()
 * - remove_raw_thing()
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

  Map& load_file(const std::filesystem::path& file,const SpaceCallback& on_space = nullptr,
                 const DefaultEmptyCallback& on_def_empty = nullptr,bool meta_only = false);
  Map& load_file_meta(const std::filesystem::path& file);
  Map& parse_grid(const std::vector<std::string>& lines,const SpaceCallback& on_space = nullptr,
                  const DefaultEmptyCallback& on_def_empty = nullptr);
  Map& parse_grid(const std::vector<std::string>& lines,Size2i size,const SpaceCallback& on_space = nullptr,
                  const DefaultEmptyCallback& on_def_empty = nullptr,std::string_view file = "");
  Map& shrink_grids_to_fit();

  virtual void add_to_bridge();
  virtual void on_context_restored();

  bool move_thing(const Pos3i& from_pos,const Pos3i& to_pos);
  bool remove_thing(const Pos3i& pos);
  bool place_thing(SpaceType thing,const Pos3i& pos);

  virtual bool move_player(const Pos3i& pos);
  virtual bool sync_player_pos();
  virtual bool change_grid(int z);

  Map& set_title(std::string_view title);
  Map& set_author(std::string_view author);

  /**
   * In degrees per frame from 0 to 90 degrees:
   * - 1 degree turns the Player 1 degree per frame (very slow).
   * - 90 degrees turns the Player instantaneously in one frame (very fast).
   *
   * 0 uses Dantares' default value.
   */
  Map& set_turning_speed(float speed);

  /**
   * The number of frames it takes to complete walking one step,
   * based on the formula `<seconds> = <speed> / FPS`.
   *
   * At 60 FPS:
   * - 1 frame takes the Player ~0.017 seconds (1 / 60 FPS) to walk a step (instantaneous, very fast).
   * - 60 frames takes the Player 1 second (60 / 60 FPS) to walk a step (very slow).
   * - 120 frames takes the Player 2 seconds (120 / 60 FPS) to walk a step (super slow).
   *
   * 0 uses Dantares' default value.
   */
  Map& set_walking_speed(float speed);

  Map& set_default_empty(SpaceType empty);
  Map& set_robot_delay(const Duration& duration);

  bool set_raw_space(const Pos3i& pos,SpaceType empty,SpaceType thing);
  bool set_raw_empty(const Pos3i& pos,SpaceType empty);
  bool set_raw_thing(const Pos3i& pos,SpaceType thing);
  bool remove_raw_thing(const Pos3i& pos);

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

  virtual Pos3i player_pos() const;
  virtual const Space* player_space() const;
  virtual SpaceType player_space_type() const;
  virtual Facing player_facing() const;

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
  Facing player_init_facing_ = Facings::kFallback;

  static bool parse_header(const std::string& line,int& version,bool warn = true);

  void load_metadata(TextReader& reader,std::string_view file);
  void load_grids(TextReader& reader,const SpaceCallback& on_space,const DefaultEmptyCallback& on_def_empty,
                  std::string_view file);

  void on_raw_thing_updated(SpaceType old_thing,SpaceType new_thing);
  virtual void update_bridge_space(const Pos3i& pos,SpaceType type);

  Space* mutable_space(const Pos3i& pos); // Can't use the name `space`, unfortunately.
  Space& unsafe_space(const Pos3i& pos);
  const Space& unsafe_space(const Pos3i& pos) const;
};

} // namespace ekoscape
#endif
