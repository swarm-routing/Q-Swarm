// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include "q-swarm-version.h"

namespace qswarm::core {

std::string_view Version() noexcept {
    return QSWARM_CORE_VERSION_STRING;
}

}  // namespace qswarm::core
