// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// The scratch consumer of the installed q-swarm-core package: the headers
// resolve from include/q-swarm-core/, and qswarm::core links.
#include "q-swarm-version.h"

#include <cstdio>

int main() {
    const auto v = qswarm::core::Version();
    std::printf("consumer: ok (q-swarm-core %.*s)\n", static_cast<int>(v.size()), v.data());
    return v.empty() ? 1 : 0;
}
