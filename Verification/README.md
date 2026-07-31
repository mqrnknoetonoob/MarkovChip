# MarkovChip — Full Seed-Space Verification Report

**Scope:** `RTL/LFSR.v` (whitening/scrambling layer), `RTL/Row_Col_BGC.v`,
`/Verification/Main_Module_Verification/Golden_Model_Test_Stochastic.cpp` (seed search),
`/Verification/Main_Module_Verification/Verification_Code/Markov_Chain_Accelerator.cpp` (RTL-level golden-model harness)

---

## 0. Why this changed from the original report

The original seed-selection pass (32 seeds, `Correlation Score` ≈ 0.109–0.110)
evaluated the **raw LFSR output** (`seed_mem`) directly. That raw sequence
reloads to the exact same fixed seed at the start of every round (the LFSR's
`en` input is tied to `lfsr_en`, which drops between every round), so the
same deterministic bit-sequence repeats every time. Though it showed a correlation
error of 10% Comparing row-LFSR and col-LFSR streams built this way introduced a **systematic, repeatable bias**
between the two — not independent noise — which compounded across multi-step
Markov iterations (verified: max abs error grew from ~0.005 at 1 step to
~0.033 at 10 steps with the old seeds/no whitening, eventually exceeding the
error tolerance).

A whitening/scrambling layer was added to `LFSR.v` (XOR-shift mixing +
zero-cost fixed bit-permutation), applied to the raw state before it reaches
the comparator (`scrambled_out`, not `seed_mem`). The seed search below was
re-run **against the whitened output**, and the resulting seeds were
re-validated directly against the full RTL (not just the standalone LFSR
model) using a multi-step, feedback-loop-aware golden-model comparison.

---

## 1. Methodology: seed search over the whitened LFSR output

`Golden_Model_Test_Stochastic.cpp` sweeps 4096 candidate seeds (bit-reversed
0–4095, matching the original serial-shift convention). For each seed:

1. The raw LFSR sequence is generated for 4096 cycles (same feedback
   polynomial: `x^12+x^7+x^4+x^3+1`).
2. Each raw value is passed through `whiten_lfsr_value()` — an XOR-shift
   (`raw ^ (raw>>3) ^ (raw<<5)`) followed by the fixed bit-permutation
   `Y_i = X_((5*i+3) mod 12)` — mirroring `LFSR.v`'s `scrambled_out` exactly.
3. 32 reference thresholds `B` are drawn (one per 128-wide sub-range of
   0–4095, randomized within each sub-range).
4. For each `B`: count how many of the 4096 whitened samples are `<= B`,
   compare against the ideal count (`B`), and record the lag-1…5
   autocorrelation of the resulting hit/miss bitstream.
5. Average Error, Absolute Highest Error, and Correlation Score are averaged
   across the 32 thresholds per seed, then all 4096 seeds are ranked
   (primary: lowest Average Error; secondary: lowest Correlation Score).

## 2. Top 32 candidate seeds (post-whitening)

| Rank | Seed (Dec) | Seed (Hex) | Average Error | Absolute Highest Error | Correlation Score |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 | 2141 | `12'h85D` | 0.0000 | 0 | 0.0171 |
| 2 | 3638 | `12'hE36` | 0.0000 | 0 | 0.0173 |
| 3 | 3196 | `12'hC7C` | 0.0000 | 0 | 0.0173 |
| 4 | 341  | `12'h155` | 0.0000 | 0 | 0.0173 |
| 5 | 1365 | `12'h555` | 0.0000 | 0 | 0.0173 |
| 6 | 372  | `12'h174` | 0.0000 | 0 | 0.0174 |
| 7 | 1396 | `12'h574` | 0.0000 | 0 | 0.0174 |
| 8 | 2172 | `12'h87C` | 0.0000 | 0 | 0.0174 |
| 9 | 3607 | `12'hE17` | 0.0000 | 0 | 0.0174 |
| 10 | 3197 | `12'hC7D` | 0.0000 | 0 | 0.0174 |
| 11 | 3639 | `12'hE37` | 0.0000 | 0 | 0.0174 |
| 12 | 2614 | `12'hA36` | 0.0000 | 0 | 0.0174 |
| 13 | 2583 | `12'hA17` | 0.0000 | 0 | 0.0174 |
| 14 | 830  | `12'h33E` | 0.0000 | 0 | 0.0174 |
| 15 | 3165 | `12'hC5D` | 0.0000 | 0 | 0.0174 |
| 16 | 1823 | `12'h71F` | 0.0000 | 0 | 0.0174 |
| 17 | 1855 | `12'h73F` | 0.0000 | 0 | 0.0174 |
| 18 | 1854 | `12'h73E` | 0.0000 | 0 | 0.0174 |
| 19 | 799  | `12'h31F` | 0.0000 | 0 | 0.0174 |
| 20 | 2578 | `12'hA12` | 0.0312 | 1 | 0.0174 |
| 21 | 2296 | `12'h8F8` | 0.0312 | 1 | 0.0174 |
| 22 | 2792 | `12'hAE8` | 0.0312 | 1 | 0.0174 |
| 23 | 3292 | `12'hCDC` | 0.0312 | 1 | 0.0174 |
| 24 | 954  | `12'h3BA` | 0.0312 | 1 | 0.0174 |
| 25 | 1950 | `12'h79E` | 0.0312 | 1 | 0.0175 |
| 26 | 959  | `12'h3BF` | 0.0312 | 1 | 0.0175 |
| 27 | 794  | `12'h31A` | 0.0312 | 1 | 0.0175 |
| 28 | 2738 | `12'hAB2` | 0.0312 | 1 | 0.0175 |
| 29 | 2743 | `12'hAB7` | 0.0312 | 1 | 0.0175 |
| 30 | 1947 | `12'h79B` | 0.0312 | 1 | 0.0175 |
| 31 | 2710 | `12'hA96` | 0.0312 | 1 | 0.0175 |
| 32 | 827  | `12'h33B` | 0.0312 | 1 | 0.0175 |

Correlation Score dropped roughly **6x** compared to the pre-whitening report
(0.109–0.110 → 0.017–0.018) for a comparably-ranked top-32 set — a direct,
quantitative confirmation that the whitening layer meaningfully decorrelates
the LFSR output.

## 3. Row/column assignment

The 32 seeds were split alternately by rank (rank 1 -> row, rank 2 -> col,
rank 3 -> row, ...) so both the row-LFSR and column-LFSR groups get an even
mix of the highest-ranked seeds, rather than one group getting only the
top-19 (Average Error = 0) and the other only the next tier:

```
ROW: 85D, C7C, 555, 574, E17, E37, A17, C5D, 73F, 31F, 8F8, CDC, 79E, 31A, AB7, A96
COL: E36, 155, 174, 87C, C7D, A36, 33E, 71F, 73E, A12, AE8, 3BA, 3BF, AB2, 79B, 33B
```

No seed is reused between the row and column sets (checked programmatically),
so no lane's row-LFSR and col-LFSR share a starting seed.

## 4. RTL-level validation (multi-step, feedback loop included)

The standalone seed-search metric (Average Error / Correlation Score) only
evaluates a single LFSR in isolation. Since the seeds ultimately feed a
16-lane, 16-round, multi-step accelerator with a feedback loop
(`stored_results_flat -> prev_step_output` each step), the candidate seed
set was additionally validated by running it through the **full RTL** (via
`verification/sim_main.cpp`, a Verilator harness that drives the real
input/output protocol and compares against a floating-point golden model:
`B0 = row p of A`, `B = B0 * A^r`), across increasing step counts:

| Seed set | r=1 | r=5 | r=10 | r=20 |
|---|---|---|---|---|
| Original hand-picked (pre-whitening) | 0.0048 | 0.0094 | 0.0121 | — |
| Naive random distinct seeds | 0.0054 | 0.0208 | 0.0438 (**exceeds tolerance**) | — |
| **This report's seeds (post-whitening search)** | 0.0083 | 0.0104 | 0.0106 | **0.0106** |

(All values are max absolute error across the 16 output states, tolerance
threshold 0.03.)

This report's seeds show a slightly higher single-step error than the
original hand-picked set, but the error stays essentially **flat** from
r=10 to r=20 rather than continuing to grow — the naive-random set fails
outright by r=10. Since the accelerator is used for multi-step iteration,
long-run stability was weighted over single-step accuracy, and this seed set
was adopted as final.

## 5. Files

- `RTL/LFSR.v` — whitening layer + these seeds' consumers (`scrambled_out`)
- `RTL/Markov_Chain_Accelerator.v` — `seeds_row_flat` / `seeds_col_flat`
  localparams updated to the row/col split above
- `Verification/Golden_Model_Test_Stochastic.cpp` — seed search source
