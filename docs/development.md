# Developer guide

Q-Swarm is set up (swarm-routing/.github#881) with no protocol code yet: the
source paper is still to be named (#23), and `core/` holds the skeleton only
(the CMake package `q-swarm-core`, its tests and CI). This guide says what runs
today; each section grows with the code. To come: the protocol in `core/`, then
the ns-3 module in `ns3/`, built in an `ns3-dev` container.

## Prerequisites

GNU Make, git, Python 3.11 or newer (the org tooling `make check` fetches),
CMake 3.20 or newer and a C++23 compiler (GCC 13, Clang 18) for `core/`,
`clang-format-18` for `make lint`, and Docker for `make shell`. vcpkg is
optional locally: `make core-test` builds on swarm-core's sources when
`VCPKG_ROOT` is not given.

## Get the code

```bash
git clone https://github.com/swarm-routing/Q-Swarm.git
cd Q-Swarm
```

No submodules or sibling checkouts are needed.

## Dependencies

- **swarm-core**: `core/` builds on its `core/`, at the release
  `swarm-core.lock` names. CI takes it from its vcpkg port
  (`vcpkg.json`, `vcpkg-configuration.json`; AGENTS.md "How this repository
  gets swarm-core"); `make core-test` without `VCPKG_ROOT` fetches the lock's
  tag into `~/.cache/swarm-routing/swarm-core` and builds it as a subproject.
- `make check` fetches swarm-routing/.github's tooling into
  `~/.cache/swarm-routing/dotgithub` (the licence-header gate and org-sync).

## Build

```bash
make core-test                              # swarm-core's sources at the lock's tag
make core-test VCPKG_ROOT=/path/to/vcpkg    # as CI: swarm-core from its vcpkg port
cmake -S core -B build/asan -DQSWARM_CORE_SANITIZE=ON -DSWARM_CORE_DIR=/path/to/swarm-core/core
```

The installed package, as the vcpkg port builds it:
`cmake -S core -B build/pkg -DQSWARM_CORE_INSTALL=ON -DQSWARM_CORE_TESTS=OFF`
(with swarm-core's package on `CMAKE_PREFIX_PATH` or the vcpkg toolchain),
then `cmake --install build/pkg --prefix <dir>`; `core/test/consumer-package/`
is a `find_package(q-swarm-core)` consumer. `make build` says `no ns3/ module
yet` until `ns3/` exists.

## Test

`make check` is the gate: lint, `make test` (the core tests), licence headers
and the org-sync check, as CI runs them.

```bash
make check
```

## Run

Nothing runs yet: the library has no protocol code.

## Docker and dev containers

`make shell` opens a shell in the `ghcr.io/swarm-routing/ns3-dev` image
with this repository at `/src`. There is no `.devcontainer/`.

## Lint and format

`make lint` (clang-format-18 with the org's `.clang-format`, as `core.yml`)
and `make format`. `core.yml` also runs clang-tidy-18 with the org's
`.clang-tidy`, findings as errors. Licence headers: `make headers`, the org's
`check-headers` gate, with swarm-core's `core/` as the include boundary.

## CI map

| Workflow | What it checks | Required |
| --- | --- | --- |
| `org-sync.yml` | the org-managed files match the pinned templates | yes (`check / org-sync`) |
| `licence.yml` | SPDX headers, the licence texts, `core/`'s includes (swarm-core's `core/` by commit) | yes (`headers / licence`) |
| `pr-title.yml` | the PR title is a lowercase Conventional Commit | yes (`title / title`) |
| `core.yml` | `core/` alone: strict warnings, tests, ASan + UBSan, clang-tidy, the package and its consumer, `make dist`, clang-format | yes (`core`) |
| `swarm-core-pin.yml` | `swarm-core.lock`, the Makefile, `core-deps` and the vcpkg override and baseline name one current swarm-core release | yes (`swarm-core / pin`) |
| `review-gate.yml` | CodeRabbit has reviewed the PR | yes (`review / coderabbit`) |
| `release.yml` | the release, on the maintainer's dispatch only | no |
| `dco.yml` | an outside contributor signed off | no |
| `scorecard.yml`, `bestpractices-evidence.yml` | OpenSSF Scorecard; best-practices evidence | no |
| `settings-apply.yml`, `epics-changed.yml` | nudge the org's repo-settings and epic lists | no |

The required checks are the ones the `main` ruleset lists (`gh api
repos/swarm-routing/Q-Swarm/rules/branches/main`), applied from
`.github/org-sync.toml` `[settings]`.

## Release

Nothing is released yet, and releases are cut on the maintainer's request
only. `release.yml` calls swarm-routing/.github's `reusable-release.yml`
([CONTRIBUTING "Cutting a release"](https://github.com/swarm-routing/.github/blob/main/CONTRIBUTING.md#cutting-a-release)):
a `bump` dispatch opens the version PR (Commitizen, `.cz.toml`; the version
needs its row in [`roadmap.md`](roadmap.md)), and merging it builds
`make dist`'s `q-swarm-<ver>.tar.gz`, signs and publishes it, and opens the
`q-swarm-core` port PR in swarm-routing/vcpkg-registry (`vcpkg-ports`).

## Troubleshooting

- `make check` fails fetching the org tooling or swarm-core: it needs network
  access to github.com.
- `find_package(swarm-core)` fails: pass the vcpkg toolchain
  (`make core-test VCPKG_ROOT=...`), or build without it (`make core-test`).
