// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See VShift_Register.h for the primary calling header

#include "VShift_Register__pch.h"
#include "VShift_Register___024root.h"

void VShift_Register___024root___eval_act(VShift_Register___024root* vlSelf) {
    (void)vlSelf;  // Prevent unused variable warning
    VShift_Register__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    VShift_Register___024root___eval_act\n"); );
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void VShift_Register___024root___nba_sequent__TOP__0(VShift_Register___024root* vlSelf);

void VShift_Register___024root___eval_nba(VShift_Register___024root* vlSelf) {
    (void)vlSelf;  // Prevent unused variable warning
    VShift_Register__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    VShift_Register___024root___eval_nba\n"); );
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VShift_Register___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void VShift_Register___024root___nba_sequent__TOP__0(VShift_Register___024root* vlSelf) {
    (void)vlSelf;  // Prevent unused variable warning
    VShift_Register__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    VShift_Register___024root___nba_sequent__TOP__0\n"); );
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.q = ((IData)(vlSelfRef.reset) ? 0U : 
                   ((0U == (IData)(vlSelfRef.mode))
                     ? (IData)(vlSelfRef.q) : ((1U 
                                                == (IData)(vlSelfRef.mode))
                                                ? (
                                                   ((IData)(vlSelfRef.serial_in) 
                                                    << 0xbU) 
                                                   | (0x7ffU 
                                                      & ((IData)(vlSelfRef.q) 
                                                         >> 1U)))
                                                : (
                                                   (2U 
                                                    == (IData)(vlSelfRef.mode))
                                                    ? (IData)(vlSelfRef.parallel_in)
                                                    : (IData)(vlSelfRef.q)))));
}

void VShift_Register___024root___eval_triggers__act(VShift_Register___024root* vlSelf);

bool VShift_Register___024root___eval_phase__act(VShift_Register___024root* vlSelf) {
    (void)vlSelf;  // Prevent unused variable warning
    VShift_Register__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    VShift_Register___024root___eval_phase__act\n"); );
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<2> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    VShift_Register___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        VShift_Register___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool VShift_Register___024root___eval_phase__nba(VShift_Register___024root* vlSelf) {
    (void)vlSelf;  // Prevent unused variable warning
    VShift_Register__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    VShift_Register___024root___eval_phase__nba\n"); );
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        VShift_Register___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void VShift_Register___024root___dump_triggers__nba(VShift_Register___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void VShift_Register___024root___dump_triggers__act(VShift_Register___024root* vlSelf);
#endif  // VL_DEBUG

void VShift_Register___024root___eval(VShift_Register___024root* vlSelf) {
    (void)vlSelf;  // Prevent unused variable warning
    VShift_Register__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    VShift_Register___024root___eval\n"); );
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
            VShift_Register___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("RTL/Shift_Register.v", 1, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY((0x64U < vlSelfRef.__VactIterCount))) {
#ifdef VL_DEBUG
                VShift_Register___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("RTL/Shift_Register.v", 1, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (VShift_Register___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (VShift_Register___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void VShift_Register___024root___eval_debug_assertions(VShift_Register___024root* vlSelf) {
    (void)vlSelf;  // Prevent unused variable warning
    VShift_Register__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    VShift_Register___024root___eval_debug_assertions\n"); );
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY((vlSelfRef.clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelfRef.reset & 0xfeU))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY((vlSelfRef.mode & 0xfcU))) {
        Verilated::overWidthError("mode");}
    if (VL_UNLIKELY((vlSelfRef.serial_in & 0xfeU))) {
        Verilated::overWidthError("serial_in");}
    if (VL_UNLIKELY((vlSelfRef.parallel_in & 0xf000U))) {
        Verilated::overWidthError("parallel_in");}
}
#endif  // VL_DEBUG
