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
#include <ranges>

// https://www.tauday.com/tau-manifesto
template <typename T>
inline constexpr auto tao_v = std::numbers::pi_v<T> * 2;

inline constexpr auto tao = tao_v<double>;

auto PlayerMine::is_within_range(std::span<const float, 3> pos) const -> bool
{
   auto dx = position_[0] - pos[0];
   auto dy = position_[1] - pos[1];
   auto dz = position_[2] - pos[2];

   auto norm_squared = [](auto... args) { return ((args * args) + ...); };
   return (norm_squared(dx, dy, dz) < range_squared_);
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
   static constexpr auto num_angles = 8;

   fire_shot("SW", 0.0f, 0.0f, 0.0f);
   fire_shot("F", 0.0f, 0.0f, 1.0f);
   fire_shot("F", 0.0f, 0.0f, -1.0f);

   for (const auto zcoord : elevations)
   {
      for (const auto i : std::views::iota(0, num_angles))
      {
         auto angle = rotation_ + static_cast<float>(i) * tao_v<float> / num_angles;
         fire_shot("F", std::cos(angle), std::sin(angle), zcoord);
      }
   }
}
