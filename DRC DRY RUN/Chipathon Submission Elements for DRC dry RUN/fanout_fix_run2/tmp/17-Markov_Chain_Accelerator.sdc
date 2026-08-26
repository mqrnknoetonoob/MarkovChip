###############################################################################
# Created by write_sdc
# Thu Aug  6 15:27:59 2026
###############################################################################
current_design Markov_Chain_Accelerator
###############################################################################
# Timing Constraints
###############################################################################
create_clock -name clk -period 20.0000 [get_ports {clk}]
set_clock_transition 0.1500 [get_clocks {clk}]
set_clock_uncertainty 0.2500 clk
set_propagated_clock [get_clocks {clk}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {input_ready}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {load_row_or_col}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {output_valid}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {rst_n}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_in0}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_in1}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_in2}]
set_input_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_in3}]
set_output_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {output_ready}]
set_output_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_out0}]
set_output_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_out1}]
set_output_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_out2}]
set_output_delay 4.0000 -clock [get_clocks {clk}] -add_delay [get_ports {spi_out3}]
###############################################################################
# Environment
###############################################################################
set_load -pin_load 0.0729 [get_ports {output_ready}]
set_load -pin_load 0.0729 [get_ports {spi_out0}]
set_load -pin_load 0.0729 [get_ports {spi_out1}]
set_load -pin_load 0.0729 [get_ports {spi_out2}]
set_load -pin_load 0.0729 [get_ports {spi_out3}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_4 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {clk}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {input_ready}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {load_row_or_col}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {output_valid}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {rst_n}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {spi_in0}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {spi_in1}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {spi_in2}]
set_driving_cell -lib_cell gf180mcu_fd_sc_mcu7t5v0__inv_1 -pin {ZN} -input_transition_rise 0.0000 -input_transition_fall 0.0000 [get_ports {spi_in3}]
set_timing_derate -early 0.9500
set_timing_derate -late 1.0500
###############################################################################
# Design Rules
###############################################################################
set_max_transition 3.0000 [current_design]
set_max_fanout 32.0000 [current_design]
