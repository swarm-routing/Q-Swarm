# Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
# SPDX-License-Identifier: MIT OR GPL-2.0-only
#
# Q-Swarm's simulator-free core on its own: a static library and its tests,
# with no simulator tree, on swarm-core's dual-licensed primitives. Today it
# is the skeleton (#35): the package, its tests and its CI, no protocol code
# (AGENTS.md golden rule 10: the fidelity sheet is reviewed first).
#
#   cmake -S core -B build/core && cmake --build build/core && ctest --test-dir build/core
#
# swarm-core comes from its vcpkg port (swarm-routing/.github#489): with the
# vcpkg toolchain, vcpkg installs the version ../vcpkg.json pins from
# swarm-routing/vcpkg-registry at ../vcpkg-configuration.json's baseline
# (manifest mode), and find_package(swarm-core CONFIG REQUIRED) finds it:
#
#   cmake -S core -B build/core -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
#
# Any installed swarm-core package works too (CMAKE_PREFIX_PATH).
# SWARM_CORE_DIR: a local swarm-core core/ directory instead (development
# against an unreleased swarm-core).
# QSWARM_CORE_SANITIZE=ON adds ASan + UBSan, as the sanitized CI step.
# QSWARM_CORE_TESTS builds the tests (ON at the top level).
# QSWARM_CORE_SWARM_CORE_FROM_PACKAGE=ON takes swarm-core from find_package
# even when SWARM_CORE_DIR is set; QSWARM_CORE_INSTALL=ON (which needs the
# package) adds the install rules and the CMake package q-swarm-core:
# find_package(q-swarm-core CONFIG REQUIRED) gives the imported target
# qswarm::core (its vcpkg port, swarm-routing/.github#436).
# QSWARM_CORE_VERSION is the package version (default: .cz.toml's, else 0.0.0).
cmake_minimum_required(VERSION 3.20)
# The manifest is at the repository root, not in core/: tell the vcpkg
# toolchain (read by project() below) where it is, unless the caller did.
if(NOT DEFINED VCPKG_MANIFEST_DIR AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/../vcpkg.json"
   AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
endif()
# The package version: .cz.toml's (Commitizen bumps it at each release), or
# QSWARM_CORE_VERSION when core/ is built without it (a release tarball, a port).
set(QSWARM_CORE_VERSION "" CACHE STRING "q-swarm-core's package version; empty: .cz.toml's")
if(QSWARM_CORE_VERSION)
  set(_qswarm_core_version "${QSWARM_CORE_VERSION}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../.cz.toml")
  file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../.cz.toml" _qswarm_core_version REGEX "^version = " LIMIT_COUNT 1)
  # a release bump of .cz.toml re-runs the configure step (the package version)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/../.cz.toml")
  string(REGEX REPLACE "^version = \"([0-9.]+)\".*" "\\1" _qswarm_core_version "${_qswarm_core_version}")
else()
  set(_qswarm_core_version 0.0.0)
endif()
project(q_swarm_core VERSION ${_qswarm_core_version} LANGUAGES CXX)

# The C++ standard of the ns-3 release the org supports, the one swarm-core's
# core/ targets too (swarm-routing/ns3-swarm-core#141).
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SWARM_CORE_DIR "" CACHE PATH "swarm-core's core/ directory; empty: find_package(swarm-core CONFIG), its vcpkg port")
option(QSWARM_CORE_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(_qswarm_core_top ON)
else()
  set(_qswarm_core_top OFF)
endif()
option(QSWARM_CORE_TESTS "Build the tests and register them with ctest" ${_qswarm_core_top})
option(QSWARM_CORE_SWARM_CORE_FROM_PACKAGE "Take swarm-core from find_package(swarm-core CONFIG) even when SWARM_CORE_DIR is set" OFF)
option(QSWARM_CORE_INSTALL "Add the install rules and the CMake package q-swarm-core" OFF)
if(SWARM_CORE_DIR AND NOT QSWARM_CORE_SWARM_CORE_FROM_PACKAGE)
  set(_qswarm_core_swarm_core_from_dir ON)
else()
  set(_qswarm_core_swarm_core_from_dir OFF)
endif()
if(QSWARM_CORE_INSTALL AND _qswarm_core_swarm_core_from_dir)
  message(FATAL_ERROR "QSWARM_CORE_INSTALL needs swarm-core from its package, not SWARM_CORE_DIR: the package links an installed swarm-core")
endif()

# Sanitizer flags first: they are directory options, so swarm-core's sources,
# when SWARM_CORE_DIR adds them below, are instrumented too (the vcpkg port is
# a prebuilt library, linked as published).
if(QSWARM_CORE_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# swarm-core: its installed CMake package (the vcpkg port, or any install on
# CMAKE_PREFIX_PATH), which defines the imported target swarm::core; or, for
# development, a checkout's core/ as a subproject, which defines the same target.
if(_qswarm_core_swarm_core_from_dir)
  if(NOT EXISTS "${SWARM_CORE_DIR}/swarm-ports.h")
    message(FATAL_ERROR "SWARM_CORE_DIR=${SWARM_CORE_DIR} is not swarm-core's core/ directory")
  endif()
  add_subdirectory(${SWARM_CORE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/swarm-core)
else()
  find_package(swarm-core CONFIG REQUIRED)
endif()

# The strict warnings the org CI applies (sim-containers' swarm-quality.cmake),
# on Q-Swarm's own sources only. A swarm-core deprecation stays a warning.
set(QSWARM_WARNINGS -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast
                    -Wcast-align -Woverloaded-virtual -Wnull-dereference -Wformat=2 -Werror
                    -Wno-error=deprecated-declarations)

add_library(q_swarm_core STATIC
  q-swarm-version.cc
)
add_library(qswarm::core ALIAS q_swarm_core)
# The installed package's imported target is qswarm::core too (EXPORT_NAME and
# the export's NAMESPACE).
set_target_properties(q_swarm_core PROPERTIES EXPORT_NAME core)
target_compile_definitions(q_swarm_core PRIVATE QSWARM_CORE_VERSION_STRING="${PROJECT_VERSION}")
include(GNUInstallDirs)
target_include_directories(q_swarm_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/q-swarm-core>)
if(QSWARM_CORE_SANITIZE)
  # The objects are instrumented, so whatever links them needs the sanitizer
  # runtime: add_link_options above reaches this directory's targets only.
  target_link_options(q_swarm_core INTERFACE -fsanitize=address,undefined)
endif()
target_link_libraries(q_swarm_core PUBLIC swarm::core)
target_compile_options(q_swarm_core PRIVATE ${QSWARM_WARNINGS})

if(QSWARM_CORE_TESTS)
  add_executable(q_swarm_core_tests
    test/run-core-tests.cc
  )
  target_link_libraries(q_swarm_core_tests PRIVATE q_swarm_core)
  target_compile_definitions(q_swarm_core_tests PRIVATE QSWARM_CORE_EXPECTED_VERSION="${PROJECT_VERSION}")
  target_compile_options(q_swarm_core_tests PRIVATE ${QSWARM_WARNINGS})

  enable_testing()
  add_test(NAME q-swarm-core COMMAND q_swarm_core_tests)
endif()

if(QSWARM_CORE_INSTALL)
  include(CMakePackageConfigHelpers)
  set(_qswarm_core_cmake_dir ${CMAKE_INSTALL_LIBDIR}/cmake/q-swarm-core)
  install(TARGETS q_swarm_core EXPORT q-swarm-coreTargets
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(FILES
    q-swarm-version.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/q-swarm-core)
  install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/q-swarm-core)
  install(EXPORT q-swarm-coreTargets NAMESPACE qswarm:: DESTINATION ${_qswarm_core_cmake_dir})
  configure_package_config_file(q-swarm-coreConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/q-swarm-coreConfig.cmake
    INSTALL_DESTINATION ${_qswarm_core_cmake_dir})
  # As swarm-core (its docs/api-policy.md): before 1.0 any minor may break,
  # so a 0.y release satisfies a request for 0.y only; from 1.0, its major.
  if(PROJECT_VERSION_MAJOR EQUAL 0)
    set(_qswarm_core_compat SameMinorVersion)
  else()
    set(_qswarm_core_compat SameMajorVersion)
  endif()
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/q-swarm-coreConfigVersion.cmake
    COMPATIBILITY ${_qswarm_core_compat})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/q-swarm-coreConfig.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/q-swarm-coreConfigVersion.cmake
          DESTINATION ${_qswarm_core_cmake_dir})
endif()
