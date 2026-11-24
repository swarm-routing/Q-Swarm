// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// The energy-aware reward of Q-Swarm v0, from the candidate source [ZCX21]
// (QLFR, docs/fidelity.md): r = -c_e(s_i) - c_e(s_j) - c_d(s_i, s_j), eq. 5,
// with the residual-energy cost of eq. 6 and the progress cost of eq. 7 read
// with hop counts in place of depth (the sheet's recorded deviation).
#ifndef Q_SWARM_REWARD_H
#define Q_SWARM_REWARD_H

namespace qswarm::core {

/// The residual-energy cost c_e = 1 - e_res / e_ini ([ZCX21] eq. 6), from the
/// node's residual energy fraction (swarm-core IEnergy). Clamped to [0, 1]; a
/// fraction that is not a number (no energy model) costs 0, so the reward
/// degrades to progress alone instead of ranking on a missing reading.
double EnergyCost(double residualFraction) noexcept;

/// The progress cost c_d = (1 - d / |d|max) / 2 ([ZCX21] eq. 7), with
/// d = hopsSelf - hopsNeighbour, the hop counts to the sink, clamped to
/// [-|d|max, |d|max], so the cost lies in [0, 1]. A non-positive |d|max is
/// read as 1.
double ProgressCost(int hopsSelf, int hopsNeighbour, double maxProgressHops) noexcept;

/// The reward of forwarding from s_i to s_j ([ZCX21] eq. 5): the negated sum
/// of both nodes' energy costs and the progress cost, in [-3, 0].
double Reward(double energyCostSelf, double energyCostNeighbour, double progressCost) noexcept;

}  // namespace qswarm::core

#endif  // Q_SWARM_REWARD_H
