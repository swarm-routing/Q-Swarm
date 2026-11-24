// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// The scratch consumer of the installed q-swarm-core package: the headers
// resolve from include/q-swarm-core/ and swarm-core's package, and
// qswarm::core links (QEnergyUpdater on swarm-core's ports).
#include "q-swarm-updater.h"
#include "q-swarm-version.h"

#include <cstdio>

namespace {

class FullBattery final : public swarm::core::IEnergy {
public:
    double residualEnergyFraction() const override { return 1.0; }
};

class FirstDraw final : public swarm::core::IRng {
public:
    double uniform() override { return 0.0; }
    int uniformInt(int /*n*/) override { return 0; }
};

}  // namespace

int main() {
    FullBattery energy;
    FirstDraw rng;
    qswarm::core::QEnergyUpdater u(qswarm::core::Config{}, energy, rng);
    u.Hear(7, 0.0, 1, 1.0, 0.0);
    const bool ok = u.ChooseNextHop() == 7 && u.Learn(7);
    const auto v = qswarm::core::Version();
    std::printf("consumer: %s (q-swarm-core %.*s)\n", ok ? "ok" : "FAIL",
                static_cast<int>(v.size()), v.data());
    return ok ? 0 : 1;
}
