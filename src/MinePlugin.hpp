/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "PlayerMine.hpp"
#include "utility.hpp"

#include "bzfsAPI.h"

#include <flat_map>
#include <span>
#include <string_view>
#include <vector>

struct PlayerInfo
{
   std::string name;
   std::array<float, 3> last_pos;
};

/**
 * @brief Plug-in class.
 */
class MinePlugin final : public bz_Plugin, public bz_CustomSlashCommandHandlerV2
{
   // Name of the plugin
   inline static constexpr auto PLUGIN_NAME = std::string_view{"Mine Plugin"};

   // Mine flag name/code.
   inline static constexpr auto MINE_FLAG_ABBR = std::string_view{"MN"};
   inline static constexpr auto MINE_FLAG_NAME = std::string_view{"Mine"};
   inline static constexpr auto MINE_FLAG_DESC = std::string_view{"Place mines to blow tanks up"};

   // Weird that a long format is stored in some places.
   // Different strings for the flag type are used in different places - hard to keep track of.
   inline static constexpr auto MINE_FLAG_LONG_NAME = std::string_view{"MiNe (+MN)"};

   // Name of the command.
   inline static constexpr auto MINE_COMMAND = std::string_view{"mine"};

 public:
   /**
    * @brief Gets the name of the plugin.
    * @return The plug-in name.
    */
   auto Name() -> const char* override;

   /**
    * @brief Performs any initialization needed for the plug-in.
    * @param command_line Command-line options when passed when loading the plug-in.
    */
   auto Init(const char* command_line) -> void override;

   /**
    * @brief Main event handler callback entry point.
    * @return event_data Pointer to the respective event data for the event.
    */
   auto Event(bz_EventData* event_data) -> void override;

   /**
    * @brief Cleanup handler.
    */
   auto Cleanup() -> void override;

   /**
    * @brief Handler for the custom slash command.
    *
    * @param player_id The ID for the player.
    * @param source_channel Channel the command was sent on.
    * @param command The slash command that was called.
    * @param message A string containing the rest of the command arguments.
    * @param param The tokenized message.
    *
    * @return Returns true if the command was valid, false otherwise.
    */
   virtual auto SlashCommand(
      PlayerId player_id,
      int source_channel,
      bz_ApiString command,
      bz_ApiString message,
      bz_APIStringList* params) -> bool override;

 private:
   /* --------------------------------------------------------------------- */
   /* Plugin API Event Handlers                                             */
   /* --------------------------------------------------------------------- */

   /**
    * @brief Callback that handles the bz_eFlagGrabbedEvent event.
    * @param flagGrabbedData Pointer to the event data @c bz_FlagGrabbedEventData_V1.
    */
   auto handle_flag_grabbed_event(bz_FlagGrabbedEventData_V1* event_data) -> void;

   /**
    * @brief Callback that handles the bz_ePlayerDieEvent event.
    * @param dieData Pointer to the event data @c bz_PlayerDieEventData_V1.
    */
   auto handle_player_die_event(bz_PlayerDieEventData_V1* event_data) -> void;

   /**
    * @brief Callback that handles the @c bz_ePlayerPartEvent event.
    * @param joinPartData Pointer to the event data @c bz_PlayerJoinPartEventData_V1.
    */
   auto handle_player_part_event(bz_PlayerJoinPartEventData_V1* event_data) -> void;

   /**
    * @brief Callback that handles the
    * @param spawnData Pointer to the event data @c bz_PlayerSpawnEventData_V1.
    */
   auto handle_player_spawned_event(bz_PlayerSpawnEventData_V1* event_data) -> void;

   /**
    * @brief Callback that handles the
    * @param updateData Pointer to the event data @c bz_PlayerUpdateEventData_V1.
    */
   auto handle_player_update_event(bz_PlayerUpdateEventData_V1* event_data) -> void;

   /* --------------------------------------------------------------------- */
   /* Mine Management                                                       */
   /* --------------------------------------------------------------------- */

   /**
    * @brief Removes all mines for a given player.
    * @param player_id The ID for the player.
    */
   void remove_mines_for_player(PlayerId player_id);

   /**
    * @brief Adds a mine.
    * @param player_id The ID for the player.
    * @param pos The location of the mine.
    */
   void add_player_mine(PlayerId player_id, std::span<const float, 3> pos);

   // Using a vector because
   // - insertion/removal is rare
   // - 99% usage is looping through the container
   // - very few elements (normal usage, under 5 elements)
   // - vector has better cache-locality than all other containers
   std::vector<PlayerMine> mines_;
   // Not used yet.
   std::flat_map<PlayerId, PlayerInfo> player_info_;
};
