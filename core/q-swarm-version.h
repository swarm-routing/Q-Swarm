// Copyright (c) 2026 Daniel Joppi (https://github.com/swarm-routing)
// SPDX-License-Identifier: MIT OR GPL-2.0-only
//
// The skeleton's only symbol: the package version the library was built as.
// No protocol code lives in core/ until docs/fidelity.md is reviewed
// (AGENTS.md golden rule 10); the first slice is the core/ epic's first issue.
#ifndef Q_SWARM_VERSION_H
#define Q_SWARM_VERSION_H

#include <string_view>

namespace qswarm::core {

/// The q-swarm-core package version this library was built as: .cz.toml's,
/// or QSWARM_CORE_VERSION when core/ is built from a release tarball or a port.
std::string_view Version() noexcept;

}  // namespace qswarm::core

#endif  // Q_SWARM_VERSION_H
