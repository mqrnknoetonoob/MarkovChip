#pragma once
#include <array>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>


// ---------------------------------------------------------------------------
// Golden model: matches the user's original C++ reference exactly.
//   B0 = row p of A  (NOT one-hot(p) -- this already represents the state
//        distribution after 1 step starting from state p)
//   B  = B0 * A^r    (r further multiplications, pure floating point)
// This is the *ideal* reference -- no quantization loss modeled here at all,
// so it is compared against the dequantized hardware result.
// ---------------------------------------------------------------------------

using Matrix16 = std::array<std::array<double, 16>, 16>; // A[i][j] = from i to j
using Vec16    = std::array<double, 16>;
using QVec16   = std::array<uint16_t, 16>; // quantized, 0..4095 each

// quantize(x) = round(x * 4096) - 1, clamped to [0, 4095]
// (matches the hardware's Comparator: density of 1s = (threshold+1)/4096)
inline uint16_t quantize(double x) {
    long v = std::lround(x * 4096.0) - 1;
    if (v < 0)    v = 0;
    if (v > 4095) v = 4095;
    return static_cast<uint16_t>(v);
}

inline double dequantize(uint16_t q) {
    return (static_cast<double>(q) + 1.0) / 4096.0;
}

// Reads a 16x16 whitespace-separated matrix of doubles from `filename`.
inline bool read_matrix(const std::string& filename, Matrix16& A) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (!(file >> A[i][j])) {
                std::cerr << "Error: Not enough data in file. Need 256 float values." << std::endl;
                return false;
            }
        }
    }
    return true;
}

// Exactly the user's multiply(): B <- B * A  (vector-matrix product)
inline void multiply(Vec16& B, const Matrix16& A) {
    Vec16 temp{};
    for (int j = 0; j < 16; j++) {
        for (int k = 0; k < 16; k++) {
            temp[j] += B[k] * A[k][j];
        }
    }
    B = temp;
}

// Runs the golden model r times, starting from B0 = row p of A.
// Returns the final floating-point reference vector.
inline Vec16 run_golden(const Matrix16& A, int p, int r) {
    Vec16 B{};
    for (int j = 0; j < 16; j++) B[j] = A[p][j];
    for (int i = 0; i < r; i++) multiply(B, A);
    return B;
}