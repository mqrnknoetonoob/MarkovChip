#include <iostream>
#include <cstdint>
#include <cstdlib>
#include "Vcounter.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vcounter* dut = new Vcounter; // Instantiate the Verilog counter module

    uint64_t fails = 0;

    // Helper lambda function to toggle the clock
    auto tick_clock = [&]() {
        dut->clk = 0; dut->eval();
        dut->clk = 1; dut->eval();
    };

    // Initialize all inputs
    dut->clk = 0;
    dut->rst_n = 1;
    dut->on_switch = 0;
    dut->count_en = 0;

    std::cout << "=== Test 1: Asynchronous Reset ===" << std::endl;
    dut->rst_n = 0; 
    dut->eval(); // Async evaluation (no clock edge needed)
    if (dut->count != 0) {
        std::cerr << "FAIL: Asynchronous reset failed to clear counter. Got: " << (int)dut->count << std::endl;
        fails++;
    }
    dut->rst_n = 1; // Release reset
    dut->eval();

    std::cout << "=== Test 2: Synchronous Reset via on_switch ===" << std::endl;
    dut->on_switch = 0; // Switch is off
    dut->count_en = 1;  // Try to enable counting
    tick_clock();
    if (dut->count != 0) {
        std::cerr << "FAIL: Counter incremented while on_switch was active-low! Got: " << (int)dut->count << std::endl;
        fails++;
    }

    std::cout << "=== Test 3: Normal Counting Sequence ===" << std::endl;
    dut->on_switch = 1; // Turn switch on
    dut->count_en = 1;  // Enable counting
    
    uint16_t expected_count = 0;
    for (int i = 0; i < 20; i++) {
        tick_clock();
        expected_count++;
        if (dut->count != expected_count) {
            std::cerr << "FAIL at step " << i << ": Expected " << expected_count << " but got " << (int)dut->count << std::endl;
            fails++;
        }
    }

    std::cout << "=== Test 4: Count Enable (Hold Value) ===" << std::endl;
    dut->count_en = 0; // Disable counting
    uint16_t hold_value = dut->count;
    for (int i = 0; i < 5; i++) {
        tick_clock();
        if (dut->count != hold_value) {
            std::cerr << "FAIL: Counter modified value while count_en=0! Got: " << (int)dut->count << std::endl;
            fails++;
        }
    }

    std::cout << "=== Test 5: Exhaustive Rollover Check ===" << std::endl;
    dut->count_en = 1; // Re-enable counting
    expected_count = dut->count;
    
    // Run the counter through a complete cycle until it wraps around
    // 4096 steps will guarantee we test every value up to 4095 and cross 0 again
    for (int i = 0; i < 4096; i++) {
        tick_clock();
        expected_count = (expected_count + 1) & 0xFFF; // Mask to 12-bit bound
        
        if (dut->count != expected_count) {
            std::cerr << "FAIL during rollover sweep at value " << expected_count << " got: " << (int)dut->count << std::endl;
            fails++;
            if (fails > 10) break; // Terminate spam if broken
        }
    }

    std::cout << "========================================" << std::endl;
    if (fails == 0) {
        std::cout << "ALL TESTS PASSED! Counter logic and control lines verified." << std::endl;
    } else {
        std::cout << fails << " MISMATCH(ES) DETECTED." << std::endl;
    }
    std::cout << "========================================" << std::endl;

    delete dut;
    return fails == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}