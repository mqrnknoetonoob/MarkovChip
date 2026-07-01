// Verification testbench for Comparator.v
//
// CONFIRMED ACTUAL BEHAVIOR (via simulation): x = 1 when A <= B, x = 0 when A > B.
// This matches the usage comment in Bit_Generator_Circuit.v ("If LFSR > Register, x = 0;
// otherwise, x = 1"), so we verify against THIS polarity as the golden reference.
//
// Exhaustively checks every (A,B) pair in the 12-bit space (4096 x 4096 = 16,777,216
// combinations) against a simple C++ reference model.

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include "VComparator.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    VComparator* top = new VComparator;

    uint64_t total = 0;
    uint64_t fails = 0;
    const uint64_t MAX_FAILS_TO_PRINT = 20;

    for (uint32_t a = 0; a < 4096; a++) {
        top->A = a;
        for (uint32_t b = 0; b < 4096; b++) {
            top->B = b;
            top->eval();

            // Golden reference model
            uint8_t expected_x = (a <= b) ? 1 : 0;

            total++;
            if ((uint8_t)top->x != expected_x) {
                fails++;
                if (fails <= MAX_FAILS_TO_PRINT) {
                    std::cerr << "MISMATCH: A=" << a << " B=" << b
                              << " got x=" << (int)top->x
                              << " expected=" << (int)expected_x << std::endl;
                }
            }
        }
    }

    std::cout << "========================================" << std::endl;
    std::cout << "Comparator exhaustive verification complete." << std::endl;
    std::cout << "Total vectors checked: " << total << std::endl;
    std::cout << "Mismatches: " << fails << std::endl;
    std::cout << "========================================" << std::endl;

    delete top;

    if (fails > 0) {
        std::cerr << "TEST FAILED" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "TEST PASSED" << std::endl;
    return EXIT_SUCCESS;
}
