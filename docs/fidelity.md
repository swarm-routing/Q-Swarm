# Q-Swarm: fidelity sheet

What the paper specifies, what this implementation does, and where they differ.
Every row cites its source (`[KEY] §x`, `Table N`, `Fig. N`, `eq. N`), or says
`repo choice: <reason>`, or `unknown`. Checked by
`swarm-routing/.github` `tools/fidelity/fidelity.py check`.

**Status: started, not reviewed.** Q-Swarm's source paper is not named yet:
[#23](https://github.com/swarm-routing/Q-Swarm/issues/23) waits on the
maintainer (swarm-routing/.github#672, decision 18). Until it is, every row
below that depends on that paper is `unknown`, and no protocol code is written
(AGENTS.md golden rule 10). The rows list the mechanisms the plan names
(swarm-routing/.github#672, epic #3): a per-node Q-value table, an
energy-aware Bellman update, Boltzmann next-hop selection, and the control
header. When #23 names the paper, its entry goes into
[`references.bib`](references.bib) with the `references` skill, the Sources
table is rendered again (`fidelity.py bib sources .`), and each row gets its
section, equation or table, or `repo choice: <reason>`.

[BL93] is Q-routing, the comparator Q-Swarm is benchmarked against (org
ROADMAP decision 6, swarm-routing/sim-tools#34); [WD92] is Q-learning, the
update both build on. Neither is a source for a Q-Swarm value: a row cites
them only where the source paper says it takes that mechanism from them.

## Sources

<!-- fidelity:sources:begin -->
| Key | Citation | Link |
|---|---|---|
| BL93 | Boyan & Littman, *Packet Routing in Dynamically Changing Networks: A Reinforcement Learning Approach*, 1993 | <https://proceedings.neurips.cc/paper_files/paper/1993/hash/4ea06fbc83cdd0a06020c35d50e1e89a-Abstract.html> |
| WD92 | Watkins & Dayan, *Q-learning*, Machine Learning 8(3-4), 1992, pp. 279–292 | <https://doi.org/10.1007/bf00992698> |
<!-- fidelity:sources:end -->

The table is generated from [`docs/references.bib`](references.bib) by
`fidelity.py bib sources`: change an entry there, never here. The source paper
Q-Swarm reproduces is not in it yet (#23).

## Mechanisms

| Mechanism | Source | Implementation | Status |
|---|---|---|---|
| Q-value table: one estimate per (destination, neighbour), its unit (delivery time, hops, energy or a mix) | unknown | — | unknown: waits for #23 |
| Energy-aware Bellman update: how residual energy enters the target, and when an update is sent (per data packet, per ack, periodic) | unknown | — | unknown: waits for #23 |
| Boltzmann (softmax) next-hop selection over the Q-values | unknown | — | unknown: waits for #23 |
| Temperature schedule (fixed, or annealed) | unknown | — | unknown: waits for #23 |
| Q-value initialisation and an unknown neighbour's estimate | unknown | — | unknown: waits for #23 |
| Neighbour discovery and loss (hello, link-layer feedback, timeout) | unknown | — | unknown: waits for #23 |
| Loop and duplicate handling | unknown | — | unknown: waits for #23 |
| Energy model: what the node reads as residual energy (swarm-core `IEnergyProvider`) | unknown | — | unknown: waits for #23 |
| Control header: its fields and units, behind swarm-core's versioned wire header (golden rule 3) | unknown | — | unknown: waits for #23 |

## Parameters

| Parameter | Value | Source | Notes |
|---|---|---|---|
| Learning rate (α) | — | unknown | waits for #23 |
| Discount (γ), if the update has one | — | unknown | waits for #23 |
| Energy weight in the update | — | unknown | waits for #23 |
| Boltzmann temperature (τ) and its schedule | — | unknown | waits for #23 |
| Initial Q-value | — | unknown | waits for #23 |
| Neighbour timeout / hello interval | — | unknown | waits for #23 |
| Initial battery energy and drain model | — | unknown | waits for #23 |

## Anchor candidate

| Trend | Scenario | Metric | Comparators | Source |
|---|---|---|---|---|
| The source paper's own trend (golden rule 6's third gate) | WSN many-to-one convergecast with battery depletion (epic #3) | unknown | Q-routing [BL93] | unknown |
