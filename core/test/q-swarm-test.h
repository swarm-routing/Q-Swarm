// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// The core tests' tiny harness: each test records failures in a Checker, and
// run-core-tests.cc walks the list (no framework, no simulator).
#ifndef Q_SWARM_TEST_H
#define Q_SWARM_TEST_H

#include "swarm-ports.h"

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace qswarm::core::test {

class Checker {
public:
    void Expect(bool ok, const std::string& what) {
        if (!ok) failures_.push_back(what);
    }
    void Near(double got, double want, const std::string& what, double tol = 1e-12) {
        Expect(std::fabs(got - want) <= tol,
               what + ": got " + std::to_string(got) + ", want " + std::to_string(want));
    }
    const std::vector<std::string>& failures() const { return failures_; }

private:
    std::vector<std::string> failures_;
};

struct NamedTest {
    const char* name;
    void (*run)(Checker&);
};

/// A fixed residual energy fraction.
class FixedEnergy final : public swarm::core::IEnergy {
public:
    explicit FixedEnergy(double fraction) : fraction_(fraction) {}
    double residualEnergyFraction() const override { return fraction_; }
    void set(double fraction) { fraction_ = fraction; }

private:
    double fraction_;
};

/// A seeded 64-bit LCG (Knuth's MMIX constants): the same seed, the same draws.
class SeededRng final : public swarm::core::IRng {
public:
    explicit SeededRng(std::uint64_t seed) : state_(seed) {}
    double uniform() override {
        state_ = state_ * 6364136223846793005ULL + 1442695040888963407ULL;
        return static_cast<double>(state_ >> 11) * 0x1.0p-53;
    }
    int uniformInt(int n) override { return n <= 0 ? 0 : static_cast<int>(uniform() * n); }

private:
    std::uint64_t state_;
};

void TestEnergyCost(Checker& c);
void TestProgressCost(Checker& c);
void TestReward(Checker& c);
void TestHearAndOrder(Checker& c);
void TestBellmanUpdate(Checker& c);
void TestConvergesToFixedPoint(Checker& c);
void TestValueAndHops(Checker& c);
void TestBoltzmannDistribution(Checker& c);
void TestGreedyAtZeroTemperature(Checker& c);
void TestExcludePreviousHop(Checker& c);
void TestSelectionIsDeterministic(Checker& c);
void TestSelectionPrefersEnergy(Checker& c);
void TestCapAndEviction(Checker& c);
void TestExpiry(Checker& c);
void TestSink(Checker& c);

}  // namespace qswarm::core::test

#endif  // Q_SWARM_TEST_H
