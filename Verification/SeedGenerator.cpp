#include <iostream>
#include <vector>
#include <random>
#include <iomanip>

int main() {
    // Random number generator setup
    std::random_device rd;  // Non-deterministic seed
    std::mt19937 gen(rd()); // Mersenne Twister engine
    
    // Range for 12-bit: 1 to 4095 (excluding 0)
    std::uniform_int_distribution<uint16_t> dis(5, 4091);

    std::vector<uint16_t> seeds(16);

    std::cout << "Generated 16 Seeds (Hex format for Verilog):" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    for (int i = 0; i < 256; ++i) {
        seeds[i] = dis(gen);
        
        // Printing in Verilog compatible format: 12'hXXX
        std::cout << "seeds[" << std::setw(2) << i << "] = 12'h" 
                  << std::hex << std::uppercase << std::setw(3) << std::setfill('0') 
                  << seeds[i] << ";" << std::endl;
    }

    return 0;
}