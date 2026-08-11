/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#include "MinePlugin.hpp"

#include "bzfsAPI.h"
#include "utility.hpp"

#include <algorithm>

auto MinePlugin::Name() -> const char* { return PLUGIN_NAME.data(); }

void MinePlugin::Init([[maybe_unused]] const char* commandLine)
{
   // Register events
   Register(bz_eFlagGrabbedEvent);
   Register(bz_ePlayerDieEvent);
   Register(bz_ePlayerPartEvent);
   Register(bz_ePlayerSpawnEvent);
   Register(bz_ePlayerUpdateEvent);

   bz::register_custom_slash_command(MINE_COMMAND, this);
   // bz_registerCustomBZDBInt(bzdb_safetyTime, 5);

   // Register custom flags.
   bz::register_custom_flag(MINE_FLAG_ABBR, MINE_FLAG_NAME, MINE_FLAG_DESC, 0, eGoodFlag);
}

auto MinePlugin::Event(bz_EventData* event_data) -> void
{
   switch (event_data->eventType)
   {
   case bz_eFlagGrabbedEvent:
      return handle_flag_grabbed_event(static_cast<bz_FlagGrabbedEventData_V1*>(event_data));
   case bz_ePlayerDieEvent:
      return handle_player_die_event(static_cast<bz_PlayerDieEventData_V1*>(event_data));
   case bz_ePlayerPartEvent:
      return handle_player_part_event(static_cast<bz_PlayerJoinPartEventData_V1*>(event_data));
   case bz_ePlayerSpawnEvent:
      return handle_player_spawned_event(static_cast<bz_PlayerSpawnEventData_V1*>(event_data));
   case bz_ePlayerUpdateEvent:
      return handle_player_update_event(static_cast<bz_PlayerUpdateEventData_V1*>(event_data));
   default:
      return;
   }
}

auto MinePlugin::handle_flag_grabbed_event(bz_FlagGrabbedEventData_V1* event_data) -> void
{
   // If the user grabbed the Mine flag, let them know they can place a mine.
   if (event_data->flagType == MINE_FLAG_ABBR)
   {
      bz::send_message(
         BZ_SERVER, event_data->playerID,
         "You grabbed a Mine flag! Type /mine at any time to set a mine.");
   }
}

auto MinePlugin::handle_player_die_event(bz_PlayerDieEventData_V1* event_data) -> void
{
   auto shot_guid = bz_getShotGUID(event_data->killerID, event_data->shotID);

   // Only handle shots that have this plugin's metadata.
   if (!bz_shotHasMetaData(shot_guid, PlayerMine::PLAYER_ID_META_DATA.data()))
   {
      return;
   }

   // If the shot metadata matches the Mine metadata, then we need to
   // change the killer in the event data. It defaults to the server.
   // Change to the mine owner.
   event_data->killerID =
      static_cast<PlayerId>(bz_getShotMetaDataI(shot_guid, PlayerMine::PLAYER_ID_META_DATA.data()));

   if (event_data->playerID == event_data->killerID)
   {
      bz::send_message(BZ_SERVER, event_data->playerID, "You were obliterated by your own mine!");
      return;
   }

   auto player_record = bz::player_record_from_id(event_data->playerID);
   if (player_record)
   {
      bz::send_message(
         BZ_SERVER, event_data->playerID, "You were obliterated by {}'s mine!",
         player_record->callsign.c_str());
   }
}

auto MinePlugin::handle_player_part_event(bz_PlayerJoinPartEventData_V1* event_data) -> void
{
   remove_mines_for_player(event_data->playerID);
}

auto MinePlugin::handle_player_spawned_event(bz_PlayerSpawnEventData_V1* event_data) -> void
{
   // TODO: spawn delay timeout.
}

auto MinePlugin::handle_player_update_event(bz_PlayerUpdateEventData_V1* event_data) -> void
{
   if (event_data->state.status != eAlive)
   {
      return;
   }

   auto range = static_cast<float>(bz_getBZDBDouble("_shockOutRadius") * 0.9);
   auto explodable_mine = std::ranges::find_if(
      mines_,
      [&](auto const& mine)
      {
         return mine.can_detonate_for(event_data->playerID) &&
                mine.is_within_range(event_data->state.pos, range);
      });

   if (explodable_mine != std::ranges::end(mines_))
   {
      explodable_mine->detonate();
      mines_.erase(explodable_mine);
   }
}

auto MinePlugin::remove_mines_for_player(PlayerId player_id) -> void
{
   std::erase_if(
      mines_, [player_id](PlayerMine const& mine) { return mine.player_id() == player_id; });
}

auto MinePlugin::add_player_mine(PlayerId player_id, std::span<const float, 3> pos) -> void
{
   mines_.emplace_back(player_id, pos);
   bz_removePlayerFlag(player_id);
   bz::send_message(BZ_SERVER, player_id, "The mine has been set!");
}

auto MinePlugin::SlashCommand(
   PlayerId player_id,
   [[maybe_unused]] int source_channel,
   bz_ApiString command,
   [[maybe_unused]] bz_ApiString message,
   [[maybe_unused]] bz_APIStringList* params) -> bool
{
   if (command.c_str() != MINE_COMMAND)
   {
      return false;
   }

   auto player_record = bz::player_record_from_id(player_id);
   if (player_record == nullptr)
   {
      return true;
   }

   if (player_record->team == eObservers)
   {
      bz::send_message(BZ_SERVER, player_id, "Observers can't place mines!");
      return true;
   }

   if (player_record->currentFlag.c_str() != MINE_FLAG_LONG_NAME)
   {
      bz::send_message(
         BZ_SERVER, player_record->playerID, "You must have the Mine (MN) flag to place a mine!");
      return true;
   }

   add_player_mine(player_id, player_record->lastKnownState.pos);
   return true;
}

auto MinePlugin::Cleanup() -> void
{
   Flush();

   bz::remove_custom_slash_command(MINE_COMMAND);

   // bz_removeCustomBZDBVariable(bzdb_safetyTime);
}
