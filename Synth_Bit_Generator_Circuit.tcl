read_verilog RTL/Bit_Generator_Circuit.v RTL/Comparator.v RTL/Counter.v RTL/priority_encoder.v RTL/LFSR.v RTL/Shift_Register.v 

read_liberty -lib gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib

synth -top Bit_Generator_Circuit -flatten

abc -liberty gf180mcu_fd_sc_mcu7t5v0__tt_025C_1v80.lib
clean

write_verilog Netlist_Bit_Generator_Circuit.v