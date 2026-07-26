// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "VBit_Generator_Circuit__Syms.h"


//======================

void VBit_Generator_Circuit::trace(VerilatedVcdC* tfp, int, int) {
    tfp->spTrace()->addInitCb(&traceInit, __VlSymsp);
    traceRegister(tfp->spTrace());
}

void VBit_Generator_Circuit::traceInit(void* userp, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    if (!Verilated::calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
                        "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->module(vlSymsp->name());
    tracep->scopeEscape(' ');
    VBit_Generator_Circuit::traceInitTop(vlSymsp, tracep);
    tracep->scopeEscape('.');
}

//======================


void VBit_Generator_Circuit::traceInitTop(void* userp, VerilatedVcd* tracep) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlTOPp->traceInitSub0(userp, tracep);
    }
}

void VBit_Generator_Circuit::traceInitSub0(void* userp, VerilatedVcd* tracep) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    const int c = vlSymsp->__Vm_baseCode;
    if (false && tracep && c) {}  // Prevent unused
    // Body
    {
        tracep->declBit(c+9,"clk", false,-1);
        tracep->declBit(c+10,"rst_n", false,-1);
        tracep->declBus(c+11,"seed_in", false,-1, 11,0);
        tracep->declBus(c+12,"par_in", false,-1, 11,0);
        tracep->declBus(c+13,"mode", false,-1, 1,0);
        tracep->declBit(c+14,"lfsr_en", false,-1);
        tracep->declBit(c+15,"count_en", false,-1);
        tracep->declBit(c+16,"on_switch", false,-1);
        tracep->declBus(c+17,"total_count", false,-1, 11,0);
        tracep->declBit(c+9,"Bit_Generator_Circuit clk", false,-1);
        tracep->declBit(c+10,"Bit_Generator_Circuit rst_n", false,-1);
        tracep->declBus(c+11,"Bit_Generator_Circuit seed_in", false,-1, 11,0);
        tracep->declBus(c+12,"Bit_Generator_Circuit par_in", false,-1, 11,0);
        tracep->declBus(c+13,"Bit_Generator_Circuit mode", false,-1, 1,0);
        tracep->declBit(c+14,"Bit_Generator_Circuit lfsr_en", false,-1);
        tracep->declBit(c+15,"Bit_Generator_Circuit count_en", false,-1);
        tracep->declBit(c+16,"Bit_Generator_Circuit on_switch", false,-1);
        tracep->declBus(c+17,"Bit_Generator_Circuit total_count", false,-1, 11,0);
        tracep->declBus(c+1,"Bit_Generator_Circuit lfsr_out", false,-1, 11,0);
        tracep->declBus(c+2,"Bit_Generator_Circuit shift_reg_out", false,-1, 11,0);
        tracep->declBit(c+3,"Bit_Generator_Circuit comp_result", false,-1);
        tracep->declBit(c+9,"Bit_Generator_Circuit u_lfsr clk", false,-1);
        tracep->declBit(c+10,"Bit_Generator_Circuit u_lfsr rst_n", false,-1);
        tracep->declBit(c+14,"Bit_Generator_Circuit u_lfsr en", false,-1);
        tracep->declBus(c+11,"Bit_Generator_Circuit u_lfsr seed_in", false,-1, 11,0);
        tracep->declBus(c+1,"Bit_Generator_Circuit u_lfsr seed_mem", false,-1, 11,0);
        tracep->declBit(c+4,"Bit_Generator_Circuit u_lfsr feedback", false,-1);
        tracep->declBit(c+9,"Bit_Generator_Circuit sr clk", false,-1);
        tracep->declBit(c+18,"Bit_Generator_Circuit sr reset", false,-1);
        tracep->declBus(c+13,"Bit_Generator_Circuit sr mode", false,-1, 1,0);
        tracep->declBit(c+20,"Bit_Generator_Circuit sr serial_in", false,-1);
        tracep->declBus(c+12,"Bit_Generator_Circuit sr parallel_in", false,-1, 11,0);
        tracep->declBus(c+2,"Bit_Generator_Circuit sr q", false,-1, 11,0);
        tracep->declBus(c+1,"Bit_Generator_Circuit comp A", false,-1, 11,0);
        tracep->declBus(c+2,"Bit_Generator_Circuit comp B", false,-1, 11,0);
        tracep->declBit(c+3,"Bit_Generator_Circuit comp x", false,-1);
        tracep->declBus(c+5,"Bit_Generator_Circuit comp GL_bits", false,-1, 11,0);
        tracep->declBus(c+6,"Bit_Generator_Circuit comp GR_bits", false,-1, 11,0);
        tracep->declBus(c+7,"Bit_Generator_Circuit comp GL_index", false,-1, 3,0);
        tracep->declBus(c+8,"Bit_Generator_Circuit comp GR_index", false,-1, 3,0);
        tracep->declBus(c+5,"Bit_Generator_Circuit comp pe1 in", false,-1, 11,0);
        tracep->declBus(c+7,"Bit_Generator_Circuit comp pe1 out", false,-1, 3,0);
        tracep->declBus(c+21,"Bit_Generator_Circuit comp pe1 i", false,-1, 31,0);
        tracep->declBus(c+6,"Bit_Generator_Circuit comp pe2 in", false,-1, 11,0);
        tracep->declBus(c+8,"Bit_Generator_Circuit comp pe2 out", false,-1, 3,0);
        tracep->declBus(c+21,"Bit_Generator_Circuit comp pe2 i", false,-1, 31,0);
        tracep->declBit(c+9,"Bit_Generator_Circuit final_counter clk", false,-1);
        tracep->declBit(c+10,"Bit_Generator_Circuit final_counter rst_n", false,-1);
        tracep->declBit(c+16,"Bit_Generator_Circuit final_counter on_switch", false,-1);
        tracep->declBit(c+19,"Bit_Generator_Circuit final_counter count_en", false,-1);
        tracep->declBus(c+17,"Bit_Generator_Circuit final_counter count", false,-1, 11,0);
    }
}

void VBit_Generator_Circuit::traceRegister(VerilatedVcd* tracep) {
    // Body
    {
        tracep->addFullCb(&traceFullTop0, __VlSymsp);
        tracep->addChgCb(&traceChgTop0, __VlSymsp);
        tracep->addCleanupCb(&traceCleanup, __VlSymsp);
    }
}

void VBit_Generator_Circuit::traceFullTop0(void* userp, VerilatedVcd* tracep) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    {
        vlTOPp->traceFullSub0(userp, tracep);
    }
}

void VBit_Generator_Circuit::traceFullSub0(void* userp, VerilatedVcd* tracep) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = static_cast<VBit_Generator_Circuit__Syms*>(userp);
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    vluint32_t* const oldp = tracep->oldp(vlSymsp->__Vm_baseCode);
    if (false && oldp) {}  // Prevent unused
    // Body
    {
        tracep->fullSData(oldp+1,(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out),12);
        tracep->fullSData(oldp+2,(vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out),12);
        tracep->fullBit(oldp+3,(((IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index) 
                                 <= (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index))));
        tracep->fullBit(oldp+4,((1U & (((((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                          >> 0xbU) 
                                         ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                            >> 6U)) 
                                        ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                           >> 3U)) 
                                       ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                          >> 2U)))));
        tracep->fullSData(oldp+5,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits),12);
        tracep->fullSData(oldp+6,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits),12);
        tracep->fullCData(oldp+7,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index),4);
        tracep->fullCData(oldp+8,(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index),4);
        tracep->fullBit(oldp+9,(vlTOPp->clk));
        tracep->fullBit(oldp+10,(vlTOPp->rst_n));
        tracep->fullSData(oldp+11,(vlTOPp->seed_in),12);
        tracep->fullSData(oldp+12,(vlTOPp->par_in),12);
        tracep->fullCData(oldp+13,(vlTOPp->mode),2);
        tracep->fullBit(oldp+14,(vlTOPp->lfsr_en));
        tracep->fullBit(oldp+15,(vlTOPp->count_en));
        tracep->fullBit(oldp+16,(vlTOPp->on_switch));
        tracep->fullSData(oldp+17,(vlTOPp->total_count),12);
        tracep->fullBit(oldp+18,((1U & (~ (IData)(vlTOPp->rst_n)))));
        tracep->fullBit(oldp+19,(((IData)(vlTOPp->count_en) 
                                  & ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index) 
                                     <= (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index)))));
        tracep->fullBit(oldp+20,(0U));
        tracep->fullIData(oldp+21,(0xcU),32);
    }
}
