/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "utility.hpp"

namespace bz
{

auto fire_server_shot(
   std::string_view shot_type,
   std::span<const float, 3> origin,
   std::span<const float, 3> direction,
   PlayerId player) -> ShotId
{
   // Nasty const_cast to make this const-correct
   return bz_fireServerShot(
      shot_type.data(), const_cast<float*>(origin.data()), const_cast<float*>(direction.data()),
      bz_getPlayerTeam(player), player);
}

auto player_record_from_id(PlayerId player) -> UniquePtr<bz_BasePlayerRecord>
{
   return {bz_getPlayerByIndex(player), {}};
}

auto register_custom_slash_command(
   std::string_view command, bz_CustomSlashCommandHandlerV2* handler) -> bool
{
   return bz_registerCustomSlashCommand(command.data(), handler);
}

auto register_custom_flag(
   std::string_view abbr,
   std::string_view name,
   std::string_view help,
   bz_eShotType shotType,
   bz_eFlagQuality quality) -> bool
{
   return bz_RegisterCustomFlag(abbr.data(), name.data(), help.data(), shotType, quality);
}

auto remove_custom_slash_command(std::string_view command) -> bool
{
   return bz_removeCustomSlashCommand(command.data());
}

} // namespace bz
