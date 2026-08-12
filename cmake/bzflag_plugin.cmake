# ==============================================================================
#
# SPDX-License-Identifier: BSD-3-clause
# SPDX-FileCopyrightText: 2026 Devin DeLong
#
# ==============================================================================

# ------------------------------------------------------------------------------
# Configure BZFlag root source location
# ------------------------------------------------------------------------------

set (BZFLAG_PROJECT_ROOT "" CACHE STRING "The root bzflag project directory")

if (NOT BZFLAG_PROJECT_ROOT)
   message (FATAL_ERROR "BZFLAG_PROJECT_ROOT must be set, e.g. -DBZFLAG_PROJECT_ROOT=/path/to/bzflag")
endif()

# Where bzfsApi.h lives (along with many other things)
set (BZFLAG_INCLUDE_PATH "${BZFLAG_PROJECT_ROOT}/include")

# ------------------------------------------------------------------------------
# Function add_plugin().
# ------------------------------------------------------------------------------

# Encourage better code with more warnings.
set (BZFLAG_PLUGIN_WARNING_FLAGS
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

function (add_bzflag_plugin TARGET)
   set(options "")
   set(oneValueArgs "")
   set(multiValueArgs SOURCES INCLUDE_PATHS LINK_LIBRARIES)
   cmake_parse_arguments(ARGS
         "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

   if (NOT ARGS_SOURCES)
      message (FATAL_ERROR "add_bzflag_plugin(${TARGET}): SOURCES is required")
   endif()

   add_library (${TARGET} MODULE ${ARGS_SOURCES})
   target_compile_options (${TARGET} PRIVATE ${BZFLAG_PLUGIN_WARNING_FLAGS})
   target_include_directories (${TARGET} PRIVATE ${BZFLAG_INCLUDE_PATH} )

   # Strips the "lib" prefix (libplugin_name.so -> plugin_name.so)
   set_target_properties(${TARGET} PROPERTIES  PREFIX "")

   if (ARGS_INCLUDE_PATHS)
      target_include_directories (${TARGET} PRIVATE ${ARGS_INCLUDE_PATHS})
   endif()

   if (ARGS_LINK_LIBRARIES)
      target_link_libraries (${TARGET} PRIVATE ${ARGS_LINK_LIBRARIES})
   endif()

   # Linker doesn't like undefined symbols. Only on MacOS?
   if (APPLE)
      if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
         target_link_options (${TARGET} PRIVATE -undefined dynamic_lookup)
      elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
         target_link_options (${TARGET} PRIVATE -Wl,-undefined,dynamic_lookup)
      endif()
   endif()
endfunction()
