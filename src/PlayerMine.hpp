/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "utility.hpp"

#include "bzfsAPI.h"

#include <array>
#include <span>
#include <string_view>

class PlayerMine
{
 public:
   /**
    * @brief Metadata to identify shots generated from the mine.
    */
   inline static constexpr auto PLAYER_ID_META_DATA = std::string_view{"PlayerMine_Id"};

   /**
    * @brief Constructs a @c PlayerMine object.
    *
    * @param player The ID of the player that owns the mine.
    * @param state The player update state.
    */
   PlayerMine(PlayerId player, bz_PlayerUpdateState const& state);

   /**
    * @brief Gets the ID of the player that owns the mine.
    *
    * @return The player's ID.
    */
   [[nodiscard]] auto player_id() const noexcept -> PlayerId { return player_id_; }

   /**
    * @brief Gets the position of the mine.
    *
    * @return The position in 3D Cartesian coordinates.
    */
   [[nodiscard]] auto position() const noexcept -> std::array<float, 3> const& { return position_; }

   /**
    * @brief Gets the team of the player that owns the mine.
    *
    * @return The mine owner's team.
    */
   [[nodiscard]] auto player_team() const -> bz_eTeamType { return bz_getPlayerTeam(player_id_); }

   /**
    * @brief Checks if a player's position is within detonation range.
    *
    * @param position The position of another player.
    * @param range The detection distance for triggering the mine.
    */
   [[nodiscard]] auto is_within_range(std::span<const float, 3> position) const -> bool;

   /**
    * @brief Checks if a mine can be triggered by a player based on team gameplay rules.
    */
   [[nodiscard]] auto can_detonate_for(PlayerId other_player_id) const -> bool;

   /**
    * @brief Detonates the mine.
    */
   auto detonate() -> void;

 private:
   /**
    * @brief Fire the shot and handle shot metadata.
    *
    * @param shot_type The flag abbreviation for the type of shot used.
    * @param dx The x-direction to fire the shot.
    * @param dy The y-direction to fire the shot.
    * @param dz The z-direction to fire the shot.
    */
   auto fire_shot(std::string_view shot_type, float dx, float dy, float dz) const -> void;

   PlayerId player_id_;
   std::array<float, 3> position_;
   float rotation_;
   float range_squared_;
};
