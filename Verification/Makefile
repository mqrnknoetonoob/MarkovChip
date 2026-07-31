.PHONY: all comparator bitgen main_build main_run clean waves

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
               $(RTL_DIR)/Result_Output_Shifter.v

CPP_FILES    = $(VERIF_CODE)/Markov_Chain_Accelerator.cpp

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

MATRIX ?= $(INPUT_DIR)/matrix.txt
P ?= 0
R ?= 5

main_run:
	@echo "Running Main Module Simulation..."
	./$(OBJ_DIR)/VMarkov_Chain_Accelerator $(MATRIX) $(P) $(R)

# View the top-level waveform in GTKWave
waves:
	gtkwave waveform_bitgen.vcd &

# --- Clean Generated Files ---
clean:
	@echo "Cleaning up..."
	rm -rf obj_dir_cmp obj_dir_bitgen obj_dir_sweep $(OBJ_DIR) *.vcd seed_verification_report.csv