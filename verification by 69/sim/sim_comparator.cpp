#include <iostream>
#include <memory>
#include <cstdlib>
#include <cassert>
#include "VComparator.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

void run_test_case(VComparator* top, VerilatedVcdC* tfp, uint64_t &sim_time, uint16_t val_A, uint16_t val_B, bool expected_x) {
    top->A = val_A & 0xFFF; 
    top->B = val_B & 0xFFF;
    top->eval();
    if (tfp) tfp->dump(sim_time);
    sim_time++;
    std::cout << "Time: " << sim_time 
              << " | A = " << top->A << " (0x" << std::hex << top->A << ")"
              << " | B = " << top->B << " (0x" << std::hex << top->B << ")"
              << " | Output x = " << (int)top->x 
              << " | Expected = " << expected_x << std::dec << std::endl;
    if (top->x != expected_x) {
        std::cerr << "--- TEST FAILED! Mismatch detected ---" << std::endl;
    }
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    VComparator* top = new VComparator;
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99); 
    tfp->open("waveform.vcd");
    uint64_t sim_time = 0;

    // Testing what x actually equals for A>B and A<B cases, both polarities documented
    run_test_case(top, tfp, sim_time, 500, 500, 0);   // A == B
    run_test_case(top, tfp, sim_time, 2048, 2047, 1); // A > B
    run_test_case(top, tfp, sim_time, 255, 256, 0);   // A < B
    run_test_case(top, tfp, sim_time, 4095, 2048, 1); // A > B
    run_test_case(top, tfp, sim_time, 10, 4000, 0);   // A < B
    run_test_case(top, tfp, sim_time, 4095, 0, 1);    // A > B

    tfp->close();
    delete top;
    delete tfp;
    return 0;
}
