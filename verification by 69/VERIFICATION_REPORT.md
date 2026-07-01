# Verification Report: Comparator.v & Bit_Generator_Circuit.v

Tooling: Verilator 5.020, C++ testbenches with self-checking reference models.
All results below were obtained by actually building and running these tests
(not predicted) — see "How to reproduce" at the bottom.

---

## 1. Comparator.v — Exhaustive Verification

**Method:** `sim/tb_comparator.cpp` drives *every* possible (A, B) pair —
4096 × 4096 = **16,777,216 combinations**, the full input space of a 12-bit
comparator — and checks the output against a one-line C++ golden model.

**Result: 0 mismatches out of 16,777,216 vectors. The comparator's core logic is functionally correct.**

**⚠️ Important polarity finding:** The actual output convention is:
- `x = 1` when `A <= B`
- `x = 0` when `A > B`

This is the **opposite** of what two things in this codebase claim:
1. The comment directly above the `assign x = ...` line in `Comparator.v`
   ("Output 1 if A > B, else 0")
2. The original `Verification/sim_comparator.cpp` testbench's expected values
   (which is why every one of its 6 original test cases reports a "mismatch"
   when actually run — confirmed by running it)

However, it **matches** the comment in `Bit_Generator_Circuit.v`
("If LFSR > Register, x = 0; otherwise, x = 1"), and the counter is wired
to count on `x=1`. So functionally, the whole design is internally
self-consistent — it's just that the in-module comment and the old
testbench's expected values are wrong, not the logic itself.

**Recommendation:** Fix the misleading comment in `Comparator.v`, and either
delete or correct `sim_comparator.cpp` (superseded by `tb_comparator.cpp`,
which exhaustively covers what it was attempting to spot-check). If the
*intended* spec was actually "x=1 means A>B" (i.e. the RTL is the thing
that's wrong, not the comments), let me know — the fix is a one-line flip
of the ternary in `Comparator.v` and I can validate it just as exhaustively.

---

## 2. Bit_Generator_Circuit.v — Integration Verification

**Method:** `sim/tb_bit_generator.cpp` builds a bit-accurate C++ reference
model of all five submodules (Seed_Memory, LFSR, Shift_Register, Comparator,
counter) and cross-checks `total_count` against it after every clock edge,
across 719 cycles covering: reset, seed shifting, parallel load, hold mode,
free-running LFSR/comparator/counter operation, `on_switch` deassertion, and
shift-right mode.

**Result: 719/719 cycles matched the reference model exactly.**

### 🔴 Bug found: LFSR is never seeded from Seed_Memory

Verilator's own lint (`-Wall`) flags this independently:
```
%Warning-UNUSEDSIGNAL: RTL/Bit_Generator_Circuit.v:17:17: Signal is not used: 'seed'
```
In `Bit_Generator_Circuit.v`, the `seed` wire is driven by `Seed_Memory`'s
output `q`, but it is **never connected to anything** — the `LFSR` instance
is hardcoded with `.SEED(12'd1)` as a compile-time parameter instead of
using `seed` as a runtime input. Practically, this means:
- Serially shifting a custom seed into `Seed_Memory` via `shift_en`/`data_in`
  has **zero effect** on the LFSR's actual sequence.
- The LFSR always starts from `12'd1` on every reset, regardless of what's
  been shifted in.

This was confirmed behaviorally in Test 2 of the testbench (shifted in
`0xABC`, verified via the reference model — and via the DUT's matching
`total_count` in later tests — that the LFSR sequence is unaffected).

**This is either:**
(a) A genuine wiring bug — if `Seed_Memory` is meant to seed the LFSR at
    runtime, `LFSR` needs a `seed_in` port and mux logic to load it (LFSR
    parameters are elaboration-time constants in Verilog, so a parameter
    can't be driven by a runtime signal — this needs an RTL restructure,
    not just a rewire), **or**
(b) Dead code — if `Seed_Memory` is a leftover / unused block, it (and its
    port list on the top-level) should be removed.

Either way, worth a decision from your team before tape-out, since GF180MCU
area optimization was called out in the README and this module currently
consumes silicon for no functional benefit.

### Style note (not a functional bug)
`Shift_Register.v` uses an **active-high, edge-triggered** reset
(`posedge reset`, with `reset` tied to `~rst_n` at the top level), while
every other sequential module (`Seed_Memory`, `LFSR`, `counter`) uses a
**standard active-low async reset** (`negedge rst_n`). Testing confirms
this is currently functionally equivalent given how it's wired, but it's an
inconsistent reset style across the design that could bite you if reset
domains or synthesis tools ever treat them differently. Consider
standardizing on `negedge rst_n` for consistency.

---

## 3. 256-Seed Sweep — Which Seed Gives the "Best" Statistical Output

**Motivation:** `Bit_Generator_Circuit` samples an LFSR (A) against a
threshold register (B) every cycle and counts "hits" (A ≤ B). This is a
stochastic-computing technique for estimating a probability — over enough
uniformly-random samples, `hit_count / N` should converge to `(B+1)/4096`.
The 256 seeds documented in `RTL/LFSR.v` are candidate starting points for
that LFSR. "Best" seed = the one whose sample statistics track the ideal
uniform-random expectation most closely.

### Key fact established first
The LFSR's feedback polynomial produces a **maximal-length sequence**:
period = 4095 cycles, visiting every nonzero 12-bit value exactly once
before repeating (verified directly). This means **if you run for a full
4095-cycle period, every seed produces an identical result** for a given
threshold — they're just different starting offsets into the same cycle.
**Seed choice is only statistically meaningful for partial-period runs**
(N < 4095), i.e. the realistic case where the real circuit samples for a
limited window rather than a full period between resets.

### Method
`sim/seed_sweep/Seed_Sweep_Top.v` instantiates all 256 `LFSR + Comparator +
counter` pipelines in parallel (reusing your actual verified RTL modules
unmodified), sharing one clock/reset/threshold. `sim/seed_sweep/tb_seed_sweep.cpp`:

1. Sweeps 7 threshold values: B = 512, 1024, 1536, 2048, 2560, 3072, 3584
2. For each threshold, resets and runs **500 cycles** (well under the 4095
   full period, so results are seed-dependent)
3. For each seed, computes: `error(seed, B) = |observed_count - N*(B+1)/4096|`
4. Averages that error across all 7 thresholds → each seed's overall score
   (lower = better / more uniform over this window)

Both `--cycles` and `--thresholds` are CLI-configurable if you want a
different window size or threshold set — see the file header comment for
the full rationale and how to change the metric.

### Results (500-cycle window, 7-threshold sweep)

| Rank | Seed Index | Seed (hex) | Mean Abs Error | Mean Rel Error |
|---|---|---|---|---|
| 1 (best) | 88 | 0x51F | 0.91 | 0.63% |
| 2 | 171 | 0x387 | 0.94 | 0.71% |
| 3 | 204 | 0x06E | 1.09 | 0.53% |
| ... | | | | |
| 255 | 176 | 0x066 | 31.59 | 23.14% |
| 256 (worst) | 96 | 0x68D | 32.74 | 22.73% |

**Best seed for a 500-cycle operating window: index 88, value `0x51F`.**

Full per-seed, per-threshold data (all 256 rows) is written to
`seed_sweep_results.csv` on every run — open it in Excel/Sheets or plot it
directly for your report.

### Caveats to flag to your friend
- This error metric assumes the "ground truth" is a perfectly uniform
  random source, which an LFSR only approximates. It's a reasonable
  baseline, but if your team has a different definition of "best" (e.g.
  weighted toward one specific threshold instead of all 7, or a different
  cycle count matching actual planned hardware usage), the metric is
  isolated in one place in `tb_seed_sweep.cpp` (see the `expected` formula)
  and easy to swap.
- Results depend on `--cycles`. Rerun with a value matching your actual
  intended sampling window per Markov-chain decision — 500 was chosen as a
  reasonable default, not a spec requirement (I couldn't find a specified
  sample-window length in the RTL/README, so let me know if there's one).



```bash
# 1. Install Verilator (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y verilator make g++

# 2. From the project root (containing RTL/ and sim/):
make all          # builds and runs everything below
make comparator   # exhaustive Comparator check (16.7M vectors)
make bitgen       # integration check (writes waveform_bitgen.vcd)
make seed_sweep   # 256-seed sweep (writes seed_sweep_results.csv)
make waves        # opens the top-level waveform in GTKWave (must be installed)
make clean        # remove build artifacts

# To customize the seed sweep (different window size / thresholds):
./obj_dir_sweep/VSeed_Sweep_Top --cycles 1000 --thresholds 1024,2048,3072 --csv my_results.csv
```

Expected output: `TEST PASSED` for the comparator (16,777,216/16,777,216
vectors), `ALL CHECKS PASSED` for the integration test (719 cycles), and a
ranked seed list + CSV for the seed sweep.
