# 1. Read all RTL Verilog files
read_verilog \
    ../RTL/Adder_Tree_16in.v \
    ../RTL/Comparator.v \
    ../RTL/counter.v \
    ../RTL/LFSR.v \
    ../RTL/LFSR_Enabler.v \
    ../RTL/Markov_Chain_Accelerator.v \
    ../RTL/memory_enable_decoder.v \
    ../RTL/priority_encoder.v \
    ../RTL/Result_Shift_Memory.v \
    ../RTL/Row_Col_BGC.v \
    ../RTL/Shift_Register.v \
    ../RTL/Stochastic_Timer.v

# 2. Set the top-level module
hierarchy -top Markov_Chain_Accelerator

# 3. Generic synthesis
synth -top Markov_Chain_Accelerator

# 4. Flip-flop mapping using gf180mcu liberty file
dfflibmap -liberty ../gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib

# 5. Logic gate mapping using abc with gf180mcu liberty file
abc -liberty ../gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib

# 6. Clean up unused nets/cells
clean

# 7. Write the synthesized netlist
write_verilog -noattr ../Synthesized_Files/Netlist_Files/Markov_Chain_Accelerator_Netlist_Final.v

# 8. Generate and save area/cell statistics report
tee -o ../Synthesized_Files/Yosys_Output_Log/stat_report_final.txt stat -liberty ../gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib