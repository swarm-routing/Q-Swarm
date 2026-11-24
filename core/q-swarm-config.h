// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// Q-Swarm v0's constants. Each one names its docs/fidelity.md "Parameters"
// row: a value from the candidate source [ZCX21] (QLFR), or a repo choice.
// v0 comes from public and secondary sources (maintainer decision 2026-11-24,
// #23), so no value here is a claim of paper fidelity.
#ifndef Q_SWARM_CONFIG_H
#define Q_SWARM_CONFIG_H

#include "swarm-types.h"

#include <cstddef>

namespace qswarm::core {

struct Config {
    /// Learning rate α of the update ([ZCX21] eq. 8). Repo choice: [ZCX21]
    /// defines α ∈ (0, 1] (eq. 3) and gives no value (row "Learning rate α").
    double alpha = 0.5;
    /// Discount factor γ: [ZCX21] Table 2 (row "Discount factor γ").
    double gamma = 0.8;
    /// Boltzmann temperature T ([KLM96] §2.2.2). Repo choice: rewards lie in
    /// [-3, 0] (eq. 5) (row "Boltzmann temperature T").
    double temperature = 0.1;
    /// A new neighbour's Q-value. Repo choice: the reward's upper bound, 0,
    /// optimistic (row "Initial Q-value").
    double initialQ = 0.0;
    /// |d|max of the progress cost ([ZCX21] eq. 7), in hops. Repo choice: a
    /// neighbour is at most one hop nearer the sink (row "|d|max").
    double maxProgressHops = 1.0;
    /// How long a neighbour entry lives without being heard again, in seconds.
    /// Repo choice: 3 × the adapter's 1 s beacon interval (row "Neighbour entry
    /// lifetime").
    swarm::core::Time neighbourLifetime = 3.0;
    /// The neighbour table's cap (golden rule 7). Repo choice (row "Neighbour
    /// table cap").
    std::size_t maxNeighbours = 64;
};

}  // namespace qswarm::core

#endif  // Q_SWARM_CONFIG_H
