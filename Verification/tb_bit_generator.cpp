#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <numeric>
#include <sys/stat.h>
#include "VBit_Generator_Circuit.h"
#include "verilated.h"

void tick_clock(VBit_Generator_Circuit* top) {
    top->clk = 1;
    top->eval();
    top->clk = 0;
    top->eval();
}

// Replicates old Seed_Memory serial shift behavior (LSB first → bit reversal)
uint16_t bit_reverse_12(int val) {
    uint16_t result = 0;
    for (int i = 0; i < 12; ++i)
        result = (result << 1) | ((val >> i) & 1);
    return result;
}

// Auto-correlation of a bitstream across lags 1 to max_lag
double calculate_autocorrelation(const std::vector<int>& bits, int max_lag) {
    int N = bits.size();
    if (N == 0) return 1.0;

    double sum = std::accumulate(bits.begin(), bits.end(), 0.0);
    double mean = sum / N;

    double var = 0.0;
    for (int b : bits)
        var += (b - mean) * (b - mean);
    var /= N;

    if (var < 1e-9) return 1.0;

    double total_corr = 0.0;
    int valid_lags = 0;

    for (int lag = 1; lag <= max_lag; ++lag) {
        double cov = 0.0;
        for (int t = 0; t < N - lag; ++t)
            cov += (bits[t] - mean) * (bits[t + lag] - mean);
        cov /= (N - lag);
        total_corr += std::abs(cov / var);
        valid_lags++;
    }

    return total_corr / valid_lags;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    VBit_Generator_Circuit* top = new VBit_Generator_Circuit;

    std::srand(std::time(0));

    // Create report directory if not exists
    mkdir("report", 0777);

    // 32 reference values from 32 ranges (0-127, 128-255, ..., 3968-4095)
    std::vector<uint16_t> B_refs(32);
    for (int r = 0; r < 32; ++r)
        B_refs[r] = (r * 128) + (std::rand() % 128);

    std::ofstream csv_file("report/seed_ver_report.csv");
    if (!csv_file.is_open()) {
        std::cerr << "Error: Could not open output CSV file!" << std::endl;
        return -1;
    }
    csv_file << "Seed(Dec),Seed(Hex),Average Error,Absolute Highest Error,Correlation Score\n";

    std::cout << "Simulating all 4096 seeds..." << std::endl;

    for (int seed_val = 0; seed_val < 4096; ++seed_val) {
        // Replicate old Seed_Memory serial-shift behavior via bit-reversal
        // Old code: shifted seed_val LSB-first into Seed_Memory (shift left)
        // which is equivalent to bit-reversing seed_val before loading
        uint16_t actual_seed = bit_reverse_12(seed_val);

        double total_error       = 0.0;
        double total_correlation = 0.0;
        int    max_error         = 0;

        for (int r = 0; r < 32; ++r) {
            // --- Reset circuit and load seed ---
            top->rst_n     = 0;
            top->seed_in   = actual_seed;
            top->lfsr_en   = 0;
            top->count_en  = 0;
            top->on_switch = 0;
            top->mode      = 0;
            top->par_in    = 0;
            top->eval();
            tick_clock(top);     // LFSR loads actual_seed on reset
            top->rst_n = 1;
            top->eval();

            // --- Parallel Load value B into Shift Register ---
            top->par_in = B_refs[r];
            top->mode   = 2;     // 2'b10 = Parallel Load
            tick_clock(top);     // lfsr_en=0, LFSR still holds actual_seed
            top->mode   = 0;     // 2'b00 = Hold mode

            // --- Run LFSR and Count for 4096 cycles ---
            top->lfsr_en   = 1;
            top->count_en  = 1;
            top->on_switch = 1;

            std::vector<int> bitstream(4096);
            uint16_t prev_count = 0;

            for (int cycle = 0; cycle < 4096; ++cycle) {
                tick_clock(top);
                uint16_t curr = top->total_count;
                // If count increased, comparator output was 1 this cycle
                bitstream[cycle] = (curr > prev_count) ? 1 : 0;
                prev_count = curr;
            }

            uint16_t hw_count = top->total_count;
            int error = std::abs(hw_count - B_refs[r]);

            total_error       += error;
            total_correlation += calculate_autocorrelation(bitstream, 5);
            if (error > max_error)
                max_error = error;
        }

        double avg_error = total_error       / 32.0;
        double avg_corr  = total_correlation / 32.0;

        // Write to CSV (label with seed_val for readability, actual_seed is the LFSR state)
        csv_file << seed_val << ",12'h"
                 << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << seed_val << ","
                 << std::dec << std::fixed << std::setprecision(4)
                 << avg_error << "," << max_error << "," << avg_corr << "\n";
    }

    csv_file.close();
    delete top;
    std::cout << "RTL Verification completed! Report saved to 'report/seed_ver_report.csv'" << std::endl;
    return 0;
}