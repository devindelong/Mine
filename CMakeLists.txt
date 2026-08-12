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

add_subdirectory (${MINE_PLUGIN_SOURCE_PATH})
