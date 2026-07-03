#include <iostream>
#include <cstdint>
#include <cstdlib>
#include "Vpriority_encoder.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    // Instantiate the Verilated Verilog module
    Vpriority_encoder* top = new Vpriority_encoder;

    uint64_t total = 0;
    uint64_t fails = 0;
    const uint64_t MAX_FAILS_TO_PRINT = 20;

    // Exhaustive test: Loop through all 4096 possible 12-bit inputs
    for (uint32_t val = 0; val < 4096; val++) {
        top->in = val;
        top->eval(); // Evaluate the Verilog logic

        // Golden reference model: 
        // Emulating the MSB-priority logic found in your Verilog
        uint8_t expected_out = 0;
        for (int i = 0; i < 12; i++) {
            if ((val >> i) & 1) {
                expected_out = i + 1;
            }
        }

        total++;
        // Compare Verilog output against C++ expected output
        if ((uint8_t)top->out != expected_out) {
            fails++;
            if (fails <= MAX_FAILS_TO_PRINT) {
                std::cerr << "MISMATCH: in=" << val 
                          << " got out=" << (int)top->out 
                          << " expected=" << (int)expected_out << std::endl;
            }
        }
    }

    // Print summary report to the terminal
    std::cout << "========================================" << std::endl;
    std::cout << "Priority Encoder exhaustive verification complete." << std::endl;
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