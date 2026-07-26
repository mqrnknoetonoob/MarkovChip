# 1. Read all the RTL files
read_verilog RTL/Bit_Generator_Circuit.v RTL/Comparator.v RTL/priority_encoder.v RTL/Counter.v RTL/LFSR.v RTL/Shift_Register.v 

# 2. Read the technology library
read_liberty -lib gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib

# 3. Synthesize the design and flatten the hierarchy
synth -top Bit_Generator_Circuit -flatten

# 4. Map the design to the target technology library
abc -liberty gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib
clean

# 5. Print area and statistics report
stat -liberty gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib