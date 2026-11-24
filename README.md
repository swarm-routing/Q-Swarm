# Q-Swarm

<!-- org-sync:begin badges -->
| Group | Badges |
| --- | --- |
| Build | [![CI](https://github.com/swarm-routing/Q-Swarm/actions/workflows/core.yml/badge.svg)](https://github.com/swarm-routing/Q-Swarm/actions/workflows/core.yml) [![Built on: ns3-dev](https://img.shields.io/badge/built%20on-ns3--dev%20image-blue?logo=docker)](https://github.com/orgs/swarm-routing/packages/container/package/ns3-dev) |
| Quality | [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=swarm-routing_Q-Swarm&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=swarm-routing_Q-Swarm) [![Bugs](https://sonarcloud.io/api/project_badges/measure?project=swarm-routing_Q-Swarm&metric=bugs)](https://sonarcloud.io/summary/new_code?id=swarm-routing_Q-Swarm) [![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=swarm-routing_Q-Swarm&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=swarm-routing_Q-Swarm) [![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=swarm-routing_Q-Swarm&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=swarm-routing_Q-Swarm) [![CodeRabbit Pull Request Reviews](https://img.shields.io/coderabbit/prs/github/swarm-routing/Q-Swarm?utm_source=oss&utm_medium=github&utm_campaign=swarm-routing%2FQ-Swarm&labelColor=171717&color=FF570A&link=https%3A%2F%2Fcoderabbit.ai&label=CodeRabbit+Reviews)](https://coderabbit.ai) |
| Release | [![Release](https://img.shields.io/github/v/release/swarm-routing/Q-Swarm?sort=semver&cacheSeconds=1800)](https://github.com/swarm-routing/Q-Swarm/releases) [![Cite](https://img.shields.io/badge/cite-CITATION.cff-blue)](CITATION.cff) [![Licence: GPL-2.0-only, core/ MIT OR GPL-2.0-only](https://img.shields.io/badge/licence-GPL--2.0--only%2C%20core%2F%20MIT%20OR%20GPL--2.0--only-blue)](LICENSING.md) |
| Trust | [![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/swarm-routing/Q-Swarm/badge)](https://scorecard.dev/viewer/?uri=github.com/swarm-routing/Q-Swarm) [![OpenSSF Best Practices](https://www.bestpractices.dev/projects/17069/badge)](https://www.bestpractices.dev/projects/17069) |
| Stack | [![C++23](https://img.shields.io/badge/C%2B%2B-23-blue?logo=cplusplus)](AGENTS.md) [![Runs on ns-3](https://img.shields.io/badge/runs%20on-ns--3-informational)](https://www.nsnam.org) |
| Community & docs | [![Colony Arena: Q-Swarm](https://img.shields.io/badge/Colony%20Arena-project%20page-orange)](https://swarm-routing.github.io/projects/Q-Swarm/) [![Open RCAPA](https://img.shields.io/github/issues/swarm-routing/Q-Swarm/rcapa?label=open%20RCAPA&color=blue)](https://github.com/swarm-routing/Q-Swarm/blob/main/docs/rcapa/README.md) [![Conventional Commits](https://img.shields.io/badge/Conventional%20Commits-1.0.0-yellow)](https://www.conventionalcommits.org) |
<!-- org-sync:end badges -->

Q-Swarm: Q-routing with energy-aware Bellman updates and Boltzmann next-hop selection, bridging classic pheromone tables with lightweight on-node reinforcement learning, for benchmark comparisons against the swarm protocols.

**Status: set up; v0 core from public sources.** Q-Swarm
names no paper of its own
([#23](https://github.com/swarm-routing/Q-Swarm/issues/23) stays open for the
maintainer). Following the maintainer's decision of 2026-11-24 (protocols
without a paper are built first from public sources), the
[fidelity sheet](docs/fidelity.md) is a **v0 from public and secondary
sources**. Its candidate source is QLFR (Zhou, Cao & Xiang, 2021,
<https://doi.org/10.1109/jiot.2020.3042901>) for the energy-aware Bellman
update, and Boltzmann exploration (Kaelbling, Littman & Moore, 1996,
<https://doi.org/10.1613/jair.301>) for next-hop selection. `core/` holds the
**v0 built from those secondary sources**: the energy-aware reward and
`QEnergyUpdater` (a bounded Q-table, the Bellman update, Boltzmann selection),
with its tests. It is not a reproduction of a paper, and no number from it is a
benchmark result (the anchor gate is open). The plan is epic
[#3](https://github.com/swarm-routing/Q-Swarm/issues/3) and
swarm-routing/.github#672; the release ladder is [docs/roadmap.md](docs/roadmap.md).

## What it will be

- `core/`: the simulator-agnostic algorithm (MIT OR GPL-2.0-only), on
  [swarm-core](https://github.com/swarm-routing/swarm-core)'s ports: a
  per-node Q-value table, an energy-aware Bellman update, Boltzmann
  next-hop selection and a versioned control header. It will install as the
  CMake package `q-swarm-core` and ship as the vcpkg port of the same name in
  [swarm-routing/vcpkg-registry](https://github.com/swarm-routing/vcpkg-registry).
- Thin adapters: an ns-3 module, then Linux.
- Benchmarks against Q-routing (Boyan & Littman, 1993;
  [proceedings page](https://proceedings.neurips.cc/paper_files/paper/1993/hash/4ea06fbc83cdd0a06020c35d50e1e89a-Abstract.html))
  and the swarm protocols, on a many-to-one WSN convergecast with battery
  depletion, after the three gates of org rule 6.

<!-- org-sync:begin develop -->
## Develop

New here? [docs/development.md](docs/development.md) is the developer guide:
the tools to install, how to get the code and its dependencies, how to
build, test, run and lint it, the Docker images, what CI checks and how to
read a red run. How changes are proposed, reviewed and released is the org's
[CONTRIBUTING.md](https://github.com/swarm-routing/.github/blob/main/CONTRIBUTING.md).
Whether a regression here gets an RCAPA (root cause analysis with corrective
and preventive actions), and how: [docs/rcapa/README.md](docs/rcapa/README.md).
<!-- org-sync:end develop -->

<!-- org-sync:begin citing -->
## Releases & citing

Versions follow [SemVer](https://semver.org). Each release is listed under [Releases](https://github.com/swarm-routing/Q-Swarm/releases), built by the org's release workflow and archived on [Zenodo](https://zenodo.org) with its own DOI.

To cite this repository, use the “Cite this repository” button (from [CITATION.cff](CITATION.cff)). It has no DOI yet: the concept DOI appears here and in the badge above once Zenodo archives its first release.

Please also cite the work it builds on:

- Y. Zhou, T. Cao, W. Xiang, *Anypath Routing Protocol Design via Q-Learning for Underwater Sensor Networks*, IEEE Internet of Things Journal 8(10), pp. 8173–8190, 2021, [doi:10.1109/jiot.2020.3042901](https://doi.org/10.1109/jiot.2020.3042901). QLFR: the public candidate source for Q-Swarm v0 (energy-aware Bellman update, eqs. 5-8), read from its open preprint arXiv:2002.09623, until #23 names the paper
- D. H. Joppi, *swarm-core*, 2026, [doi:10.5281/zenodo.24476619](https://doi.org/10.5281/zenodo.24476619). The shared swarm-routing primitives core/ builds on (IEnergy, IRng, rouletteSelect), at the release swarm-core.lock names (v2.0.0)

What this implementation reproduces from its sources, and every deliberate deviation, is in [docs/fidelity.md](docs/fidelity.md).

Using it in your work? Contributions are welcome, and citing it helps just as much.
<!-- org-sync:end citing -->

## Epics

<!-- epics:begin -->
<!-- Generated by swarm-routing/.github tools/epics/epics.py from the issues labelled `epic`; edit the issues, then re-run it. -->

What is planned and in progress here: the issues labelled [`epic`](https://github.com/swarm-routing/Q-Swarm/issues?q=label%3Aepic). An epic's open sub-issues carry a `priority:*` and a `model:*` label, so a contributor or an AI agent can pick one up ([how issues work](https://github.com/swarm-routing/.github/blob/main/CONTRIBUTING.md#issues)). The whole org's plan: the [roadmap](https://github.com/swarm-routing/.github/blob/main/ROADMAP.md#epics).

| Epic | Status | Sub-issues closed |
|------|--------|-------------------|
| [#3](https://github.com/swarm-routing/Q-Swarm/issues/3) Bootstrap Q-Swarm protocol on swarm-core (QEnergyUpdater, fidelity, contract tests) | Planned | 0 / 2 |
| [#38](https://github.com/swarm-routing/Q-Swarm/issues/38) core/: Q-Swarm's simulator-free core, written from the reviewed fidelity sheet | Planned | 0 / 1 |

<!-- epics:end -->

## Licence

GPL-2.0-only, with `core/` MIT OR GPL-2.0-only: see [LICENSING.md](LICENSING.md).
