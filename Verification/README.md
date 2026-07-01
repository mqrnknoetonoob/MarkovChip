# MarkovChip — Full Seed-Space Verification Report

**Scope:** `RTL/Bit_Generator_Circuit.v`, `RTL/LFSR.v`, `Verification/tb_bit_generator.cpp`

---

## 1. Methodology: exhaustive 4096-seed sweep

`Verification/tb_bit_generator.cpp` sweeps **every possible 12-bit seed (0–4095)**, replacing the earlier 256-entry hard-coded seed list used in `verification by 69/sim/seed_sweep/`. For each seed:

1. The circuit is reset and the seed is loaded via `seed_in` (bit-reversed, consistent with the original serial-shift convention).
2. 32 reference thresholds `B` are drawn (one from each of the 32 equal sub-ranges spanning 0–4095).
3. For each `B`, the Shift Register is parallel-loaded with `B`, the LFSR runs for **4096 cycles**, and `total_count` (the comparator-hit count) is recorded.
4. Three metrics are computed and averaged across the 32 thresholds:
   - **Average Error** — mean of `|total_count − B|`
   - **Absolute Highest Error** — worst-case `|total_count − B|` across the 32 runs
   - **Correlation Score** — mean lag-1…5 autocorrelation magnitude of the per-cycle hit/miss bitstream (lower = less self-correlated / more random-looking)

Results were written to `Verification/Report/seed_ver_report.csv`, covering all 4096 seeds.

---

## 2. Top 32 candidate seeds

Ranked by lowest Average Error, then lowest Correlation Score:

| Rank | Seed (Dec) | Seed (Hex) | Average Error | Absolute Highest Error | Correlation Score |
|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 | 511  | `12'h1FF` | 0.0000 | 0 | 0.1093 |
| 2 | 2303 | `12'h8FF` | 0.0000 | 0 | 0.1093 |
| 3 | 3455 | `12'hD7F` | 0.0000 | 0 | 0.1093 |
| 4 | 1151 | `12'h47F` | 0.0000 | 0 | 0.1094 |
| 5 | 3775 | `12'hEBF` | 0.0000 | 0 | 0.1094 |
| 6 | 2815 | `12'hAFF` | 0.0000 | 0 | 0.1095 |
| 7 | 1023 | `12'h3FF` | 0.0000 | 0 | 0.1096 |
| 8 | 1983 | `12'h7BF` | 0.0000 | 0 | 0.1096 |
| 9 | 3967 | `12'hF7F` | 0.0000 | 0 | 0.1097 |
| 10 | 575  | `12'h23F` | 0.0000 | 0 | 0.1098 |
| 11 | 991  | `12'h3DF` | 0.0000 | 0 | 0.1098 |
| 12 | 1663 | `12'h67F` | 0.0000 | 0 | 0.1098 |
| 13 | 3935 | `12'hF5F` | 0.0000 | 0 | 0.1098 |
| 14 | 1471 | `12'h5BF` | 0.0000 | 0 | 0.1099 |
| 15 | 2783 | `12'hADF` | 0.0000 | 0 | 0.1099 |
| 16 | 2879 | `12'hB3F` | 0.0000 | 0 | 0.1099 |
| 17 | 3263 | `12'hCBF` | 0.0000 | 0 | 0.1099 |
| 18 | 1535 | `12'h5FF` | 0.0000 | 0 | 0.1100 |
| 19 | 1631 | `12'h65F` | 0.0000 | 0 | 0.1100 |
| 20 | 3327 | `12'hCFF` | 0.0000 | 0 | 0.1100 |
| 21 | 63   | `12'h03F` | 0.0000 | 0 | 0.1101 |
| 22 | 127  | `12'h07F` | 0.0000 | 0 | 0.1101 |
| 23 | 479  | `12'h1DF` | 0.0000 | 0 | 0.1101 |
| 24 | 2047 | `12'h7FF` | 0.0000 | 0 | 0.1101 |
| 25 | 2367 | `12'h93F` | 0.0000 | 0 | 0.1101 |
| 26 | 2431 | `12'h97F` | 0.0000 | 0 | 0.1101 |
| 27 | 3839 | `12'hEFF` | 0.0000 | 0 | 0.1101 |
| 28 | 959  | `12'h3BF` | 0.0000 | 0 | 0.1102 |
| 29 | 2271 | `12'h8DF` | 0.0000 | 0 | 0.1102 |
| 30 | 2751 | `12'hABF` | 0.0000 | 0 | 0.1102 |
| 31 | 2943 | `12'hB7F` | 0.0000 | 0 | 0.1102 |
| 32 | 3423 | `12'hD5F` | 0.0000 | 0 | 0.1102 |

These 32 seeds achieved perfect Average/Highest Error scores across all 32 threshold tests, with the lowest Correlation Scores among that group.
