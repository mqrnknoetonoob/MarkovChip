# Verification Report: Top-Level Integration & Submodules

**Tooling:** Verilator 5.x (WSL/Ubuntu), C++ testbenches with self-checking reference models.
**Environment:** Windows/VS Code via WSL Subsystem.

All results below were obtained by actually building and running these tests (not predicted) — see "How to reproduce" at the bottom.

---

## 1. Combinatorial Submodule Verification

### 1.1 Comparator.v
**Method:** `sim/tb_comparator.cpp` drives *every* possible (A, B) pair — 4096 × 4096 = **16,777,216 combinations**, the full input space of a 12-bit comparator — and checks the output against a one-line C++ golden model.

**Result: 0 mismatches out of 16,777,216 vectors. The comparator's core logic is functionally correct.**

**⚠️ Important polarity finding:** The actual output convention is:
- `x = 1` when `A <= B`
- `x = 0` when `A > B`

This is the **opposite** of what two things in this codebase claim:
1. The comment directly above the `assign x = ...` line in `Comparator.v` ("Output 1 if A > B, else 0")
2. The original `Verification/sim_comparator.cpp` testbench's expected values

However, it **matches** the comment in `Bit_Generator_Circuit.v` ("If LFSR > Register, x = 0; otherwise, x = 1"), and the counter is wired to count on `x=1`. Functionally, the design is internally self-consistent; the in-module comment and the old testbench's expected values are wrong, not the logic itself.

### 1.2 priority_encoder.v
**Method:** `sim/tb_priority_encoder.cpp` drives *every* possible 12-bit input state (4096 total combinations) and checks the output against a C++ golden model tracking the MSB-priority specification.

**Result: 0 mismatches out of 4096 vectors. The combinatorial logic is functionally correct.**
* The internal casting mechanism (`i[3:0] + 4'd1`) successfully prevents width-mismatch warnings during compilation. 
* The default output state correctly defaults to `4'd0` when no bits are set.

---

## 2. Sequential Submodule Verification

### 2.1 LFSR.v
**Method:** `sim/tb_LFSR.cpp` verifies the structural behavior of the Linear Feedback Shift Register against a C++ calculation of the tap positions (Bits 11, 6, 3, 2). The simulation verified async reset, hold states (`en = 0`), and evaluated a full 4,095-cycle active shift sequence (`en = 1`).

**Result: All simulated cycles matched the reference model perfectly.**
* **Maximal Length Sequence:** The feedback polynomial successfully generates a maximal-length pseudo-random sequence, repeating exactly every 4095 cycles.
* **Seeding Limitation:** This module contains no physical input ports for a dynamic seed. It only accepts the `SEED` parameter at elaboration/compile-time, meaning it can only be reset to its static initial state, not dynamically seeded at runtime.

### 2.2 counter.v
**Method:** `sim/tb_counter.cpp` evaluates the 12-bit counter across all control flow branches. The testbench verified: asynchronous active-low reset, synchronous master switch disable, count enable state-holding, and a full 4096-step exhaustive rollover sweep.

**Result: 0 mismatches. The counter correctly prioritizes control signals and handles natural 12-bit integer rollover.**

### 2.3 Shift_Register.v
**Method:** `sim/tb_Shift_Register.cpp` evaluates the Universal Shift Register across all operational multiplexer states determined by the `mode` selection lines. The testbench verified Hold (`2'b00`), Shift Right (`2'b01`), Parallel Load (`2'b10`), and Reset.

**Result: 100% operational match against the C++ structural model.**
* **Style Note:** This module utilizes an **active-high** reset (`posedge reset`), whereas every other sequential module (`Seed_Memory`, `LFSR`, `counter`) uses a **standard active-low async reset** (`negedge rst_n`). Testing confirms this is functionally equivalent given top-level wiring, but consider standardizing for synthesis consistency.

---

## 3. Integration Verification

### 3.1 Bit_Generator_Circuit.v
**Method:** `sim/tb_bit_generator.cpp` builds a bit-accurate C++ reference model of all five submodules and cross-checks `total_count` against it after every clock edge, across 719 cycles covering: reset, seed shifting, parallel load, hold mode, free-running operation, and mode switching.

**Result: 719/719 cycles matched the reference model exactly.**

**🔴 Bug found: LFSR is never seeded from Seed_Memory**
In `Bit_Generator_Circuit.v`, the `seed` wire is driven by `Seed_Memory`'s output `q`, but it is **never connected to anything** — the `LFSR` instance is hardcoded with `.SEED(12'd1)` as a compile-time parameter instead of using `seed` as a runtime input. 
* Serially shifting a custom seed into `Seed_Memory` via `shift_en`/`data_in` has **zero effect** on the LFSR's actual sequence.
* The LFSR always starts from `12'd1` on every reset, regardless of what's been shifted in.

This is either a genuine wiring bug requiring RTL restructure (adding a `seed_in` port to the LFSR) or dead code (meaning `Seed_Memory` should be removed to save silicon area).

---

## 4. 256-Seed Sweep — Statistical Output Analysis

**Motivation:** `Bit_Generator_Circuit` samples an LFSR (A) against a threshold register (B) every cycle and counts "hits" (A ≤ B). The 256 seeds documented in `RTL/LFSR.v` are candidate starting points. 

**Method:** `sim/seed_sweep/Seed_Sweep_Top.v` instantiates all 256 pipelines in parallel. It sweeps 7 threshold values, resets and runs 500 cycles per threshold, and calculates the mean absolute/relative error against an ideal uniform-random expectation.

**Results (500-cycle window, 7-threshold sweep):**
| Rank | Seed Index | Seed (hex) | Mean Abs Error | Mean Rel Error |
|---|---|---|---|---|
| 1 (best) | 88 | 0x51F | 0.91 | 0.63% |
| 2 | 171 | 0x387 | 0.94 | 0.71% |
| 3 | 204 | 0x06E | 1.09 | 0.53% |
| ... | | | | |
| 255 | 176 | 0x066 | 31.59 | 23.14% |
| 256 (worst) | 96 | 0x68D | 32.74 | 22.73% |

**Best seed for a 500-cycle operating window: index 88, value `0x51F`.**

---

## 5. How to Reproduce

Execute the following commands from the project root in the terminal to compile and run the testbenches.

```bash
# 1. Integration, Comparator, & Seed Sweep checks (via Makefile)
make all          # builds and runs everything below
make comparator   # exhaustive Comparator check (16.7M vectors)
make bitgen       # integration check (writes waveform_bitgen.vcd)
make seed_sweep   # 256-seed sweep (writes seed_sweep_results.csv)
make waves        # opens the top-level waveform in GTKWave (must be installed)

# 2. Standalone Submodule Checks
verilator -Wall -Wno-EOFNEWLINE --cc RTL/priority_encoder.v --exe sim/tb_priority_encoder.cpp --build -Mdir obj_dir_pe
./obj_dir_pe/Vpriority_encoder

verilator -Wall -Wno-EOFNEWLINE -Wno-WIDTHTRUNC -GSEED=1 --cc RTL/LFSR.v --exe sim/tb_LFSR.cpp --build -Mdir obj_dir_lfsr
./obj_dir_lfsr/VLFSR

verilator -Wall -Wno-EOFNEWLINE --cc RTL/counter.v --exe sim/tb_counter.cpp --build -Mdir obj_dir_counter
./obj_dir_counter/Vcounter

verilator -Wall -Wno-EOFNEWLINE --cc RTL/Shift_Register.v --exe sim/tb_Shift_Register.cpp --build -Mdir obj_dir_shift_reg
./obj_dir_shift_reg/VShift_Register

# To clean the build directory
make clean
rm -rf obj_dir_pe obj_dir_lfsr obj_dir_counter obj_dir_shift_reg