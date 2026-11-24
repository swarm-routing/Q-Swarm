# CONTEXT.md — orientation and history

Background an agent or a new contributor needs that does not belong in
`AGENTS.md`: domain notes, current state, glossary, open questions. Read the
section a task needs, not the whole file.

## Current state

Set up on the org templates (swarm-routing/.github#881, #35), 2026-11-24:
README, AGENTS/CLAUDE, LICENSING, CITATION.cff, `docs/roadmap.md` (the release
ladder from v0.1.0), `docs/fidelity.md` started, `docs/references.bib`. No
protocol code. The source paper is not named (#23). Under the maintainer's
2026-11-24 decision, the fidelity sheet is a v0 from public sources: QLFR
(Zhou, Cao & Xiang, 2021) is the candidate for the energy-aware Bellman
update, and Kaelbling, Littman & Moore (1996) give Boltzmann selection. Code
waits for the maintainer to accept it (AGENTS.md golden rule 10).

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

- Which paper does Q-Swarm reproduce (#23)? The v0 sheet's candidate is QLFR,
  an underwater protocol. A paper with a terrestrial-WSN trend would give the
  anchor its source; that anchor row is still `unknown`.
- Several open values are repo choices waiting for a sweep: α, T, the initial
  Q, and the neighbour lifetime.
