// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// Standalone runner of the core tests, no simulator (core/CMakeLists.txt,
// ctest). Exits non-zero when a test failed.
#include "q-swarm-test.h"
#include "q-swarm-version.h"

#include <array>
#include <cstdio>
#include <string_view>

namespace qswarm::core::test {
namespace {

void TestVersion(Checker& c) {
    c.Expect(Version() == std::string_view{QSWARM_CORE_EXPECTED_VERSION},
             "the library reports the package version it was built as");
}

constexpr std::array<NamedTest, 16> kTests{{
    {"version: the package version", TestVersion},
    {"reward: residual-energy cost (eq. 6)", TestEnergyCost},
    {"reward: progress cost, hops for depth (eq. 7)", TestProgressCost},
    {"reward: r = -c_e - c_e - c_d (eq. 5)", TestReward},
    {"table: hear, refresh, ascending order", TestHearAndOrder},
    {"update: the Bellman step (eq. 8)", TestBellmanUpdate},
    {"update: converges to r + gamma V", TestConvergesToFixedPoint},
    {"table: V and hops to the sink", TestValueAndHops},
    {"selection: the Boltzmann distribution", TestBoltzmannDistribution},
    {"selection: greedy at T = 0", TestGreedyAtZeroTemperature},
    {"selection: the previous hop", TestExcludePreviousHop},
    {"selection: seeded and true to its distribution", TestSelectionIsDeterministic},
    {"selection: prefers residual energy", TestSelectionPrefersEnergy},
    {"table: cap and eviction", TestCapAndEviction},
    {"table: expiry", TestExpiry},
    {"sink: V = 0, never forwards", TestSink},
}};

}  // namespace
}  // namespace qswarm::core::test

int main() {
    using namespace qswarm::core::test;
    int failed = 0;
    for (const auto& t : kTests) {
        Checker c;
        t.run(c);
        for (const auto& f : c.failures())
            std::fprintf(stderr, "FAIL %s: %s\n", t.name, f.c_str());
        std::printf("%s %s\n", c.failures().empty() ? "ok  " : "FAIL", t.name);
        if (!c.failures().empty()) ++failed;
    }
    std::printf("%d failed\n", failed);
    return failed == 0 ? 0 : 1;
}
