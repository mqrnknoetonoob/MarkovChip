#include <iostream>
#include <cstdint>
#include <cstdlib>
#include "VShift_Register.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    VShift_Register* dut = new VShift_Register; // Instantiate the Shift Register module

    uint64_t fails = 0;

    // Helper lambda function to toggle the clock
    auto tick_clock = [&]() {
        dut->clk = 0; dut->eval();
        dut->clk = 1; dut->eval();
    };

    // Initialize all input ports
    dut->clk = 0;
    dut->reset = 0;
    dut->mode = 0b00;
    dut->serial_in = 0;
    dut->parallel_in = 0;

    std::cout << "=== Test 1: Asynchronous Active-High Reset ===" << std::endl;
    dut->reset = 1; // Assert high reset
    dut->eval();    // Evaluates immediately without clock edge
    if (dut->q != 0) {
        std::cerr << "FAIL: Active-high reset did not clear register. Got: 0x" << std::hex << dut->q << std::endl;
        fails++;
    }
    dut->reset = 0; // Release reset
    dut->eval();

    std::cout << "=== Test 2: Parallel Load Mode (mode = 10) ===" << std::endl;
    dut->mode = 0b10;          // Parallel load mode
    dut->parallel_in = 0xA5A;   // 12-bit alternating bit pattern (101001011010)
    tick_clock();
    if (dut->q != 0xA5A) {
        std::cerr << "FAIL: Parallel load failed. Expected 0xA5A, Got: 0x" << std::hex << dut->q << std::endl;
        fails++;
    }

    std::cout << "=== Test 3: Hold Mode (mode = 00) ===" << std::endl;
    dut->mode = 0b00;          // No-operation mode
    dut->parallel_in = 0x000;   // Change input to ensure it's ignored
    tick_clock();
    if (dut->q != 0xA5A) {
        std::cerr << "FAIL: Register altered state during hold mode. Got: 0x" << std::hex << dut->q << std::endl;
        fails++;
    }

    std::cout << "=== Test 4: Shift Right Mode (mode = 01) with Serial Input '1' ===" << std::endl;
    dut->mode = 0b01;       // Shift right mode
    dut->serial_in = 1;     // Feed 1 into MSB (bit 11)
    
    // Initial state: 0xA5A -> 1010 0101 1010
    // Expected after shift right: MSB becomes 1, right-most bit drops out.
    // 1 -> 1010 0101 101 -> 1101 0010 1101 -> 0xD2D
    tick_clock();
    if (dut->q != 0xD2D) {
        std::cerr << "FAIL: Shift right with serial_in=1 failed. Expected 0xD2D, Got: 0x" << std::hex << dut->q << std::endl;
        fails++;
    }

    std::cout << "=== Test 5: Shift Right Mode (mode = 01) with Serial Input '0' ===" << std::endl;
    dut->serial_in = 0;     // Feed 0 into MSB
    
    // Current state: 0xD2D -> 1101 0010 1101
    // Expected after shift right: 0 -> 1101 0010 110 -> 0110 1001 0110 -> 0x696
    tick_clock();
    if (dut->q != 0x696) {
        std::cerr << "FAIL: Shift right with serial_in=0 failed. Expected 0x696, Got: 0x" << std::hex << dut->q << std::endl;
        fails++;
    }

    std::cout << "========================================" << std::endl;
    if (fails == 0) {
        std::cout << "ALL TESTS PASSED! Universal Shift Register behaves perfectly." << std::endl;
    } else {
        std::cout << fails << " MISMATCH(ES) DETECTED." << std::endl;
    }
    std::cout << "========================================" << std::endl;

    delete dut;
    return fails == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}