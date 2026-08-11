# ==============================================================================
#
# Copyright (c) 2026 Devin DeLong
# SPDX-License-Identifier: BSD-3-clause
#
# Licensed under the BSD 3-Clause License.
# See the license file in the project root for full license information.
#
# ==============================================================================

cmake_minimum_required (VERSION 3.27)

project (mine
  LANGUAGES CXX
)

# Set aliases for the project directories.
set (MINE_PLUGIN_LIBRARY_NAME "${PROJECT_NAME}")
set (MINE_PLUGIN_ROOT_PATH    "${PROJECT_SOURCE_DIR}")
set (MINE_PLUGIN_SOURCE_PATH  "${MINE_PLUGIN_ROOT_PATH}/src")
set (MINE_PLUGIN_TEST_PATH    "${MINE_PLUGIN_ROOT_PATH}/tests")

include ("${MINE_PLUGIN_ROOT_PATH}/cmake/bzflag_plugin.cmake")

# C++ version.
set (MINE_PLUGIN_CXX_VERSION cxx_std_23)

# Plugin name
set (MINE_PLUGIN ${MINE_PLUGIN_LIBRARY_NAME})

set (MINE_PLUGIN_WARNING_FLAGS
    -Wall # enables most common warnings (misleadingly not "all")
    -Wextra # the next tier that -Wall misses
    -Wpedantic # strict ISO conformance, catches non-standard extensions
    -Wconversion # implicit narrowing conversions, catches int → float etc.
    -Wshadow # local variable shadows outer scope variable
    -Wnon-virtual-dtor # class with virtual functions but non-virtual destructor
    -Wold-style-cast # C-style casts, encourages static_cast etc.
    -Wcast-align # casting pointer to a type with stricter alignment
    -Woverloaded-virtual # derived class hides base class virtual method
    -Wnull-dereference # detectable null dereference paths
)

set (MINE_PLUGIN_LINK_FLAGS -undefined dynamic_lookup)

add_subdirectory (${MINE_PLUGIN_SOURCE_PATH})
