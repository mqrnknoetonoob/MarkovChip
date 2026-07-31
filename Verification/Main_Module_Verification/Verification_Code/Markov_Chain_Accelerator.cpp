// Build:
//   verilator --cc --exe --build -Wall --top-module Markov_Chain_Accelerator \
//     sim_main.cpp Markov_Chain_Accelerator.v LFSR_Enabler.v Stochastic_Timer.v \
//     Row_Col_BGC.v Shift_Register.v LFSR.v Comparator.v priority_encoder.v \
//     counter.v Adder_Tree_16in.v Result_Memory_Shift.v memory_enable_decoder.v \
//     Result_Output_Shifter.v
// Run: ./obj_dir/VMarkov_Chain_Accelerator [matrix_file] [p] [r]
//      (if p/r omitted, prompts interactively -- matches the original C++ style)

#include <verilated.h>
#include "VMarkov_Chain_Accelerator.h"
#include "Golden_Model.hpp"

#include <iostream>
#include <iomanip>
#include <array>
#include <string>
#include <cstdlib>

static VMarkov_Chain_Accelerator *dut;
static vluint64_t sim_time = 0;

// ---------------------------------------------------------------------------
// Clock / basic stepping helpers
// ---------------------------------------------------------------------------
void tick() {
    dut->clk = 0; dut->eval(); sim_time++;
    dut->clk = 1; dut->eval(); sim_time++;
}

// ---------------------------------------------------------------------------
// Group mapping (mirrors the RTL): BGC index -> which spi line drives it.
//   group 0 = BGC 0,1,2,3   (spi_in0..3 respectively)
//   group 1 = BGC 4,5,6,7
//   group 2 = BGC 8,9,10,11
//   group 3 = BGC 12,13,14,15
// The group selector advances by 1 every input_ready pulse, wrapping 0..3.
// Bits are sent MSB-first per 12-bit value (matches the shift-register's
// right-shift-with-MSB-entry convention).
// ---------------------------------------------------------------------------
static int group_turn = 0;

void drive_one_pulse(const std::array<uint16_t,16>& vals, int bit_idx, bool load_row) {
    int base = group_turn * 4;
    dut->spi_in0 = (vals[base + 0] >> bit_idx) & 1;
    dut->spi_in1 = (vals[base + 1] >> bit_idx) & 1;
    dut->spi_in2 = (vals[base + 2] >> bit_idx) & 1;
    dut->spi_in3 = (vals[base + 3] >> bit_idx) & 1;
    dut->input_ready = 1;
    dut->load_row_or_col = load_row ? 1 : 0;
    tick();
    group_turn = (group_turn + 1) % 4;
}

// Loads a full set of 16 12-bit values (a "column" or a "row") -- 48 pulses
// total (12 bits x 4 groups, round-robin). Bits sent LSB(0)->MSB(11), since
// the shift register's q <= {serial_in, q[11:1]} puts the newest bit at the
// MSB each cycle -- so the value's own LSB must arrive first to end up at
// the register's bit 0 after all 12 shifts.
void load_16values(const std::array<uint16_t,16>& vals, bool load_row) {
    group_turn = 0;
    for (int bit_idx = 0; bit_idx <= 11; bit_idx++) {
        for (int g = 0; g < 4; g++) {
            drive_one_pulse(vals, bit_idx, load_row);
        }
    }
    dut->input_ready = 0;
    tick();
}

// After a row load finishes, one compute round takes exactly 4096 cycles.
// This is a fixed, known protocol timing (not something the host has to
// guess) -- but we still watch output_ready throughout as a sanity check.
bool wait_round_done() {
    bool saw_output_ready = false;
    for (int i = 0; i < 4096; i++) {
        tick();
        if (dut->output_ready) saw_output_ready = true;
    }
    return saw_output_ready;
}

// Pulses output_valid and captures the 48-cycle shift-out, reconstructing
// the 16 stored 12-bit results (LSB->MSB, same group order as input).
std::array<uint16_t,16> read_output() {
    std::array<uint16_t,16> result{};
    dut->output_valid = 1;
    tick();  // load tick: all 16 regs parallel-load AND this is already the
             // first valid sample (group0's freshly-loaded LSB, out_en
             // hasn't advanced yet since shift_active was 0 going into
             // this very edge).
    dut->output_valid = 0;

    int turn = 0;
    auto sample = [&]() {
        int base = turn * 4;
        result[base + 0] = (result[base + 0] >> 1) | (dut->spi_out0 << 11);
        result[base + 1] = (result[base + 1] >> 1) | (dut->spi_out1 << 11);
        result[base + 2] = (result[base + 2] >> 1) | (dut->spi_out2 << 11);
        result[base + 3] = (result[base + 3] >> 1) | (dut->spi_out3 << 11);
        turn = (turn + 1) % 4;
    };

    sample(); // sample #0, right after the load tick (group0's first bit)
    for (int c = 1; c < 48; c++) {
        tick();
        sample();
    }
    return result;
}

// ---------------------------------------------------------------------------
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    dut = new VMarkov_Chain_Accelerator;

    // ---- 1. Get matrix file, p (starting row), r (steps) ---------------------
    std::string filename;
    int p, r;
    if (argc >= 4) {
        filename = argv[1];
        p = std::atoi(argv[2]);
        r = std::atoi(argv[3]);
    } else {
        std::cout << "Enter matrix file path: ";
        std::cin >> filename;
        std::cout << "Enter the row index p (0 to 15): ";
        std::cin >> p;
        std::cout << "Enter the number of steps r: ";
        std::cin >> r;
    }
    if (p < 0 || p >= 16) { std::cerr << "Error: p must be 0..15\n"; return 1; }
    if (r < 1)            { std::cerr << "Error: r must be >= 1\n"; return 1; }

    Matrix16 A;
    if (!read_matrix(filename, A)) return 1;

    // ---- 2. Golden model (pure floating point, matches original code) -------
    Vec16 golden = run_golden(A, p, r);

    // ---- 3. Quantize for hardware ---------------------------------------------
    Matrix16 dummy;
    std::array<QVec16, 16> A_q;
    QVec16 B0_q;
    for (int i = 0; i < 16; i++) {
        B0_q[i] = quantize(A[p][i]);   // B0 = row p of A
        for (int j = 0; j < 16; j++)
            A_q[i][j] = quantize(A[i][j]);
    }

    // ---- 4. Reset ---------------------------------------------------------------
    dut->rst_n = 0;
    dut->input_ready = 0;
    dut->load_row_or_col = 0;
    dut->output_valid = 0;
    for (int i = 0; i < 5; i++) tick();
    dut->rst_n = 1;

    // ---- 5. Load initial column = B0 (row p of A), quantized -------------------
    load_16values(B0_q, /*load_row=*/false);

    // ---- 6. Run r steps. Each step = 16 rounds; round j loads column j of A ----
    //         (spread across the 16 lanes) into the "row" registers.
    //         load_row_or_col stays 1 for every row load, including across step
    //         boundaries -- the column (state vector) reloads automatically in
    //         hardware via feedback once output_ready pulses.
    int done_steps = 0;
    for (int step = 0; step < r; step++) {
        for (int round = 0; round < 16; round++) {
            std::array<uint16_t,16> row_j;
            for (int i = 0; i < 16; i++) row_j[i] = A_q[i][round];
            load_16values(row_j, /*load_row=*/true);
            bool oready = wait_round_done();

            if (round == 15) {
                done_steps++;
                std::cout << "[step " << (step + 1) << "/" << r << "] "
                          << "16 rounds complete, output_ready seen=" << oready
                          << ", done_steps=" << done_steps << "\n";
                if (!oready) {
                    std::cerr << "  *** WARNING: output_ready did not pulse "
                                 "when expected! Check timing. ***\n";
                }
            } else if (oready) {
                std::cerr << "  *** WARNING: output_ready pulsed early "
                             "(round " << round << ") -- unexpected! ***\n";
            }
        }
    }

    // ---- 7. done_steps matches required r -> start reading output ------------
    std::array<uint16_t,16> hw_result{};
    if (done_steps == r) {
        std::cout << "\ndone_steps (" << done_steps << ") == required steps (" << r
                  << ") -> starting output shift-out.\n";
        hw_result = read_output();
    } else {
        std::cerr << "done_steps (" << done_steps << ") != required steps (" << r
                  << ") -> NOT reading output.\n";
        dut->final(); delete dut; return 1;
    }

    // ---- 8. Compare hw_result (dequantized) against golden (pure floating) ----
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\nResults after " << r << " step(s), starting from state " << p << ":\n";
    std::cout << std::setw(4) << "idx" << std::setw(12) << "golden"
              << std::setw(12) << "hw(deq)" << std::setw(12) << "hw(raw)"
              << std::setw(14) << "abs_err" << std::setw(12) << "rel_err%\n";

    double max_abs_err = 0.0, max_rel_err = 0.0;
    for (int k = 0; k < 16; k++) {
        double hw_prob = dequantize(hw_result[k]);
        double abs_err = std::abs(hw_prob - golden[k]);
        double rel_err = (golden[k] > 1e-9) ? (abs_err / golden[k] * 100.0) : 0.0;
        max_abs_err = std::max(max_abs_err, abs_err);
        max_rel_err = std::max(max_rel_err, rel_err);
        std::cout << std::setw(4) << k << std::setw(12) << golden[k]
                  << std::setw(12) << hw_prob << std::setw(12) << hw_result[k]
                  << std::setw(14) << abs_err << std::setw(11) << rel_err << "%\n";
    }

    // Tolerance: stochastic computing has inherent sampling noise (~1/sqrt(4096)
    // ~ 1.5% standard deviation per stage, compounding slightly over steps).
    // Tune ABS_TOL based on measured variance across multiple seeds/matrices.
    const double ABS_TOL = 0.03;
    std::cout << "\nmax_abs_err=" << max_abs_err << "  max_rel_err=" << max_rel_err << "%\n";
    std::cout << (max_abs_err < ABS_TOL ? "PASS (within tolerance)\n"
                                         : "*** FAIL (exceeds tolerance) ***\n");

    dut->final();
    delete dut;
    return (max_abs_err < ABS_TOL) ? 0 : 1;
}