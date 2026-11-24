// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// QEnergyUpdater: the neighbour table, the update of [ZCX21] eq. 8 and the
// Boltzmann selection of [KLM96] §2.2.2 (docs/fidelity.md).
#include "q-swarm-test.h"
#include "q-swarm-updater.h"

#include <cmath>
#include <map>
#include <string>
#include <vector>

namespace qswarm::core::test {

namespace {

using swarm::core::kInvalidAddress;
using swarm::core::NodeAddress;

double ProbabilityOf(const std::vector<swarm::core::WeightedHop>& d, NodeAddress node) {
    for (const auto& h : d) {
        if (h.node == node) return h.weight;
    }
    return 0.0;
}

double QOf(const QEnergyUpdater& u, NodeAddress node) {
    for (const auto& n : u.neighbours()) {
        if (n.address == node) return n.q;
    }
    return std::nan("");
}

}  // namespace

void TestHearAndOrder(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);
    u.Hear(7, -1.0, 2, 0.5, 0.0);
    u.Hear(3, -2.0, 1, 1.0, 0.0);
    u.Hear(5, -3.0, kUnknownHops, 1.0, 0.0);
    const auto n = u.neighbours();
    c.Expect(n.size() == 3, "three neighbours heard");
    c.Expect(n.size() == 3 && n[0].address == 3 && n[1].address == 5 && n[2].address == 7,
             "the table is in ascending address order");
    c.Near(QOf(u, 7), Config{}.initialQ, "a new neighbour starts at initialQ");
    u.Hear(7, -0.5, 1, 0.25, 1.0);
    const auto again = u.neighbours();
    c.Expect(again.size() == 3, "hearing a known neighbour adds no entry");
    c.Near(again[2].advertisedValue, -0.5, "a known neighbour's advertised V is refreshed");
    c.Expect(again[2].hops == 1, "a known neighbour's hops are refreshed");
    c.Near(again[2].energyCost, 0.75, "a known neighbour's energy cost is refreshed (eq. 6)");
    u.Hear(kInvalidAddress, 0.0, 1, 1.0, 1.0);
    c.Expect(u.neighbours().size() == 3, "the invalid address is never a neighbour");
}

void TestBellmanUpdate(Checker& c) {
    FixedEnergy energy(0.5);  // this node: c_e = 0.5
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);  // alpha 0.5, gamma 0.8
    u.Hear(2, -1.0, 1, 0.75, 0.0);            // V = -1, 1 hop, c_e = 0.25; this node: 2 hops
    c.Expect(u.Learn(2), "Learn on a known neighbour");
    // r = -0.5 - 0.25 - c_d(2, 1) = -0.75; Q = 0.5 (r + 0.8 * -1) + 0.5 * 0
    c.Near(QOf(u, 2), -0.775, "eq. 8, first step");
    c.Expect(u.Learn(2), "Learn again");
    c.Near(QOf(u, 2), 0.5 * (-0.75 - 0.8) + 0.5 * -0.775, "eq. 8, second step");
    c.Expect(!u.Learn(9), "Learn on an unknown neighbour is refused");
}

void TestConvergesToFixedPoint(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);
    u.Hear(2, -0.4, 1, 1.0, 0.0);  // r = -c_d(2, 1) = 0
    for (int i = 0; i < 80; ++i)
        u.Learn(2);
    // the fixed point of eq. 8 for a fixed neighbour: Q* = r + gamma V
    c.Near(QOf(u, 2), 0.0 + 0.8 * -0.4, "eq. 8 converges to r + gamma V", 1e-9);
}

void TestValueAndHops(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);
    c.Near(u.Value(), Config{}.initialQ, "V is initialQ while no neighbour is known");
    c.Expect(u.HopsToSink() == kUnknownHops, "no neighbour: hops unknown");
    u.Hear(4, -1.0, kUnknownHops, 1.0, 0.0);
    c.Expect(u.HopsToSink() == kUnknownHops, "a neighbour with no route gives no hop count");
    u.Hear(5, -1.0, 3, 1.0, 0.0);
    u.Hear(6, -2.0, 1, 1.0, 0.0);
    c.Expect(u.HopsToSink() == 2, "hops = 1 + the fewest a neighbour advertised");
    u.Learn(5);  // r = -c_d(2, 3) = -1; Q = 0.5 (-1 + 0.8 * -1) = -0.9
    u.Learn(6);  // r = 0; Q = 0.5 (0 + 0.8 * -2) = -0.8
    c.Near(u.Value(), -0.8, "V = max Q over the routed neighbours (eq. 4)");
}

void TestBoltzmannDistribution(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);  // T = 0.1
    u.Hear(1, 0.0, 1, 1.0, 0.0);
    u.Hear(2, -1.0, 1, 1.0, 0.0);
    u.Learn(1);  // Q = 0.5 (0 + 0) = 0
    u.Learn(2);  // Q = 0.5 (0 - 0.8) = -0.4
    const auto d = u.Distribution();
    const double e4 = std::exp(4.0);  // (Q1 - Q2) / T = 0.4 / 0.1
    c.Near(ProbabilityOf(d, 1), e4 / (1.0 + e4), "P = e^{Q/T} / sum e^{Q'/T} ([KLM96] §2.2.2)");
    c.Near(ProbabilityOf(d, 1) + ProbabilityOf(d, 2), 1.0, "the probabilities sum to 1");
    u.Hear(3, 0.0, kUnknownHops, 1.0, 0.0);
    c.Near(ProbabilityOf(u.Distribution(), 3), 0.0, "a neighbour with no route is no candidate");
}

void TestGreedyAtZeroTemperature(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    Config cfg;
    cfg.temperature = 0.0;
    QEnergyUpdater u(cfg, energy, rng);
    u.Hear(1, 0.0, 1, 1.0, 0.0);
    u.Hear(2, 0.0, 1, 1.0, 0.0);
    u.Hear(3, -1.0, 1, 1.0, 0.0);
    u.Learn(3);  // 1 and 2 stay at 0, tied; 3 drops
    const auto d = u.Distribution();
    c.Expect(d.size() == 1 && d[0].node == 1,
             "T = 0: the best Q only, the lowest address on a tie");
    c.Expect(u.ChooseNextHop() == 1, "T = 0 chooses the best");
}

void TestExcludePreviousHop(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);
    u.Hear(1, 0.0, 1, 1.0, 0.0);
    c.Expect(u.ChooseNextHop(1) == 1, "the previous hop is kept when it is the only candidate");
    u.Hear(2, 0.0, 1, 1.0, 0.0);
    const auto d = u.Distribution(1);
    c.Expect(d.size() == 1 && d[0].node == 2, "the previous hop is left out otherwise");
    QEnergyUpdater empty(Config{}, energy, rng);
    c.Expect(empty.ChooseNextHop() == kInvalidAddress, "no neighbour: no next hop");
}

void TestSelectionIsDeterministic(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rngA(42);
    SeededRng rngB(42);
    QEnergyUpdater a(Config{}, energy, rngA);
    QEnergyUpdater b(Config{}, energy, rngB);
    for (QEnergyUpdater* u : {&a, &b}) {
        u->Hear(1, 0.0, 1, 1.0, 0.0);
        u->Hear(2, -0.1, 1, 1.0, 0.0);
        u->Hear(3, -0.2, 1, 1.0, 0.0);
        for (NodeAddress n : {1, 2, 3})
            u->Learn(n);
    }
    bool same = true;
    std::map<NodeAddress, int> counts;
    const int draws = 20000;
    for (int i = 0; i < draws; ++i) {
        const NodeAddress x = a.ChooseNextHop();
        same = same && x == b.ChooseNextHop();
        ++counts[x];
    }
    c.Expect(same, "the same seed gives the same selections (golden rule 5)");
    const auto d = a.Distribution();
    for (const auto& h : d) {
        const double freq = static_cast<double>(counts[h.node]) / draws;
        c.Near(freq, h.weight, "draw frequency of " + std::to_string(h.node), 0.02);
    }
}

void TestSelectionPrefersEnergy(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);
    u.Hear(1, 0.0, 1, 1.0, 0.0);  // full battery
    u.Hear(2, 0.0, 1, 0.1, 0.0);  // nearly empty: c_e = 0.9
    u.Learn(1);
    u.Learn(2);
    const auto d = u.Distribution();
    c.Expect(ProbabilityOf(d, 1) > 0.95,
             "the neighbour with more residual energy is preferred (eqs. 5-6)");
}

void TestCapAndEviction(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    Config cfg;
    cfg.maxNeighbours = 2;
    QEnergyUpdater u(cfg, energy, rng);
    u.Hear(10, 0.0, 1, 1.0, 0.0);
    u.Hear(20, 0.0, 1, 1.0, 1.0);
    u.Hear(10, 0.0, 1, 1.0, 2.0);  // 10 heard again: 20 is now the oldest
    u.Hear(30, 0.0, 1, 1.0, 3.0);
    const auto n = u.neighbours();
    c.Expect(n.size() == 2, "the table never exceeds its cap (golden rule 7)");
    c.Expect(n.size() == 2 && n[0].address == 10 && n[1].address == 30,
             "the entry heard longest ago makes room");
    Config none;
    none.maxNeighbours = 0;
    QEnergyUpdater z(none, energy, rng);
    z.Hear(1, 0.0, 1, 1.0, 0.0);
    c.Expect(z.neighbours().empty(), "a cap of 0 keeps nothing");
}

void TestExpiry(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater u(Config{}, energy, rng);  // lifetime 3 s
    u.Hear(1, 0.0, 1, 1.0, 0.0);
    u.Hear(2, 0.0, 1, 1.0, 2.0);
    u.Expire(3.0);
    c.Expect(u.neighbours().size() == 2, "an entry heard exactly a lifetime ago is kept");
    u.Expire(4.0);
    const auto n = u.neighbours();
    c.Expect(n.size() == 1 && n[0].address == 2, "an entry older than its lifetime is dropped");
}

void TestSink(Checker& c) {
    FixedEnergy energy(1.0);
    SeededRng rng(1);
    QEnergyUpdater sink(Config{}, energy, rng, true);
    sink.Hear(1, -1.0, 1, 1.0, 0.0);
    c.Expect(sink.IsSink(), "a sink knows it is one");
    c.Near(sink.Value(), 0.0, "the sink advertises V = 0");
    c.Expect(sink.HopsToSink() == 0, "the sink is 0 hops from itself");
    c.Expect(sink.ChooseNextHop() == kInvalidAddress, "the sink never forwards");
    c.Expect(!sink.Learn(1), "the sink never learns a next hop");
}

}  // namespace qswarm::core::test
