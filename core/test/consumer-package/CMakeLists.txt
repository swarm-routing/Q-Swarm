# Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
# SPDX-License-Identifier: MIT OR GPL-2.0-only
#
# A scratch consumer of the installed package q-swarm-core (core.yml's package
# check): find_package finds it and its swarm-core dependency, qswarm::core links.
cmake_minimum_required(VERSION 3.20)
project(q_swarm_core_package_consumer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(q-swarm-core CONFIG REQUIRED)
message(STATUS "q-swarm-core ${q-swarm-core_VERSION} from ${q-swarm-core_DIR}")
add_executable(consumer consumer.cc)
target_link_libraries(consumer PRIVATE qswarm::core)
