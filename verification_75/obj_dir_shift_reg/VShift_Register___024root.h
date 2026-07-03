// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See VShift_Register.h for the primary calling header

#ifndef VERILATED_VSHIFT_REGISTER___024ROOT_H_
#define VERILATED_VSHIFT_REGISTER___024ROOT_H_  // guard

#include "verilated.h"


class VShift_Register__Syms;

class alignas(VL_CACHE_LINE_BYTES) VShift_Register___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(reset,0,0);
    VL_IN8(mode,1,0);
    VL_IN8(serial_in,0,0);
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__reset__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(parallel_in,11,0);
    VL_OUT16(q,11,0);
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<2> __VactTriggered;
    VlTriggerVec<2> __VnbaTriggered;

    // INTERNAL VARIABLES
    VShift_Register__Syms* const vlSymsp;

    // CONSTRUCTORS
    VShift_Register___024root(VShift_Register__Syms* symsp, const char* v__name);
    ~VShift_Register___024root();
    VL_UNCOPYABLE(VShift_Register___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
