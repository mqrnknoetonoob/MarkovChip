// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "VLFSR__pch.h"

//============================================================
// Constructors

VLFSR::VLFSR(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new VLFSR__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , rst_n{vlSymsp->TOP.rst_n}
    , en{vlSymsp->TOP.en}
    , q{vlSymsp->TOP.q}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

VLFSR::VLFSR(const char* _vcname__)
    : VLFSR(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

VLFSR::~VLFSR() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void VLFSR___024root___eval_debug_assertions(VLFSR___024root* vlSelf);
#endif  // VL_DEBUG
void VLFSR___024root___eval_static(VLFSR___024root* vlSelf);
void VLFSR___024root___eval_initial(VLFSR___024root* vlSelf);
void VLFSR___024root___eval_settle(VLFSR___024root* vlSelf);
void VLFSR___024root___eval(VLFSR___024root* vlSelf);

void VLFSR::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate VLFSR::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    VLFSR___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        VLFSR___024root___eval_static(&(vlSymsp->TOP));
        VLFSR___024root___eval_initial(&(vlSymsp->TOP));
        VLFSR___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    VLFSR___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool VLFSR::eventsPending() { return false; }

uint64_t VLFSR::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* VLFSR::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void VLFSR___024root___eval_final(VLFSR___024root* vlSelf);

VL_ATTR_COLD void VLFSR::final() {
    VLFSR___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* VLFSR::hierName() const { return vlSymsp->name(); }
const char* VLFSR::modelName() const { return "VLFSR"; }
unsigned VLFSR::threads() const { return 1; }
void VLFSR::prepareClone() const { contextp()->prepareClone(); }
void VLFSR::atClone() const {
    contextp()->threadPoolpOnClone();
}
