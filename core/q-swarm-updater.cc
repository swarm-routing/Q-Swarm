// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include "q-swarm-updater.h"

#include "q-swarm-reward.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace qswarm::core {

using swarm::core::kInvalidAddress;
using swarm::core::NodeAddress;
using swarm::core::Time;
using swarm::core::WeightedHop;

QEnergyUpdater::QEnergyUpdater(Config config, const swarm::core::IEnergy& energy,
                               swarm::core::IRng& rng, bool isSink)
    : config_(config), energy_(energy), rng_(rng), isSink_(isSink) {}

QEnergyUpdater::Neighbour* QEnergyUpdater::find(NodeAddress neighbour) {
    auto it = std::lower_bound(table_.begin(), table_.end(), neighbour,
                               [](const Neighbour& n, NodeAddress a) { return n.address < a; });
    return it != table_.end() && it->address == neighbour ? &*it : nullptr;
}

void QEnergyUpdater::Hear(NodeAddress neighbour, double advertisedValue, int hops,
                          double residualFraction, Time now) {
    if (neighbour == kInvalidAddress || config_.maxNeighbours == 0) return;
    if (Neighbour* n = find(neighbour)) {
        n->advertisedValue = advertisedValue;
        n->hops = hops;
        n->energyCost = EnergyCost(residualFraction);
        n->lastHeard = now;
        return;
    }
    if (table_.size() >= config_.maxNeighbours) {
        // the entry heard longest ago makes room; on a tie, the lowest address
        auto oldest = std::min_element(
            table_.begin(), table_.end(),
            [](const Neighbour& a, const Neighbour& b) { return a.lastHeard < b.lastHeard; });
        table_.erase(oldest);
    }
    Neighbour entry{
        neighbour, config_.initialQ, advertisedValue, hops, EnergyCost(residualFraction), now};
    auto at = std::lower_bound(table_.begin(), table_.end(), neighbour,
                               [](const Neighbour& n, NodeAddress a) { return n.address < a; });
    table_.insert(at, entry);
}

void QEnergyUpdater::Expire(Time now) {
    std::erase_if(
        table_, [&](const Neighbour& n) { return now - n.lastHeard > config_.neighbourLifetime; });
}

void QEnergyUpdater::weights(NodeAddress exclude, std::vector<WeightedHop>& out) const {
    out.clear();
    if (isSink_) return;
    auto routed = [](const Neighbour& n) { return n.hops != kUnknownHops; };
    const auto candidates = std::count_if(table_.begin(), table_.end(), routed);
    const bool onlyExcluded =
        candidates == 1 && std::any_of(table_.begin(), table_.end(), [&](const Neighbour& n) {
            return routed(n) && n.address == exclude;
        });
    double best = -std::numeric_limits<double>::infinity();
    for (const Neighbour& n : table_) {
        if (!routed(n) || (n.address == exclude && !onlyExcluded)) continue;
        out.push_back({n.address, n.q});
        best = std::max(best, n.q);
    }
    const double t = config_.temperature;
    const bool greedy = !(t > 0.0) || !std::isfinite(t);
    bool taken = false;
    for (WeightedHop& h : out) {
        if (greedy) {
            // T <= 0: the best Q-value only, the lowest address on a tie
            h.weight = (!taken && h.weight == best) ? 1.0 : 0.0;
            taken = taken || h.weight > 0.0;
        } else {
            // e^{Q/T} / sum e^{Q'/T} ([KLM96] §2.2.2), shifted by the best Q so
            // that no weight overflows; the shift cancels in the normalisation
            h.weight = std::exp((h.weight - best) / t);
        }
    }
}

NodeAddress QEnergyUpdater::ChooseNextHop(NodeAddress exclude) {
    weights(exclude, scratch_);
    return swarm::core::rouletteSelect(scratch_, rng_);
}

std::vector<WeightedHop> QEnergyUpdater::Distribution(NodeAddress exclude) const {
    std::vector<WeightedHop> out;
    weights(exclude, out);
    double sum = 0.0;
    for (const WeightedHop& h : out)
        sum += h.weight;
    if (sum > 0.0) {
        for (WeightedHop& h : out)
            h.weight /= sum;
    }
    std::erase_if(out, [](const WeightedHop& h) { return !(h.weight > 0.0); });
    return out;
}

bool QEnergyUpdater::Learn(NodeAddress neighbour) {
    if (isSink_) return false;
    Neighbour* n = find(neighbour);
    if (n == nullptr) return false;
    const int self = HopsToSink();
    const double progress = (self == kUnknownHops || n->hops == kUnknownHops)
                                ? ProgressCost(0, 0, config_.maxProgressHops)
                                : ProgressCost(self, n->hops, config_.maxProgressHops);
    const double r = Reward(EnergyCost(energy_.residualEnergyFraction()), n->energyCost, progress);
    n->q = config_.alpha * (r + config_.gamma * n->advertisedValue) + (1.0 - config_.alpha) * n->q;
    return true;
}

double QEnergyUpdater::Value() const {
    if (isSink_) return 0.0;
    double best = -std::numeric_limits<double>::infinity();
    for (const Neighbour& n : table_) {
        if (n.hops != kUnknownHops) best = std::max(best, n.q);
    }
    return std::isinf(best) ? config_.initialQ : best;
}

int QEnergyUpdater::HopsToSink() const {
    if (isSink_) return 0;
    int fewest = kUnknownHops;
    for (const Neighbour& n : table_) {
        if (n.hops != kUnknownHops && (fewest == kUnknownHops || n.hops < fewest)) fewest = n.hops;
    }
    return fewest == kUnknownHops ? kUnknownHops : fewest + 1;
}

}  // namespace qswarm::core
