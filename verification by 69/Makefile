.PHONY: all comparator bitgen seed_sweep clean waves

VERILATOR = verilator

all: comparator bitgen seed_sweep

# --- Comparator standalone exhaustive verification ---
comparator:
	$(VERILATOR) -Wall --cc RTL/Comparator.v RTL/priority_encoder.v \
		--exe sim/tb_comparator.cpp -Mdir obj_dir_cmp
	$(MAKE) -C obj_dir_cmp -f VComparator.mk VComparator
	./obj_dir_cmp/VComparator

# --- Bit_Generator_Circuit integration verification (with reference model) ---
# Note: -Wno-fatal is used because this design currently has one known lint
# warning (UNUSEDSIGNAL on 'seed') tracked as a finding in VERIFICATION_REPORT.md.
# Remove -Wno-fatal once that's fixed to keep the build fatal-clean going forward.
bitgen:
	$(VERILATOR) -Wall -Wno-fatal --cc \
		RTL/Bit_Generator_Circuit.v RTL/Seed_Memory.v RTL/LFSR.v RTL/Shift_Register.v \
		RTL/Comparator.v RTL/priority_encoder.v RTL/counter.v \
		--exe sim/tb_bit_generator.cpp --trace -Mdir obj_dir_bitgen
	$(MAKE) -C obj_dir_bitgen -f VBit_Generator_Circuit.mk VBit_Generator_Circuit
	./obj_dir_bitgen/VBit_Generator_Circuit

# View the top-level waveform in GTKWave (must be installed separately)
waves:
	gtkwave waveform_bitgen.vcd &

# --- 256-seed sweep: scores every seed in RTL/LFSR.v's documented list by
# how closely its comparator hit-count tracks the statistically expected
# value across a sweep of thresholds. See sim/seed_sweep/tb_seed_sweep.cpp
# for the full methodology and CLI options (--cycles, --thresholds, --csv).
seed_sweep:
	$(VERILATOR) -Wall -Wno-fatal --cc -Isim/seed_sweep \
		sim/seed_sweep/Seed_Sweep_Top.v RTL/LFSR.v RTL/Comparator.v RTL/priority_encoder.v RTL/counter.v \
		--exe sim/seed_sweep/tb_seed_sweep.cpp -Mdir obj_dir_sweep
	$(MAKE) -C obj_dir_sweep -f VSeed_Sweep_Top.mk VSeed_Sweep_Top
	./obj_dir_sweep/VSeed_Sweep_Top

clean:
	rm -rf obj_dir_cmp obj_dir_bitgen obj_dir_sweep *.vcd seed_sweep_results.csv
