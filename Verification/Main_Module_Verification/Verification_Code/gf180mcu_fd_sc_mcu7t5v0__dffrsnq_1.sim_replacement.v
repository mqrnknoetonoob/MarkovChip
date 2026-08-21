// ============================================================================
// SIMULATION-ONLY replacement for gf180mcu_fd_sc_mcu7t5v0__dffrsnq_1.
//
// Same situation as dffrnq_1: the PDK "functional" model calls a Verilog
// UDP (UDP_GF018hv5v_..._MGM_HN_IQ_FF_UDP) that Verilator cannot parse.
// This is a plain-Verilog behavioral equivalent for simulation only.
//
// Derived from the PDK source (dffrsnq_1.functional.v):
//   not MGM_BG_0( MGM_P0, RN );               // MGM_P0 = ~RN
//   not MGM_BG_1( MGM_C0, SETN );              // MGM_C0 = ~SETN
//   not MGM_BG_2( MGM_D0, D );                 // MGM_D0 = ~D
//   UDP_..._MGM_HN_IQ_FF_UDP( IQ1, MGM_C0, MGM_P0, CLK, MGM_D0, notifier );
//   not MGM_BG_3( Q, IQ1 );                    // Q = ~IQ1
// -> active-low async reset (RN) + active-low async set (SETN),
//    rising-edge D flip-flop:
//   RN low     -> Q asynchronously clears to 0
//   SETN low   -> Q asynchronously sets to 1
//   both high  -> Q <= D on the rising edge of CLK
//
// ASSUMPTION (please verify against the PDK liberty/datasheet if your
// design can ever drive RN and SETN low at the same time): reset is
// modeled here as dominant over set when both are asserted simultaneously.
// In this netlist SETN is actively driven (used for LFSR seed-loading on
// seed_mem registers, not tied to a constant), so this priority choice
// could matter if reset and seed-load logic ever overlap in time.
//
// NOT for synthesis, STA, DRC, or LVS -- those still use the real PDK
// files unchanged. This file exists ONLY to let sim_main_gatelevel.cpp
// build and run against the gate-level netlist.
// ============================================================================

module gf180mcu_fd_sc_mcu7t5v0__dffrsnq_1 ( CLK, D, SETN, RN, Q, notifier );
    input  CLK, D, RN, SETN, notifier;
    output reg Q;

    always @(posedge CLK or negedge RN or negedge SETN) begin
        if (!RN)
            Q <= 1'b0;
        else if (!SETN)
            Q <= 1'b1;
        else
            Q <= D;
    end

endmodule