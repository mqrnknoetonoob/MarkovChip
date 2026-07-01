// tb_seed_sweep.cpp
//
// Runs all 256 seeds (from RTL/LFSR.v's documented seed list) in parallel
// against a sweep of comparator thresholds, and scores each seed by how
// closely its observed comparator-true count tracks the statistically
// expected count.
//
// --- Why this metric? ---
// Bit_Generator_Circuit uses Comparator(A=LFSR, B=threshold) to decide "hit"
// (x=1) vs "miss" (x=0) each cycle, and counts hits. If the LFSR's output
// were a perfectly uniform random 12-bit value each cycle, the probability
// of a hit (A <= B) would be exactly (B+1)/4096. Over N cycles, the expected
// hit count is:
//
//     expected(B, N) = N * (B+1) / 4096
//
// A "good" seed, for a given run length N, is one whose ACTUAL hit count
// over those N cycles stays close to this expected value across a range of
// thresholds B - i.e. it behaves like a good uniform random source over that
// window, not just over a full 4095-cycle period (see note below).
//
// error(seed, B) = | observed_count(seed, B, N) - expected(B, N) |
//
// Each seed's overall score is the MEAN absolute error across a sweep of
// threshold values, averaged over N. Lower score = better (closer to ideal
// uniform behavior over a realistic operating window).
//
// --- Important caveat ---
// This LFSR is a maximal-length 12-bit LFSR (period 4095: it visits every
// nonzero 12-bit value exactly once before repeating - verified separately).
// That means if N >= 4095, EVERY seed produces the exact same total hit
// count for a given threshold (they just start at different points in the
// identical cycle) - so seed choice becomes statistically meaningless once
// you run a full period. This sweep is only meaningful for N < 4095, i.e.
// testing a PARTIAL window of the sequence, which is the realistic case if
// the real circuit doesn't run for a full 4095-cycle period between resets/
// seed reloads. Adjust --cycles based on your actual operating window.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "VSeed_Sweep_Top.h"
#include "verilated.h"

static vluint64_t g_time = 0;
double sc_time_stamp() { return g_time; }

static const int NUM_SEEDS = 256;

// The same 256 seed values, in the same order used to build the SV localparam
// array (index 0 = seeds[0], ..., index 255 = seeds[FF]), duplicated here in
// C++ purely for labeling output - NOT used to drive the DUT (the DUT already
// has them baked in via seed_list_256.vh).
#include "seed_values_cpp.h"

void clock_tick(VSeed_Sweep_Top* dut) {
    dut->clk = 0; dut->eval(); g_time++;
    dut->clk = 1; dut->eval(); g_time++;
}

void apply_reset(VSeed_Sweep_Top* dut, uint16_t threshold_B) {
    dut->rst_n = 0;
    dut->threshold_B = threshold_B;
    dut->lfsr_en = 0;
    dut->count_en = 0;
    dut->on_switch = 0;
    for (int i = 0; i < 3; i++) clock_tick(dut);
    dut->rst_n = 1;
    clock_tick(dut); // one cycle to settle post-reset before running
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    // ---- Configurable parameters (defaults chosen to stay well inside the
    // 4095-cycle full period, so seed differences remain statistically
    // meaningful; override via CLI if you want a different window). ----
    int cycles = 500;
    std::vector<int> thresholds = {512, 1024, 1536, 2048, 2560, 3072, 3584};
    std::string csv_path = "seed_sweep_results.csv";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--cycles" && i + 1 < argc) {
            cycles = std::atoi(argv[++i]);
        } else if (arg == "--thresholds" && i + 1 < argc) {
            thresholds.clear();
            std::stringstream ss(argv[++i]);
            std::string tok;
            while (std::getline(ss, tok, ',')) thresholds.push_back(std::atoi(tok.c_str()));
        } else if (arg == "--csv" && i + 1 < argc) {
            csv_path = argv[++i];
        }
    }

    if (cycles >= 4095) {
        std::cerr << "WARNING: cycles=" << cycles << " >= LFSR full period (4095). "
                  << "All seeds will score identically since they just sample the "
                  << "same complete cycle starting from different offsets. "
                  << "Use --cycles < 4095 for a meaningful comparison." << std::endl;
    }

    VSeed_Sweep_Top* dut = new VSeed_Sweep_Top;

    // observed[seed][threshold_index]
    std::vector<std::vector<uint32_t>> observed(NUM_SEEDS, std::vector<uint32_t>(thresholds.size(), 0));

    for (size_t ti = 0; ti < thresholds.size(); ti++) {
        uint16_t B = (uint16_t)thresholds[ti];
        apply_reset(dut, B);
        dut->lfsr_en = 1;
        dut->count_en = 1;
        dut->on_switch = 1;
        for (int c = 0; c < cycles; c++) clock_tick(dut);

        for (int s = 0; s < NUM_SEEDS; s++) {
            observed[s][ti] = dut->total_count[s];
        }
        std::cout << "Threshold B=" << B << " done ("
                  << (ti + 1) << "/" << thresholds.size() << ")" << std::endl;
    }

    delete dut;

    // ---- Compute per-seed error metrics ----
    struct SeedResult {
        int seed_idx;
        uint16_t seed_val;
        double mean_abs_error;
        double mean_rel_error_pct;
    };
    std::vector<SeedResult> results(NUM_SEEDS);

    std::ofstream csv(csv_path);
    csv << "seed_index,seed_hex";
    for (size_t ti = 0; ti < thresholds.size(); ti++) {
        csv << ",B=" << thresholds[ti] << "_count,B=" << thresholds[ti] << "_expected,B=" << thresholds[ti] << "_abs_error";
    }
    csv << ",mean_abs_error,mean_rel_error_pct\n";

    for (int s = 0; s < NUM_SEEDS; s++) {
        double sum_abs = 0.0, sum_rel = 0.0;
        csv << s << ",0x" << std::hex << SEED_VALUES[s] << std::dec;
        for (size_t ti = 0; ti < thresholds.size(); ti++) {
            double expected = cycles * (thresholds[ti] + 1) / 4096.0;
            double abs_err = std::fabs((double)observed[s][ti] - expected);
            double rel_err = expected > 0 ? (abs_err / expected) * 100.0 : 0.0;
            sum_abs += abs_err;
            sum_rel += rel_err;
            csv << "," << observed[s][ti] << "," << expected << "," << abs_err;
        }
        double mean_abs = sum_abs / thresholds.size();
        double mean_rel = sum_rel / thresholds.size();
        results[s] = {s, SEED_VALUES[s], mean_abs, mean_rel};
        csv << "," << mean_abs << "," << mean_rel << "\n";
    }
    csv.close();

    std::vector<SeedResult> ranked = results;
    std::sort(ranked.begin(), ranked.end(), [](const SeedResult& a, const SeedResult& b) {
        return a.mean_abs_error < b.mean_abs_error;
    });

    std::cout << "\n========================================" << std::endl;
    std::cout << "Seed sweep complete: " << NUM_SEEDS << " seeds x "
              << thresholds.size() << " thresholds x " << cycles << " cycles" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "\nTOP 10 BEST seeds (lowest mean absolute error):" << std::endl;
    std::cout << "Rank | Seed Index | Seed (hex) | Mean Abs Error | Mean Rel Error %" << std::endl;
    for (int i = 0; i < 10; i++) {
        const auto& r = ranked[i];
        std::cout << " " << (i + 1) << "    | " << r.seed_idx << "\t     | 0x"
                  << std::hex << r.seed_val << std::dec << "\t| "
                  << r.mean_abs_error << "\t\t| " << r.mean_rel_error_pct << "%" << std::endl;
    }

    std::cout << "\nWORST 10 seeds (highest mean absolute error):" << std::endl;
    std::cout << "Rank | Seed Index | Seed (hex) | Mean Abs Error | Mean Rel Error %" << std::endl;
    for (int i = 0; i < 10; i++) {
        const auto& r = ranked[NUM_SEEDS - 1 - i];
        std::cout << " " << (NUM_SEEDS - i) << "  | " << r.seed_idx << "\t     | 0x"
                  << std::hex << r.seed_val << std::dec << "\t| "
                  << r.mean_abs_error << "\t\t| " << r.mean_rel_error_pct << "%" << std::endl;
    }

    std::cout << "\nBest seed overall: index " << ranked[0].seed_idx << " (0x"
              << std::hex << ranked[0].seed_val << std::dec << "), mean abs error = "
              << ranked[0].mean_abs_error << std::endl;
    std::cout << "Full per-seed, per-threshold results written to: " << csv_path << std::endl;

    return 0;
}
