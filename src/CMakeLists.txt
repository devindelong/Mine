# ==============================================================================
#
# Copyright (c) 2026 Devin DeLong
# SPDX-License-Identifier: BSD-3-clause
#
# Licensed under the BSD 3-Clause License.
# See the license file in the project root for full license information.
#
# ==============================================================================

file(GLOB MINE_PLUGIN_SOURCES CONFIGURE_DEPENDS *.cpp)

add_bzflag_plugin (
  ${MINE_PLUGIN}
  SOURCES
    ${MINE_PLUGIN_SOURCES}
  INCLUDE_PATHS
    ${MINE_PLUGIN_SOURCE_PATH}
)

target_compile_features (
  ${MINE_PLUGIN}
  PRIVATE
    ${MINE_PLUGIN_CXX_VERSION}
)
