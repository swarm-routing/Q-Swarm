# CONTEXT.md — orientation and history

Background an agent or a new contributor needs that does not belong in
`AGENTS.md`: domain notes, current state, glossary, open questions. Read the
section a task needs, not the whole file.

## Current state

Set up on the org templates (swarm-routing/.github#881, #35), 2026-11-24:
README, AGENTS/CLAUDE, LICENSING, CITATION.cff, `docs/roadmap.md` (the release
ladder from v0.1.0), `docs/fidelity.md` started, `docs/references.bib`. No
protocol code: the source paper is not named (#23), so every paper row of the
fidelity sheet is `unknown` (AGENTS.md golden rule 10).

History: the repository was created under swarm-routing/.github#672 (Phase 3,
the hexagonal multi-runtime plan) as epic #3. swarm-routing/.github#54 and
swarm-routing/.github#265 record the "Q-Swarm hybrid" as a design that names no
paper; decision 18 of swarm-routing/.github#672 says Q-Swarm names the paper it
reproduces, or moves under swarm-routing/.github#252 with
swarm-routing/.github#264's gates. Decision 6 keeps Q-routing itself as a
comparator in swarm-routing/sim-tools (swarm-routing/sim-tools#34).

## Glossary

- **Q-routing**: Boyan & Littman's (1993) per-node table of estimated
  delivery times, one per (destination, neighbour), updated from the
  neighbour's own best estimate; the comparator here.
- **Bellman update**: the Q-learning (Watkins & Dayan, 1992) step that moves an
  estimate toward the observed cost plus the next node's best estimate.
- **Energy-aware**: the update's target also weighs the neighbour's residual
  energy (read through swarm-core's `IEnergyProvider`); how, is the source
  paper's (#23).
- **Boltzmann selection**: choosing the next hop with probability proportional
  to `exp(-Q/τ)` (softmax over the costs), with temperature τ.

## Open questions

- Which paper does Q-Swarm reproduce (#23)? Every `unknown` row of
  `docs/fidelity.md` waits on it, and so does the anchor's trend.
