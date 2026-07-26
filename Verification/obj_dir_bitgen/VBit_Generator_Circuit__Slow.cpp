// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See VBit_Generator_Circuit.h for the primary calling header

#include "VBit_Generator_Circuit.h"
#include "VBit_Generator_Circuit__Syms.h"

//==========

VL_CTOR_IMP(VBit_Generator_Circuit) {
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = __VlSymsp = new VBit_Generator_Circuit__Syms(this, name());
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Reset internal values
    
    // Reset structure values
    _ctor_var_reset();
}

void VBit_Generator_Circuit::__Vconfigure(VBit_Generator_Circuit__Syms* vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->__VlSymsp = vlSymsp;
    if (false && this->__VlSymsp) {}  // Prevent unused
    Verilated::timeunit(-12);
    Verilated::timeprecision(-12);
}

VBit_Generator_Circuit::~VBit_Generator_Circuit() {
    VL_DO_CLEAR(delete __VlSymsp, __VlSymsp = NULL);
}

void VBit_Generator_Circuit::_settle__TOP__2(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_settle__TOP__2\n"); );
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->Bit_Generator_Circuit__DOT__u_lfsr__DOT__feedback 
        = (1U & (((((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                    >> 0xbU) ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                >> 6U)) ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                           >> 3U)) 
                 ^ ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                    >> 2U)));
    vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits 
        = ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
           & (~ (IData)(vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out)));
    vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits 
        = ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out) 
           & (~ (IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out)));
    vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 0U;
    if ((1U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 1U;
    }
    if ((2U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 2U;
    }
    if ((4U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 3U;
    }
    if ((8U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 4U;
    }
    if ((0x10U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 5U;
    }
    if ((0x20U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 6U;
    }
    if ((0x40U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 7U;
    }
    if ((0x80U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 8U;
    }
    if ((0x100U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 9U;
    }
    if ((0x200U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 0xaU;
    }
    if ((0x400U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 0xbU;
    }
    if ((0x800U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index = 0xcU;
    }
    vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 0U;
    if ((1U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 1U;
    }
    if ((2U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 2U;
    }
    if ((4U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 3U;
    }
    if ((8U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 4U;
    }
    if ((0x10U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 5U;
    }
    if ((0x20U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 6U;
    }
    if ((0x40U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 7U;
    }
    if ((0x80U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 8U;
    }
    if ((0x100U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 9U;
    }
    if ((0x200U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 0xaU;
    }
    if ((0x400U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 0xbU;
    }
    if ((0x800U & (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_bits))) {
        vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index = 0xcU;
    }
}

void VBit_Generator_Circuit::_eval_initial(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_eval_initial\n"); );
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->__Vclklast__TOP__clk = vlTOPp->clk;
    vlTOPp->__Vclklast__TOP__rst_n = vlTOPp->rst_n;
}

void VBit_Generator_Circuit::final() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::final\n"); );
    // Variables
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = this->__VlSymsp;
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
}

void VBit_Generator_Circuit::_eval_settle(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_eval_settle\n"); );
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->_settle__TOP__2(vlSymsp);
    vlTOPp->__Vm_traceActivity[1U] = 1U;
    vlTOPp->__Vm_traceActivity[0U] = 1U;
}

void VBit_Generator_Circuit::_ctor_var_reset() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_ctor_var_reset\n"); );
    // Body
    clk = VL_RAND_RESET_I(1);
    rst_n = VL_RAND_RESET_I(1);
    seed_in = VL_RAND_RESET_I(12);
    par_in = VL_RAND_RESET_I(12);
    mode = VL_RAND_RESET_I(2);
    lfsr_en = VL_RAND_RESET_I(1);
    count_en = VL_RAND_RESET_I(1);
    on_switch = VL_RAND_RESET_I(1);
    total_count = VL_RAND_RESET_I(12);
    Bit_Generator_Circuit__DOT__lfsr_out = VL_RAND_RESET_I(12);
    Bit_Generator_Circuit__DOT__shift_reg_out = VL_RAND_RESET_I(12);
    Bit_Generator_Circuit__DOT__u_lfsr__DOT__feedback = VL_RAND_RESET_I(1);
    Bit_Generator_Circuit__DOT__comp__DOT__GL_bits = VL_RAND_RESET_I(12);
    Bit_Generator_Circuit__DOT__comp__DOT__GR_bits = VL_RAND_RESET_I(12);
    Bit_Generator_Circuit__DOT__comp__DOT__GL_index = VL_RAND_RESET_I(4);
    Bit_Generator_Circuit__DOT__comp__DOT__GR_index = VL_RAND_RESET_I(4);
    { int __Vi0=0; for (; __Vi0<2; ++__Vi0) {
            __Vm_traceActivity[__Vi0] = VL_RAND_RESET_I(1);
    }}
}
