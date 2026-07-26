// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See VBit_Generator_Circuit.h for the primary calling header

#include "VBit_Generator_Circuit.h"
#include "VBit_Generator_Circuit__Syms.h"

//==========

void VBit_Generator_Circuit::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate VBit_Generator_Circuit::eval\n"); );
    VBit_Generator_Circuit__Syms* __restrict vlSymsp = this->__VlSymsp;  // Setup global symbol table
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
#ifdef VL_DEBUG
    // Debug assertions
    _eval_debug_assertions();
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        vlSymsp->__Vm_activity = true;
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("../RTL/Bit_Generator_Circuit.v", 1, "",
                "Verilated model didn't converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

void VBit_Generator_Circuit::_eval_initial_loop(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    _eval_initial(vlSymsp);
    vlSymsp->__Vm_activity = true;
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        _eval_settle(vlSymsp);
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("../RTL/Bit_Generator_Circuit.v", 1, "",
                "Verilated model didn't DC converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

VL_INLINE_OPT void VBit_Generator_Circuit::_sequent__TOP__1(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_sequent__TOP__1\n"); );
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if (vlTOPp->rst_n) {
        if (vlTOPp->on_switch) {
            if (((IData)(vlTOPp->count_en) & ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GL_index) 
                                              <= (IData)(vlTOPp->Bit_Generator_Circuit__DOT__comp__DOT__GR_index)))) {
                vlTOPp->total_count = (0xfffU & ((IData)(1U) 
                                                 + (IData)(vlTOPp->total_count)));
            }
        } else {
            vlTOPp->total_count = 0U;
        }
    } else {
        vlTOPp->total_count = 0U;
    }
    if (vlTOPp->rst_n) {
        vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out 
            = ((0U == (IData)(vlTOPp->mode)) ? (IData)(vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out)
                : ((1U == (IData)(vlTOPp->mode)) ? 
                   (0x7ffU & ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out) 
                              >> 1U)) : ((2U == (IData)(vlTOPp->mode))
                                          ? (IData)(vlTOPp->par_in)
                                          : (IData)(vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out))));
        vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out 
            = ((IData)(vlTOPp->lfsr_en) ? ((0xffeU 
                                            & ((IData)(vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out) 
                                               << 1U)) 
                                           | (IData)(vlTOPp->Bit_Generator_Circuit__DOT__u_lfsr__DOT__feedback))
                : (IData)(vlTOPp->seed_in));
    } else {
        vlTOPp->Bit_Generator_Circuit__DOT__shift_reg_out = 0U;
        vlTOPp->Bit_Generator_Circuit__DOT__lfsr_out 
            = vlTOPp->seed_in;
    }
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

void VBit_Generator_Circuit::_eval(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_eval\n"); );
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if ((((IData)(vlTOPp->clk) & (~ (IData)(vlTOPp->__Vclklast__TOP__clk))) 
         | ((~ (IData)(vlTOPp->rst_n)) & (IData)(vlTOPp->__Vclklast__TOP__rst_n)))) {
        vlTOPp->_sequent__TOP__1(vlSymsp);
        vlTOPp->__Vm_traceActivity[1U] = 1U;
    }
    // Final
    vlTOPp->__Vclklast__TOP__clk = vlTOPp->clk;
    vlTOPp->__Vclklast__TOP__rst_n = vlTOPp->rst_n;
}

VL_INLINE_OPT QData VBit_Generator_Circuit::_change_request(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_change_request\n"); );
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    return (vlTOPp->_change_request_1(vlSymsp));
}

VL_INLINE_OPT QData VBit_Generator_Circuit::_change_request_1(VBit_Generator_Circuit__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_change_request_1\n"); );
    VBit_Generator_Circuit* const __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    // Change detection
    QData __req = false;  // Logically a bool
    return __req;
}

#ifdef VL_DEBUG
void VBit_Generator_Circuit::_eval_debug_assertions() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    VBit_Generator_Circuit::_eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((rst_n & 0xfeU))) {
        Verilated::overWidthError("rst_n");}
    if (VL_UNLIKELY((seed_in & 0xf000U))) {
        Verilated::overWidthError("seed_in");}
    if (VL_UNLIKELY((par_in & 0xf000U))) {
        Verilated::overWidthError("par_in");}
    if (VL_UNLIKELY((mode & 0xfcU))) {
        Verilated::overWidthError("mode");}
    if (VL_UNLIKELY((lfsr_en & 0xfeU))) {
        Verilated::overWidthError("lfsr_en");}
    if (VL_UNLIKELY((count_en & 0xfeU))) {
        Verilated::overWidthError("count_en");}
    if (VL_UNLIKELY((on_switch & 0xfeU))) {
        Verilated::overWidthError("on_switch");}
}
#endif  // VL_DEBUG
