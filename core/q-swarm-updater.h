// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// QEnergyUpdater: Q-Swarm v0's per-node learner (docs/fidelity.md, #39). It
// keeps one Q-value per neighbour toward the sink ([ZCX21] §3.2), learns it
// with the energy-aware Bellman update of [ZCX21] eq. 8 and the reward of
// eq. 5, and picks the next hop by Boltzmann selection over the Q-values
// ([KLM96] §2.2.2, the sheet's recorded deviation from QLFR's priority list).
//
// No simulator header (golden rule 11): the node's residual energy comes from
// swarm-core's IEnergy port, every draw from its IRng port (golden rule 5),
// and time is passed in. The neighbour table is bounded (golden rule 7) and
// kept in ascending address order, so nothing depends on container order.
#ifndef Q_SWARM_UPDATER_H
#define Q_SWARM_UPDATER_H

#include "q-swarm-config.h"
#include "swarm-ports.h"
#include "swarm-selection.h"
#include "swarm-types.h"

#include <span>
#include <vector>

namespace qswarm::core {

/// A hop count that is not known: a neighbour with no route to the sink yet.
inline constexpr int kUnknownHops = -1;

class QEnergyUpdater {
public:
    /// One neighbour's entry: what it advertised ([ZCX21] §5.2: its V-value,
    /// its distance to the sink, its residual energy) and this node's
    /// Q-value for forwarding to it.
    struct Neighbour {
        swarm::core::NodeAddress address = swarm::core::kInvalidAddress;
        double q = 0.0;
        double advertisedValue = 0.0;
        int hops = kUnknownHops;
        double energyCost = 0.0;
        swarm::core::Time lastHeard = 0.0;
    };

    /// `energy` and `rng` must outlive the updater. A sink advertises V = 0 and
    /// 0 hops, and never forwards.
    QEnergyUpdater(Config config, const swarm::core::IEnergy& energy, swarm::core::IRng& rng,
                   bool isSink = false);

    /// A neighbour was heard: it advertised `advertisedValue` (its V),
    /// `hops` to the sink (kUnknownHops when it has none) and its residual
    /// energy fraction. A new neighbour starts at Config::initialQ; when the
    /// table is full, the entry heard longest ago (then the lowest address)
    /// makes room.
    void Hear(swarm::core::NodeAddress neighbour, double advertisedValue, int hops,
              double residualFraction, swarm::core::Time now);

    /// Drops the entries not heard within Config::neighbourLifetime of `now`.
    void Expire(swarm::core::Time now);

    /// The next hop for a packet toward the sink, by Boltzmann selection over
    /// the Q-values (one rng.uniform() when there is a candidate), or
    /// kInvalidAddress when there is none or this node is the sink. `exclude`
    /// (the previous hop) is left out unless it is the only candidate.
    swarm::core::NodeAddress
    ChooseNextHop(swarm::core::NodeAddress exclude = swarm::core::kInvalidAddress);

    /// The Boltzmann distribution ChooseNextHop draws from, in ascending
    /// address order; it draws nothing.
    std::vector<swarm::core::WeightedHop>
    Distribution(swarm::core::NodeAddress exclude = swarm::core::kInvalidAddress) const;

    /// After forwarding a packet to `neighbour`: the update of [ZCX21] eq. 8,
    /// Q <- alpha (r + gamma V_neighbour) + (1 - alpha) Q, with r of eq. 5 from
    /// this node's energy now and the neighbour's advertised energy and hops.
    /// False (and nothing changes) for a neighbour not in the table.
    bool Learn(swarm::core::NodeAddress neighbour);

    /// The V-value this node advertises: 0 at the sink, max Q otherwise, or
    /// Config::initialQ while it knows no neighbour.
    double Value() const;

    /// This node's hop count to the sink: 0 at the sink, 1 + the fewest hops a
    /// neighbour advertised, or kUnknownHops.
    int HopsToSink() const;

    bool IsSink() const { return isSink_; }
    const Config& config() const { return config_; }
    std::span<const Neighbour> neighbours() const { return table_; }

private:
    Neighbour* find(swarm::core::NodeAddress neighbour);
    void weights(swarm::core::NodeAddress exclude,
                 std::vector<swarm::core::WeightedHop>& out) const;

    Config config_;
    const swarm::core::IEnergy& energy_;
    swarm::core::IRng& rng_;
    bool isSink_;
    std::vector<Neighbour> table_;  // ascending address, at most Config::maxNeighbours
    std::vector<swarm::core::WeightedHop> scratch_;
};

}  // namespace qswarm::core

#endif  // Q_SWARM_UPDATER_H
