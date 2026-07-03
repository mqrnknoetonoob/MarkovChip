#include <iostream>
#include <cstdint>
#include <cstdlib>
#include "VLFSR.h"
#include "verilated.h"

// Golden Reference Model: C++ calculation of the LFSR feedback polynomial
uint16_t get_next_lfsr_state(uint16_t current_state) {
    // Feedback polynomial: x^12 + x^7 + x^4 + x^3 + 1
    // Tap positions 11, 6, 3, 2 (0-indexed matching q[11] ^ q[6] ^ q[3] ^ q[2])
    uint8_t feedback = ((current_state >> 11) ^ (current_state >> 6) ^ (current_state >> 3) ^ (current_state >> 2)) & 1;
    return ((current_state << 1) | feedback) & 0xFFF; // Shift left, insert feedback at LSB, mask to 12 bits
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    VLFSR* dut = new VLFSR; // Instantiate the Verilog module

    uint64_t fails = 0;

    // Helper lambda function to toggle the clock
    auto tick_clock = [&]() {
        dut->clk = 0; dut->eval();
        dut->clk = 1; dut->eval();
    };

    std::cout << "=== Test 1: Asynchronous Reset ===" << std::endl;
    dut->clk = 0;
    dut->en = 0;
    dut->rst_n = 0; // Assert active-low reset
    dut->eval();    // Async evaluation

    // Capture the state loaded by the parameter parameter injection on reset
    uint16_t initial_seed = dut->q;
    std::cout << "LFSR initialized via parameter SEED value: 0x" << std::hex << initial_seed << std::dec << std::endl;

    dut->rst_n = 1; // Release reset
    dut->eval();

    std::cout << "=== Test 2: Clocking with en = 0 (Hold State) ===" << std::endl;
    for (int i = 0; i < 5; i++) {
        tick_clock();
        if (dut->q != initial_seed) {
            std::cerr << "FAIL: LFSR state changed while en=0! Got: 0x" << std::hex << dut->q << std::endl;
            fails++;
        }
    }

    std::cout << "=== Test 3: Run LFSR (en = 1) for a full 4,095 cycle period ===" << std::endl;
    dut->en = 1; // Enable shifting sequence
    uint16_t expected_state = initial_seed;
    
    for (int i = 0; i < 4095; i++) {
        expected_state = get_next_lfsr_state(expected_state);
        tick_clock();
        
        if (dut->q != expected_state) {
            std::cerr << "FAIL at cycle " << std::dec << i 
                      << ": Expected 0x" << std::hex << expected_state 
                      << " but got 0x" << dut->q << std::endl;
            fails++;
            if (fails > 10) break; // Halts output spam if the loop breaks entirely
        }
    }

    std::cout << "========================================" << std::endl;
    if (fails == 0) {
        std::cout << "ALL TESTS PASSED! LFSR feedback tracks the polynomial perfectly." << std::endl;
    } else {
        std::cout << fails << " MISMATCH(ES) DETECTED." << std::endl;
    }
    std::cout << "========================================" << std::endl;

    delete dut;
    return fails == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}