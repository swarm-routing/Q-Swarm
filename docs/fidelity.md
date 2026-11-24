# Q-Swarm: fidelity sheet

What the paper specifies, what this implementation does, and where they differ.
Every row cites its source (`[KEY] §x`, `Table N`, `Fig. N`, `eq. N`), or says
`repo choice: <reason>`, or `unknown`. Checked by
`swarm-routing/.github` `tools/fidelity/fidelity.py check`.

**Status: v0, from public sources. Reviewed as a v0 (CodeRabbit and green
required checks, #43); the maintainer's own review is still to come.**
Q-Swarm names no paper of its own (#23 stays open for the maintainer). On
2026-11-24 the maintainer decided that the protocols without a paper are built
first from public sources: "For the protocols without a paper, build the first
version from the internet knowledge and any other reference available publicly"
(swarm-routing/.github#881, comment 7147077907). This sheet is that first
version. A second decision, on #23 (comment 7147226785), counts CodeRabbit's
review plus green required checks as this v0 sheet's review. The v0 `core/`
(#39) is built on it: `core/q-swarm-reward.*`, `core/q-swarm-updater.*`, and
the constants in `core/q-swarm-config.h`. The maintainer's review and the
choice of paper still gate the anchor and any claim of paper fidelity. The
three gates of org rule 6 are unchanged, and nothing here is a benchmark
number.

## The candidate source, and why

The plan names three mechanisms (#3, swarm-routing/.github#672): a per-node
Q-value table, an energy-aware Bellman update, and Boltzmann next-hop
selection, for a many-to-one sensor network that drains its batteries. No
public paper found has all three. This v0 takes each mechanism from the best
public text that specifies it:

- **[ZCX21] QLFR is the candidate source paper**, for the Q-table, the
  energy-aware Bellman update and how neighbours share their values. Its
  preprint (arXiv:2002.09623) is open and states every equation used here
  (eqs. 3–8, §3.2, §4.2, §5.2, Table 2). It is a per-node, sink-bound
  (convergecast) protocol whose reward has residual-energy terms. It has a
  published comparison against QELAR [HF10] (Fig. 9), which gives an anchor
  trend.
- **[HF10] QELAR** is the better-known original: the first energy-aware
  Q-learning router. Its full text is closed, so no row cites it directly; it
  appears only through [ZCX21] §2 and Fig. 9.
- **[KLM96] §2.2.2 gives Boltzmann exploration**, which QLFR does not use. QLFR
  forwards along a priority list with holding times (§4.1, §4.3), an
  underwater anypath mechanism. Q-Swarm keeps the plan's Boltzmann choice: a
  documented deviation.
- **[BL93] (Q-routing) is the comparator**, cited only to say how Q-Swarm
  differs from it (rule 12).

[ZCX21]'s sections and tables are numbered in Roman numerals with letters (III-B,
Table II); this sheet writes them in Arabic numerals (III-B is §3.2, Table II
is Table 2), as `fidelity.py check` requires. Every locator was read from the
arXiv:2002.09623 text.

Every row marks its provenance in Notes or Status:
- `paper`: from the candidate [ZCX21].
- `secondary: <KEY>`: from another public source.
- `repo choice`: this repository's own choice, with its reason.

Where QLFR is underwater-specific (depth, acoustic speed, anypath holding
time), the sheet records the terrestrial-WSN reading as a deviation.

## Sources

<!-- fidelity:sources:begin -->
| Key | Citation | Link |
|---|---|---|
| BL93 | Boyan & Littman, *Packet Routing in Dynamically Changing Networks: A Reinforcement Learning Approach*, 1993 | <https://proceedings.neurips.cc/paper_files/paper/1993/hash/4ea06fbc83cdd0a06020c35d50e1e89a-Abstract.html> |
| HF10 | Hu & Fei, *QELAR: A Machine-Learning-Based Adaptive Routing Protocol for Energy-Efficient and Lifetime-Extended Underwater Sensor Networks*, IEEE Transactions on Mobile Computing 9(6), 2010, pp. 796–809 | <https://doi.org/10.1109/tmc.2010.28> |
| KLM96 | Kaelbling, Littman & Moore, *Reinforcement Learning: A Survey*, Journal of Artificial Intelligence Research 4, 1996, pp. 237–285 | <https://doi.org/10.1613/jair.301> |
| SwarmCore | Joppi, *swarm-core*, Zenodo, 2026 | <https://doi.org/10.5281/zenodo.24476619> |
| WD92 | Watkins & Dayan, *Q-learning*, Machine Learning 8(3-4), 1992, pp. 279–292 | <https://doi.org/10.1007/bf00992698> |
| ZCX21 | Zhou, Cao & Xiang, *Anypath Routing Protocol Design via Q-Learning for Underwater Sensor Networks*, IEEE Internet of Things Journal 8(10), 2021, pp. 8173–8190 | <https://doi.org/10.1109/jiot.2020.3042901> |
<!-- fidelity:sources:end -->

The table is generated from [`docs/references.bib`](references.bib) by
`fidelity.py bib sources`: change an entry there, never here.

## Mechanisms

| Mechanism | Source | Implementation | Status |
|---|---|---|---|
| Q-value table: per node, one value Q(s_i, a_j) per neighbour s_j toward the sink; the state is the node holding the packet, the action is the next hop | [ZCX21] §3.2 | `QEnergyUpdater`'s table: one entry per neighbour (v0 has one sink), in ascending address order, capped at `maxNeighbours` (rule 7); the entry heard longest ago makes room | as written: paper |
| Bellman update: Q_{k+1}(s_i, a_j) = α[r + γ max_a Q_k(s_j, a)] + (1 − α) Q_k(s_i, a_j), with V_k(s_j) = max_a Q_k(s_j, a) | [ZCX21] eq. 8; [ZCX21] eq. 3 | `QEnergyUpdater::Learn` after each send to s_j, with s_j's advertised V as max_a Q_k(s_j, a) and the reward of eq. 5; `Value()` is max Q over the routed neighbours, 0 at the sink | as written: paper |
| Energy-aware reward: r = − c_e(s_i) − c_e(s_j) − c_d(s_i, s_j), each cost in [0, 1] | [ZCX21] eq. 5 | `Reward()`, with c_d read for a terrestrial WSN (next rows) | as written: paper |
| Residual-energy cost c_e(s) = 1 − e_res(s) / e_ini(s) | [ZCX21] eq. 6 | `EnergyCost()` from swarm-core's `IEnergy::residualEnergyFraction()` (e_res / e_ini), clamped to [0, 1]; a neighbour's from the fraction it advertised. Repo choice: a fraction that is not a number (no energy model) costs 0, so the ranking falls back to progress | as written: paper, with the repo choice for a missing reading |
| Progress cost c_d(s_i, s_j) = ½ (1 − d(s_i, s_j) / \|d\|max), d the depth difference | [ZCX21] eq. 7 | `ProgressCost()`: repo choice, a terrestrial WSN has no depth, so d(s_i, s_j) = hops(s_i) − hops(s_j), the hop counts to the sink, clamped to ±\|d\|max (a stored hop count can be stale, so \|d\| may exceed 1 until the entry is refreshed; the clamp keeps c_d in [0, 1] and r ≤ 0, eq. 5); d = 0 when either is unknown | deviation: hop count to the sink in place of depth, clamped |
| Routing knowledge exchange: each packet header carries the sender's V-value, its depth, residual energy and ID; every node that receives it updates its neighbour entry | [ZCX21] §5.2; [ZCX21] Fig. 5 | `QEnergyUpdater::Hear(address, V, hops, residual fraction, now)` takes what a neighbour advertised; the control header that carries it (golden rule 3) is a later child of #38 | not implemented: the header; the table side as written, with hop count for depth |
| Next-hop selection: Boltzmann (softmax) over the neighbours' Q-values, P(a) = e^{Q(a)/T} / Σ_a' e^{Q(a')/T} | [KLM96] §2.2.2 | `ChooseNextHop()`: weights e^{(Q − Q_max)/T} over the neighbours with a route, drawn by swarm-core's `rouletteSelect` with the injected `IRng` (one draw, rule 5); the previous hop is left out unless it is the only candidate; T ≤ 0 is greedy, the lowest address on a tie | deviation: secondary [KLM96]; QLFR forwards by priority list and holding time ([ZCX21] §4.1, §4.3), which is underwater anypath and not adopted |
| Temperature schedule: a fixed T, or decreased over time to explore less | [KLM96] §2.2.2 | repo choice: a fixed T in v0 (KLM96 warns a schedule needs careful tuning); a decay is a later default-off switch | as written: secondary [KLM96], fixed T |
| Q-value initialisation, and the estimate for a neighbour not yet heard from | unknown | repo choice: `initialQ` = 0 for a new neighbour, the reward's upper bound (r ≤ 0, eq. 5), which makes it optimistic | unknown: QLFR does not state it |
| Neighbour loss: when an entry is dropped | unknown | repo choice: `Expire(now)` drops an entry not heard within `neighbourLifetime` (bounded, rule 7) | unknown: QLFR's nodes move (§7.1) but no timeout is stated |
| Multipath suppression and holding time | [ZCX21] §4.3; [ZCX21] §4.4 | not adopted: Q-Swarm forwards each packet to one chosen neighbour | deviation: anypath forwarding is out of v0's scope |
| Energy model: the residual energy each node reads | [ZCX21] §7.1 | swarm-core's `IEnergy` port; the transmit and receive costs are the adapter's (ns-3 energy model) | as written: the port; the adapter's model is not implemented |
| Q-routing comparator: Q_x(d, y) is estimated delivery time, updated by ΔQ = η(q + s + t − Q) with t the neighbour's best estimate | [BL93] §2 | comparator only, in swarm-routing/sim-tools; Q-Swarm's update is [ZCX21] eq. 8, not this one (rule 12) | as written: comparator, not Q-Swarm's |

## Parameters

| Parameter | Value | Source | Notes |
|---|---|---|---|
| Discount factor γ | 0.8 | [ZCX21] Table 2 | paper |
| Learning rate α | 0.5 | repo choice: [ZCX21] defines α ∈ (0, 1] (eq. 3) but gives no value; 0.5 until the anchor's sweep | repo choice; Q-routing's 0.5 ([BL93] §2) is noted, not the source |
| Boltzmann temperature T | 0.1 | repo choice: rewards lie in [−3, 0] (eq. 5), so T = 0.1 separates neighbours that differ by a tenth of the range; swept before the anchor | repo choice, [KLM96] §2.2.2 for the rule |
| Initial Q-value | 0 | repo choice: the reward's upper bound (eq. 5), optimistic for a new neighbour | repo choice; unknown in the source |
| \|d\|max (progress normaliser) | 1 hop | repo choice: with current hop counts a neighbour is at most one hop nearer the sink; a stale stored count can differ by more, so d is clamped to ±\|d\|max (row "Progress cost"), which keeps c_d in [0, 1] and the reward ≤ 0 that the initial Q-value relies on | deviation from depth |
| Neighbour entry lifetime | 3 s | repo choice: 3 × the adapter's 1 s beacon interval, a bounded table; no source value | repo choice (`neighbourLifetime`) |
| Neighbour table cap | 64 | repo choice: golden rule 7's cap; no source value | repo choice (`maxNeighbours`) |
| Initial energy e_ini | adapter-configured | unknown | [ZCX21] defines e_ini (eq. 6) but its value is not in Table 2 |
| Transmit / receive power | 2 W / 0.5 W | [ZCX21] Table 2 | paper; acoustic-modem values, the ns-3 scenario's energy model sets its own |
| Maximal transmission range | 150 m | [ZCX21] Table 2 | paper; acoustic, not carried to the WSN scenario |

## Anchor candidate

| Trend | Scenario | Metric | Comparators | Source |
|---|---|---|---|---|
| QLFR's network lifetime is slightly shorter than QELAR's, at least 80.5 % of it, while its delay is lower | 200 to 300 nodes, 1 m/s, normalised as in QELAR | network lifetime (first node to drain), average end-to-end delay | QELAR [HF10] | [ZCX21] Fig. 9; [ZCX21] §7.2 |
| Q-Swarm's own trend for its WSN convergecast with battery depletion (epic #3) | many-to-one sink convergecast, battery drain | network lifetime, PDR, delay | Q-routing [BL93] | unknown |

The second row is the anchor the gates need, and it has no source until #23
names a paper with a terrestrial-WSN trend. The first row is the candidate's
own published trend. It is underwater, so it is recorded here and not
pre-registered.
