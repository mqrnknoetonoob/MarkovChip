// ============================================================================
// SIMULATION-ONLY replacement for gf180mcu_fd_sc_mcu7t5v0__dffrnq_1.
//
// The original PDK "functional" model uses a Verilog UDP
// (UDP_GF018hv5v_..._MGM_N_IQ_FF_UDP), which Verilator cannot parse
// (no UDP/primitive-table support). This module reproduces the SAME
// logical behavior with plain Verilog so Verilator can build the
// gate-level netlist for functional co-simulation.
//
// Derived from the PDK source (dffrnq_1.functional.v):
//   not MGM_BG_0( MGM_P0, RN );              // MGM_P0 = ~RN
//   not MGM_BG_1( MGM_D0, D  );               // MGM_D0 = ~D
//   UDP_..._MGM_N_IQ_FF_UDP( IQ1, 1'b0, MGM_P0, CLK, MGM_D0, notifier );
//   not MGM_BG_2( Q, IQ1 );                   // Q = ~IQ1
// The two inversions on D and on the output cancel out functionally,
// leaving a standard active-low async-reset, rising-edge D flip-flop:
//   RN low  -> Q asynchronously clears to 0
//   RN high -> Q <= D on the rising edge of CLK
//
// NOT for synthesis, STA, DRC, or LVS -- those still use the real PDK
// files unchanged. This file exists ONLY to let sim_main_gatelevel.cpp
// build and run against the gate-level netlist.
// ============================================================================

module gf180mcu_fd_sc_mcu7t5v0__dffrnq_1 ( CLK, D, RN, Q, notifier );
    input  CLK, D, RN, notifier;
    output reg Q;

    always @(posedge CLK or negedge RN) begin
        if (!RN)
            Q <= 1'b0;
        else
            Q <= D;
    end

endmodule