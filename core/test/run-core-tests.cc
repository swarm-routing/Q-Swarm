// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// Standalone runner of the core tests, no simulator (core/CMakeLists.txt,
// ctest). The skeleton's tests: the library links, it reports the package
// version it was built as, and swarm-core's headers resolve through its
// package. Exits non-zero when a test failed.
#include "q-swarm-version.h"
#include "swarm-types.h"

#include <cstdio>
#include <string_view>
#include <type_traits>

namespace {

int failed = 0;

void Check(bool ok, const char* name) {
    std::printf("%s %s\n", ok ? "ok  " : "FAIL", name);
    if (!ok) ++failed;
}

}  // namespace

int main() {
    Check(qswarm::core::Version() == std::string_view{QSWARM_CORE_EXPECTED_VERSION},
          "version: the library reports the package version it was built as");
    Check(std::is_integral_v<swarm::core::NodeAddress>,
          "swarm-core: its headers resolve from its package");
    std::printf("%d failed\n", failed);
    return failed == 0 ? 0 : 1;
}
