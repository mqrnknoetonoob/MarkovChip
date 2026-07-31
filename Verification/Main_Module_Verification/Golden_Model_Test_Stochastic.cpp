#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <cstdint>

const int N_SEEDS = 4096;
const int N_CYCLES = 4096;
const int N_THRESH = 32;

// 12-bit bit-reversal function
std::vector<uint16_t> bit_reverse_12_arr(const std::vector<uint16_t>& vals) {
    std::vector<uint16_t> result(vals.size(), 0);
    for (size_t idx = 0; idx < vals.size(); ++idx) {
        uint16_t val = vals[idx];
        uint16_t rev = 0;
        for (int i = 0; i < 12; ++i) {
            uint16_t bit = (val >> i) & 1;
            rev = (rev << 1) | bit;
        }
        result[idx] = rev;
    }
    return result;
}

// NEW FUNCTION: Implements the mathematical hardware whitening layer
// Combines an XOR-Shift with a non-linear wire permutation
uint16_t whiten_lfsr_value(uint16_t raw_val) {
    // 1. Apply single-cycle XOR-Shift (Mixes bits linearly)
    uint16_t xor_shifted = raw_val ^ (raw_val >> 3) ^ (raw_val << 5);
    xor_shifted &= 0xFFF; // Keep it strictly 12-bit

    // 2. Apply Permutation Mapping: Y_i = X_((5 * i + 3) mod 12)
    // This shatters the spatial shift-register tracking wires
    uint16_t permuted = 0;
    for (int i = 0; i < 12; ++i) {
        int src_bit_idx = (5 * i + 3) % 12;
        uint16_t bit = (xor_shifted >> src_bit_idx) & 1;
        permuted |= (bit << i);
    }
    return permuted;
}

// Autocorrelation function equivalent
double autocorr_forward(const std::vector<double>& bits, int max_lag = 5) {
    int N = bits.size();
    double sum = 0.0;
    for (double b : bits) sum += b;
    double mean = sum / N;

    double var_sum = 0.0;
    for (double b : bits) {
        var_sum += (b - mean) * (b - mean);
    }
    double var = var_sum / N;

    if (var < 1e-9) return 1.0;

    double total_corr = 0.0;
    int valid_lags = 0;

    for (int lag = 1; lag <= max_lag; ++lag) {
        double cov = 0.0;
        for (int i = 0; i < N - lag; ++i) {
            cov += (bits[i] - mean) * (bits[i + lag] - mean);
        }
        cov /= (N - lag);
        total_corr += std::abs(cov / var);
        valid_lags++;
    }

    return (valid_lags > 0) ? (total_corr / valid_lags) : 1.0;
}

// Result structure to store evaluation metrics
struct Result {
    int seed_val;
    double avg_error;
    int max_error;
    double avg_corr;
};

int main() {
    std::vector<uint16_t> seed_vals(N_SEEDS);
    for (int i = 0; i < N_SEEDS; ++i) {
        seed_vals[i] = static_cast<uint16_t>(i);
    }

    std::vector<uint16_t> actual_seeds = bit_reverse_12_arr(seed_vals);

    // Randomized B_refs using C++ random
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 127);

    std::vector<int32_t> B_refs(N_THRESH);
    std::cout << "B_refs used: [";
    for (int r = 0; r < N_THRESH; ++r) {
        B_refs[r] = r * 128 + dist(gen);
        std::cout << B_refs[r] << (r == N_THRESH - 1 ? "" : ", ");
    }
    std::cout << "]\n";

    // Initialize states and history
    std::vector<uint16_t> states = actual_seeds;
    std::vector<std::vector<uint16_t>> history(N_CYCLES, std::vector<uint16_t>(N_SEEDS));

    for (int t = 0; t < N_CYCLES; ++t) {
        for (int s = 0; s < N_SEEDS; ++s) {
            history[t][s] = states[s];
        }
        for (int s = 0; s < N_SEEDS; ++s) {
            uint16_t st = states[s];
            uint16_t b11 = (st >> 11) & 1;
            uint16_t b6  = (st >> 6) & 1;
            uint16_t b3  = (st >> 3) & 1;
            uint16_t b2  = (st >> 2) & 1;
            uint16_t feedback = b11 ^ b6 ^ b3 ^ b2;
            states[s] = ((st << 1) | feedback) & 0xFFF;
        }
    }

    std::vector<Result> results;
    results.reserve(N_SEEDS);

    for (int seed_val = 0; seed_val < N_SEEDS; ++seed_val) {
        std::vector<int32_t> S(N_CYCLES);
        for (int t = 0; t < N_CYCLES; ++t) {
            // FIXED: Intercept the historical LFSR value and pass it through the whitener
            S[t] = static_cast<int32_t>(whiten_lfsr_value(history[t][seed_val]));
        }

        double total_error = 0.0;
        double total_corr = 0.0;
        int max_error = 0;

        for (int32_t B : B_refs) {
            std::vector<double> bitstream(N_CYCLES);
            int hw_count = 0;
            for (int t = 0; t < N_CYCLES; ++t) {
                double bit = (S[t] <= B) ? 1.0 : 0.0;
                bitstream[t] = bit;
                if (bit == 1.0) hw_count++;
            }

            int err = std::abs(hw_count - static_cast<int>(B));
            total_error += err;
            if (err > max_error) {
                max_error = err;
            }
            total_corr += autocorr_forward(bitstream, 5);
        }

        double avg_error = total_error / N_THRESH;
        double avg_corr = total_corr / N_THRESH;
        results.push_back({seed_val, avg_error, max_error, avg_corr});
    }

    // Sort results: primary by avg_error (ascending), secondary by avg_corr (ascending)
    std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
        if (a.avg_error != b.avg_error)
            return a.avg_error < b.avg_error;
        return a.avg_corr < b.avg_corr;
    });

    // Print top 32 results
    std::cout << std::left << std::setw(5) << "Rank"
              << std::setw(10) << "Seed(Dec)"
              << std::setw(12) << "Seed(Hex)"
              << std::setw(10) << "AvgErr"
              << std::setw(8) << "MaxErr"
              << std::setw(8) << "Corr" << "\n";

    for (size_t i = 0; i < 32 && i < results.size(); ++i) {
        std::cout << std::left << std::setw(5) << (i + 1)
                  << std::setw(10) << results[i].seed_val
                  << "12'h" << std::uppercase << std::setfill('0') << std::setw(3) << std::hex << results[i].seed_val << std::dec << std::setfill(' ') << "   "
                  << std::left << std::setw(10) << std::fixed << std::setprecision(4) << results[i].avg_error
                  << std::setw(8) << results[i].max_error
                  << std::setw(8) << std::fixed << std::setprecision(4) << results[i].avg_corr << "\n";
    }

    // Save to CSV
    std::ofstream f("/seed_ver_report_diff.csv");
    if (f.is_open()) {
        f << "Seed(Dec),Seed(Hex),Average Error,Absolute Highest Error,Correlation Score\n";
        for (const auto& res : results) {
            f << res.seed_val << ",";
            f << "12'h" << std::uppercase << std::setfill('0') << std::setw(3) << std::hex << res.seed_val << std::dec << std::setfill(' ') << ",";
            f << std::fixed << std::setprecision(4) << res.avg_error << ",";
            f << res.max_error << ",";
            f << std::fixed << std::setprecision(4) << res.avg_corr << "\n";
        }
        f.close();
    }

    return 0;
}
