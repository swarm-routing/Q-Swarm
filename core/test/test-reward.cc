// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// The reward of [ZCX21] eqs. 5-7 (docs/fidelity.md), with hop counts for depth.
#include "q-swarm-reward.h"
#include "q-swarm-test.h"

#include <limits>

namespace qswarm::core::test {

void TestEnergyCost(Checker& c) {
    c.Near(EnergyCost(1.0), 0.0, "a full battery costs 0 (eq. 6)");
    c.Near(EnergyCost(0.25), 0.75, "c_e = 1 - e_res / e_ini (eq. 6)");
    c.Near(EnergyCost(0.0), 1.0, "an empty battery costs 1");
    c.Near(EnergyCost(1.5), 0.0, "a fraction above 1 is clamped");
    c.Near(EnergyCost(-0.5), 1.0, "a fraction below 0 is clamped");
    c.Near(EnergyCost(std::numeric_limits<double>::quiet_NaN()), 0.0,
           "no energy model (NaN) costs 0, not a wrong ranking");
}

void TestProgressCost(Checker& c) {
    c.Near(ProgressCost(3, 2, 1.0), 0.0, "one hop nearer the sink costs 0 (eq. 7, d = |d|max)");
    c.Near(ProgressCost(3, 3, 1.0), 0.5, "a sideways hop costs 1/2 (d = 0)");
    c.Near(ProgressCost(3, 4, 1.0), 1.0, "one hop farther costs 1 (d = -|d|max)");
    c.Near(ProgressCost(5, 1, 1.0), 0.0, "d is clamped to |d|max");
    c.Near(ProgressCost(1, 5, 1.0), 1.0, "d is clamped to -|d|max");
    c.Near(ProgressCost(4, 1, 1.0), 0.0,
           "a stale neighbour count (d = 3) keeps c_d in [0, 1], not -1 (#43)");
    c.Near(ProgressCost(1, 4, 1.0), 1.0, "d = -3 is clamped too");
    c.Near(ProgressCost(3, 2, 0.0), 0.0, "a non-positive |d|max is read as 1");
    c.Near(ProgressCost(4, 2, 2.0), 0.0, "|d|max = 2: two hops nearer costs 0");
    c.Near(ProgressCost(4, 3, 2.0), 0.25, "|d|max = 2: one hop nearer costs 1/4");
}

void TestReward(Checker& c) {
    c.Near(Reward(0.25, 0.5, 0.0), -0.75, "r = -c_e(s_i) - c_e(s_j) - c_d (eq. 5)");
    c.Near(Reward(0.0, 0.0, 0.0), 0.0, "the reward's upper bound is 0");
    c.Near(Reward(1.0, 1.0, 1.0), -3.0, "the reward's lower bound is -3");
}

}  // namespace qswarm::core::test
