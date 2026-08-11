/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "bzfsAPI.h"

#include <concepts>
#include <cstdint>
#include <format>
#include <memory>
#include <span>
#include <string_view>

using PlayerId = int;
using ShotId = std::uint32_t;

namespace bz
{

template <typename To, typename... Args>
auto send_message(PlayerId from, To to, std::format_string<Args...> fmt, Args&&... args)
   requires std::integral<To> || std::same_as<To, bz_eTeamType>
{
   auto message = std::format(fmt, std::forward<Args>(args)...);
   bz_sendTextMessage(from, to, message.c_str());
}

template <typename... Args>
auto debug(int level, std::format_string<Args...> fmt, Args&&... args)
{
   auto message = std::format(fmt, std::forward<Args>(args)...);
   bz_debugMessage(level, message.c_str());
}

struct Deleter
{
   auto operator()(bz_BasePlayerRecord* ptr) const -> void { bz_freePlayerRecord(ptr); }
   auto operator()(bz_APIIntList* ptr) const -> void { bz_deleteIntList(ptr); }
   auto operator()(bz_APIFloatList* ptr) const -> void { bz_deleteFloatList(ptr); }
   auto operator()(bz_APIStringList* ptr) const -> void { bz_deleteStringList(ptr); }
   auto operator()(bz_MaterialInfo* ptr) const -> void { bz_deleteMaterial(ptr); }

   // Fallback for everything else, except T[]
   auto operator()(auto* ptr) const -> void { delete ptr; }
};

template <typename T>
using UniquePtr = std::unique_ptr<T, Deleter>;

/**
 * @brief Wrapper around @c bz_fireservershot().
 *
 * @param shot_type The flag abbreviation for the type of shot used.
 * @param origin The 3D Cartesian coordinates for the origin of the shot.
 * @param direction A 3D vector representing the direction to fire the shot.
 * @param player The player ID of the player that "fired" it.
 */
auto fire_server_shot(
   std::string_view shot_type,
   std::span<const float, 3> origin,
   std::span<const float, 3> direction,
   PlayerId player) -> ShotId;

auto player_record_from_id(PlayerId index) -> UniquePtr<bz_BasePlayerRecord>;

auto register_custom_slash_command(
   std::string_view command, bz_CustomSlashCommandHandlerV2* handler) -> bool;

auto register_custom_flag(
   std::string_view abbr,
   std::string_view name,
   std::string_view help,
   bz_eShotType shotType,
   bz_eFlagQuality quality) -> bool;

auto remove_custom_slash_command(std::string_view command) -> bool;

} // namespace bz
