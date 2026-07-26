// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "VBit_Generator_Circuit__Syms.h"


void VBit_Generator_Circuit::traceChgTop0(void* userp, VerilatedVcd* tracep) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Variables
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    {
        vlTOPp->traceChgSub0(userp, tracep);
    }
}

void VBit_Generator_Circuit::traceChgSub0(void* userp, VerilatedVcd* tracep) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    vluint32_t* const oldp = tracep->oldp(vlSymsp->__Vm_baseCode + 1);
    if (false && oldp) {}  // Prevent unused
    // Body
    {
        if (VL_UNLIKELY(vlTOPp->__Vm_traceActivity[1U])) {
            tracep->chgSData(oldp+0,(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out),12);
            tracep->chgSData(oldp+1,(vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out),12);
            tracep->chgBit(oldp+2,(((IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index) 
                                    <= (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index))));
            tracep->chgBit(oldp+3,((1U & (((((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                             >> 0xbU) 
                                            ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                               >> 6U)) 
                                           ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                              >> 3U)) 
                                          ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                             >> 2U)))));
            tracep->chgSData(oldp+4,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits),12);
            tracep->chgSData(oldp+5,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits),12);
            tracep->chgCData(oldp+6,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index),4);
            tracep->chgCData(oldp+7,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index),4);
        }
        tracep->chgBit(oldp+8,(vlTOPp->clk));
        tracep->chgBit(oldp+9,(vlTOPp->rst_n));
        tracep->chgSData(oldp+10,(vlTOPp->seed_in),12);
        tracep->chgSData(oldp+11,(vlTOPp->par_in),12);
        tracep->chgCData(oldp+12,(vlTOPp->mode),2);
        tracep->chgBit(oldp+13,(vlTOPp->lfsr_en));
        tracep->chgBit(oldp+14,(vlTOPp->count_en));
        tracep->chgBit(oldp+15,(vlTOPp->on_switch));
        tracep->chgSData(oldp+16,(vlTOPp->total_count),12);
        tracep->chgBit(oldp+17,((1U & (~ (IData)(vlTOPp->rst_n)))));
        tracep->chgBit(oldp+18,(((IData)(vlTOPp->count_en) 
                                 & ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index) 
                                    <= (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index)))));
    }
}

void VBit_Generator_Circuit::traceCleanup(void* userp, VerilatedVcd* /*unused*/) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlSymsp->__Vm_activity = false;
        vlTOPp->__Vm_traceActivity[0U] = 0U;
        vlTOPp->__Vm_traceActivity[1U] = 0U;
    }
}
