.PHONY: all comparator bitgen main_build main_run gate_build gate_run phys_build phys_run clean waves

VERILATOR = verilator

# --- Main Module Paths ---
RTL_DIR      = ../RTL
VERIF_CODE   = Main_Module_Verification/Verification_Code
INPUT_DIR    = Main_Module_Verification/Input
OBJ_DIR      = obj_dir

VERILATOR_FLAGS = --cc --exe --build -Wall \
                --top-module Markov_Chain_Accelerator \
                -I$(RTL_DIR)

RTL_FILES    = $(RTL_DIR)/Markov_Chain_Accelerator.v \
               $(RTL_DIR)/LFSR_Enabler.v \
               $(RTL_DIR)/Stochastic_Timer.v \
               $(RTL_DIR)/Row_Col_BGC.v \
               $(RTL_DIR)/Shift_Register.v \
               $(RTL_DIR)/LFSR.v \
               $(RTL_DIR)/Comparator.v \
               $(RTL_DIR)/priority_encoder.v \
               $(RTL_DIR)/counter.v \
               $(RTL_DIR)/Adder_Tree_16in.v \
               $(RTL_DIR)/Result_Shift_Memory.v \
               $(RTL_DIR)/memory_enable_decoder.v \

CPP_FILES    = $(VERIF_CODE)/Markov_Chain_Accelerator.cpp

# --- Gate-Level (post-synthesis netlist) Paths ---
NETLIST_DIR  = ../Synthesized_Files/Netlist_Files
PDK_DIR      = ../GF180MCU_PDK/verilog_only
OBJ_DIR_GATE = obj_dir_gate

NETLIST_FILE = $(NETLIST_DIR)/Markov_Chain_Accelerator_Netlist_Final.v

# Exact set of standard cells instantiated in the synthesis netlist (_1 drive
# strength only). Functional (not behavioral) models -- no SDF timing needed.
#
# NOTE: dffrnq_1 and dffrsnq_1 are NOT pulled from PDK_DIR -- the real PDK
# functional models use a Verilog UDP that Verilator cannot parse. We use
# plain-Verilog, behavior-equivalent replacements instead (see
# SIM_REPLACEMENT_FILES below). Simulation-only; synthesis/DRC/LVS still use
# the real PDK files untouched.
CELL_FILES   = $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__and2_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__and3_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__and4_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__aoi21_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__aoi211_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__aoi22_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__aoi221_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__clkinv_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__mux2_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__nand2_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__nand3_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__nand4_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__nor2_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__nor3_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__nor4_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__oai21_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__oai211_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__oai22_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__oai221_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__oai31_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__oai32_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__or2_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__or4_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__xnor2_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__xnor3_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__xor2_1.functional.v \
               $(PDK_DIR)/gf180mcu_fd_sc_mcu7t5v0__xor3_1.functional.v

SIM_REPLACEMENT_FILES = $(VERIF_CODE)/gf180mcu_fd_sc_mcu7t5v0__dffrnq_1.sim_replacement.v \
               $(VERIF_CODE)/gf180mcu_fd_sc_mcu7t5v0__dffrsnq_1.sim_replacement.v

GATE_VERILATOR_FLAGS = --cc --exe --build -Wall -Wno-fatal \
                --top-module Markov_Chain_Accelerator \
                --Mdir $(OBJ_DIR_GATE)

CPP_FILES_GATE = $(VERIF_CODE)/sim_main_gatelevel.cpp

# --- Physical Netlist (GDS-extracted / routed netlist) Verification Targets ---
OBJ_DIR_PHYS = obj_dir_phys
PHYS_NETLIST = ../Physical_Design/Markov_Chain_Accelerator.nl.v

PHYS_VERILATOR_FLAGS = --cc --exe --build -Wall -Wno-fatal \
                --top-module Markov_Chain_Accelerator \
                --Mdir $(OBJ_DIR_PHYS)

phys_build:
	@echo "Building Physical Design (post-layout) Verilator model..."
	$(VERILATOR) $(PHYS_VERILATOR_FLAGS) $(CPP_FILES_GATE) $(PHYS_NETLIST) $(CELL_FILES) $(SIM_REPLACEMENT_FILES)

phys_run:
	@echo "Running Physical Design Netlist Simulation..."
	./$(OBJ_DIR_PHYS)/VMarkov_Chain_Accelerator $(MATRIX) $(P) $(R)

# --- Default Target ---
all: comparator bitgen main_build main_run

# --- Comparator standalone exhaustive verification ---
comparator:
	$(VERILATOR) -Wall --cc $(RTL_DIR)/Comparator.v $(RTL_DIR)/priority_encoder.v \
		--exe sim_comparator.cpp -Mdir obj_dir_cmp
	$(MAKE) -C obj_dir_cmp -f VComparator.mk VComparator
	./obj_dir_cmp/VComparator

# --- Bit_Generator_Circuit integration verification ---
bitgen:
	$(VERILATOR) -Wall -Wno-fatal --cc \
		$(RTL_DIR)/Bit_Generator_Circuit.v \
		$(RTL_DIR)/LFSR.v \
		$(RTL_DIR)/Shift_Register.v \
		$(RTL_DIR)/Comparator.v \
		$(RTL_DIR)/priority_encoder.v \
		$(RTL_DIR)/counter.v \
		--exe tb_bit_generator.cpp --trace -Mdir obj_dir_bitgen
	$(MAKE) -C obj_dir_bitgen -f VBit_Generator_Circuit.mk VBit_Generator_Circuit
	./obj_dir_bitgen/VBit_Generator_Circuit

# --- Main Module Verification Targets ---
main_build:
	@echo "Building Main Markov Chain Accelerator Verilator model..."
	$(VERILATOR) $(VERILATOR_FLAGS) $(CPP_FILES) $(RTL_FILES)

MATRIX ?= $(INPUT_DIR)/Matrix1.txt
P ?= 0
R ?= 5

main_run:
	@echo "Running Main Module Simulation..."
	./$(OBJ_DIR)/VMarkov_Chain_Accelerator $(MATRIX) $(P) $(R)

# --- Gate-Level (post-synthesis netlist) Verification Targets ---
gate_build:
	@echo "Building Gate-Level (post-synthesis) Verilator model..."
	$(VERILATOR) $(GATE_VERILATOR_FLAGS) $(CPP_FILES_GATE) $(NETLIST_FILE) $(CELL_FILES) $(SIM_REPLACEMENT_FILES)

gate_run:
	@echo "Running Gate-Level Simulation..."
	./$(OBJ_DIR_GATE)/VMarkov_Chain_Accelerator $(MATRIX) $(P) $(R)

# View the top-level waveform in GTKWave
waves:
	gtkwave waveform_bitgen.vcd &

# --- Clean Generated Files ---
clean:
	@echo "Cleaning up..."
	rm -rf obj_dir_cmp obj_dir_bitgen obj_dir_sweep $(OBJ_DIR) $(OBJ_DIR_GATE) $(OBJ_DIR_PHYS) *.vcd seed_verification_report.csv