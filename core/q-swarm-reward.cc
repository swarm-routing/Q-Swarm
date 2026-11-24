// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include "q-swarm-reward.h"

#include <algorithm>
#include <cmath>

namespace qswarm::core {

double EnergyCost(double residualFraction) noexcept {
    if (std::isnan(residualFraction)) return 0.0;
    return 1.0 - std::clamp(residualFraction, 0.0, 1.0);
}

double ProgressCost(int hopsSelf, int hopsNeighbour, double maxProgressHops) noexcept {
    const double dmax = maxProgressHops > 0.0 ? maxProgressHops : 1.0;
    const double d =
        std::clamp(static_cast<double>(hopsSelf) - static_cast<double>(hopsNeighbour), -dmax, dmax);
    return 0.5 * (1.0 - d / dmax);
}

double Reward(double energyCostSelf, double energyCostNeighbour, double progressCost) noexcept {
    return -energyCostSelf - energyCostNeighbour - progressCost;
}

}  // namespace qswarm::core
