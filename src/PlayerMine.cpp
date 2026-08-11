/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "PlayerMine.hpp"
#include "utility.hpp"

#include "bzfsAPI.h"

#include <cmath>
#include <numbers>

auto PlayerMine::is_within_range(std::span<const float, 3> pos, float range) const -> bool
{
   auto is_in_range = [range](float mine_pos, float player_pos)
   { return std::abs(mine_pos - player_pos) < range; };

   return (
      is_in_range(position_[0], pos[0]) && is_in_range(position_[1], pos[1]) &&
      is_in_range(position_[2], pos[2]));
}

auto PlayerMine::fire_shot(std::string_view shot_type, float dx, float dy, float dz) const -> void
{
   const auto direction = std::array{dx, dy, dz};
   auto shot_guid = bz::fire_server_shot(shot_type, position_, direction, player_id_);
   bz_setShotMetaData(
      shot_guid, PLAYER_ID_META_DATA.data(), static_cast<std::uint32_t>(player_id_));
}

auto PlayerMine::can_detonate_for(PlayerId other_player_id) const -> bool
{
   auto other_player_team = bz_getPlayerTeam(other_player_id);

   auto valid_player = other_player_team != eObservers;
   auto same_player = player_id() == other_player_id;
   auto same_team = (player_team() == other_player_team) and (other_player_team != eRogueTeam);
   auto open_ffa_game = bz_getGameType() == eOpenFFAGame;

   return valid_player && !same_player && (open_ffa_game || !same_team);
}

auto PlayerMine::detonate() -> void
{
   static constexpr auto sqrt2_over_2 = std::numbers::sqrt2_v<float> / 2.0f;
   static constexpr auto elevations = std::array{-sqrt2_over_2, 0.0f, sqrt2_over_2};

   // Fire shock wave sphere.
   fire_shot("SW", 0.0f, 0.0f, 0.0f);

   // Up
   fire_shot("F", 0.0f, 0.0f, 1.0f);

   // Down
   fire_shot("F", 0.0f, 0.0f, -1.0f);

   for (const auto zcoord : elevations)
   {
      // xy-plane, every pi/4
      fire_shot("F", 1.0f, 0.0f, zcoord);
      fire_shot("F", 0.0f, 1.0f, zcoord);
      fire_shot("F", -1.0f, 0.0f, zcoord);
      fire_shot("F", 0.0f, -1.0f, zcoord);
      fire_shot("F", sqrt2_over_2, sqrt2_over_2, zcoord);
      fire_shot("F", -sqrt2_over_2, sqrt2_over_2, zcoord);
      fire_shot("F", -sqrt2_over_2, -sqrt2_over_2, zcoord);
      fire_shot("F", sqrt2_over_2, -sqrt2_over_2, zcoord);
   }
}
