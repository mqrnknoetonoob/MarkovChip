// ============================================================================
// GATE-LEVEL verification harness for Markov_Chain_Accelerator.
//
// This is the SAME driver/checker logic as the RTL testbench
// (Markov_Chain_Accelerator.cpp) -- unchanged, because the gate-level
// netlist's top module has the identical port list/order:
//   clk, rst_n, spi_in0-3, input_ready, load_row_or_col,
//   output_valid, output_ready, spi_out0-3
//
// The ONLY thing that changes is what we hand to Verilator at build time:
// the gate-level netlist + the PDK standard-cell functional models,
// instead of the individual RTL .v files.
//
// Build (fill in the PDK functional-model path):
//   verilator --cc --exe --build -Wall --top-module Markov_Chain_Accelerator \
//     sim_main_gatelevel.cpp \
//     Markov_Chain_Accelerator_Netlist.v \
//     <path-to-pdk>/gf180mcu_fd_sc_mcu7t5v0.v      \  <-- functional models, NOT liberty
//     -Wno-fatal
//
// Run: ./obj_dir/VMarkov_Chain_Accelerator [matrix_file] [p] [r]
// ============================================================================

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

bool wait_round_done() {
    bool saw_output_ready = false;
    for (int i = 0; i < 4096; i++) {
        tick();
        if (dut->output_ready) saw_output_ready = true;
    }
    return saw_output_ready;
}

std::array<uint16_t,16> read_output() {
    std::array<uint16_t,16> result{};
    dut->output_valid = 1;
    tick();
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

    sample();
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

    Vec16 golden = run_golden(A, p, r);

    std::array<QVec16, 16> A_q;
    QVec16 B0_q;
    for (int i = 0; i < 16; i++) {
        B0_q[i] = quantize(A[p][i]);
        for (int j = 0; j < 16; j++)
            A_q[i][j] = quantize(A[i][j]);
    }

    dut->rst_n = 0;
    dut->input_ready = 0;
    dut->load_row_or_col = 0;
    dut->output_valid = 0;
    for (int i = 0; i < 5; i++) tick();
    dut->rst_n = 1;

    load_16values(B0_q, /*load_row=*/false);

    int done_steps = 0;
    for (int step = 0; step < r; step++) {
        for (int round = 0; round < 16; round++) {
            std::array<uint16_t,16> row_j;
            for (int i = 0; i < 16; i++) row_j[i] = A_q[i][round];
            load_16values(row_j, /*load_row=*/true);
            bool oready = wait_round_done();

            if (round == 15) {
                done_steps++;
                std::cout << "[GATE-LEVEL step " << (step + 1) << "/" << r << "] "
                          << "16 rounds complete, output_ready seen=" << oready
                          << ", done_steps=" << done_steps << "\n";
                if (!oready) {
                    std::cerr << "  *** WARNING: output_ready did not pulse "
                                 "when expected! Check timing (dff reset polarity?). ***\n";
                }
            } else if (oready) {
                std::cerr << "  *** WARNING: output_ready pulsed early "
                             "(round " << round << ") -- unexpected! ***\n";
            }
        }
    }

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

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\n[GATE-LEVEL] Results after " << r << " step(s), starting from state " << p << ":\n";
    std::cout << std::setw(4) << "idx" << std::setw(12) << "golden"
              << std::setw(12) << "hw(deq)" << std::setw(12) << "hw(raw)"
              << std::setw(14) << "abs_err" << std::setw(12) << "rel_err%\n";

    double max_abs_err = 0.0, max_rel_err = 0.0;
    int overflow_flags = 0;
    for (int k = 0; k < 16; k++) {
        double hw_prob = dequantize(hw_result[k]);
        double abs_err = std::abs(hw_prob - golden[k]);
        double rel_err = (golden[k] > 1e-9) ? (abs_err / golden[k] * 100.0) : 0.0;
        max_abs_err = std::max(max_abs_err, abs_err);
        max_rel_err = std::max(max_rel_err, rel_err);
        // hw_result is only ever populated from a 12-bit shift-out, so it
        // can never itself signal "> 4095" -- this raw value is exactly
        // what the 12-bit-truncated adder tree produced. A silently wrapped
        // (overflowed) sum will just look like an unusually large abs_err
        // here rather than an explicit flag; that's why the abs_err column
        // is the thing to watch closely, especially vs the RTL-sim numbers
        // for the SAME matrix file.
        std::cout << std::setw(4) << k << std::setw(12) << golden[k]
                  << std::setw(12) << hw_prob << std::setw(12) << hw_result[k]
                  << std::setw(14) << abs_err << std::setw(11) << rel_err << "%\n";
    }

    const double ABS_TOL = 0.03;
    std::cout << "\nmax_abs_err=" << max_abs_err << "  max_rel_err=" << max_rel_err << "%\n";
    std::cout << (max_abs_err < ABS_TOL ? "PASS (within tolerance)\n"
                                         : "*** FAIL (exceeds tolerance) ***\n");

    dut->final();
    delete dut;
    return (max_abs_err < ABS_TOL) ? 0 : 1;
}
