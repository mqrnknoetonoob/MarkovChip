.PHONY: all comparator bitgen clean waves

VERILATOR = verilator

all: comparator bitgen

# --- Comparator standalone exhaustive verification ---
comparator:
	$(VERILATOR) -Wall --cc ../RTL/Comparator.v ../RTL/priority_encoder.v \
		--exe sim_comparator.cpp -Mdir obj_dir_cmp
	$(MAKE) -C obj_dir_cmp -f VComparator.mk VComparator
	./obj_dir_cmp/VComparator

# --- Bit_Generator_Circuit integration verification ---
bitgen:
	$(VERILATOR) -Wall -Wno-fatal --cc \
		../RTL/Bit_Generator_Circuit.v ../RTL/LFSR.v ../RTL/Shift_Register.v \
		../RTL/Comparator.v ../RTL/priority_encoder.v ../RTL/counter.v \
		--exe tb_bit_generator.cpp --trace -Mdir obj_dir_bitgen
	$(MAKE) -C obj_dir_bitgen -f VBit_Generator_Circuit.mk VBit_Generator_Circuit
	./obj_dir_bitgen/VBit_Generator_Circuit

# View the top-level waveform in GTKWave
waves:
	gtkwave waveform_bitgen.vcd &

clean:
	rm -rf obj_dir_cmp obj_dir_bitgen obj_dir_sweep *.vcd seed_verification_report.csv