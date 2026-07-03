// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "VShift_Register__pch.h"

//============================================================
// Constructors

VShift_Register::VShift_Register(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new VShift_Register__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , reset{vlSymsp->TOP.reset}
    , mode{vlSymsp->TOP.mode}
    , serial_in{vlSymsp->TOP.serial_in}
    , parallel_in{vlSymsp->TOP.parallel_in}
    , q{vlSymsp->TOP.q}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

VShift_Register::VShift_Register(const char* _vcname__)
    : VShift_Register(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

VShift_Register::~VShift_Register() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void VShift_Register___024root___eval_debug_assertions(VShift_Register___024root* vlSelf);
#endif  // VL_DEBUG
void VShift_Register___024root___eval_static(VShift_Register___024root* vlSelf);
void VShift_Register___024root___eval_initial(VShift_Register___024root* vlSelf);
void VShift_Register___024root___eval_settle(VShift_Register___024root* vlSelf);
void VShift_Register___024root___eval(VShift_Register___024root* vlSelf);

void VShift_Register::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate VShift_Register::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    VShift_Register___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        VShift_Register___024root___eval_static(&(vlSymsp->TOP));
        VShift_Register___024root___eval_initial(&(vlSymsp->TOP));
        VShift_Register___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    VShift_Register___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool VShift_Register::eventsPending() { return false; }

uint64_t VShift_Register::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* VShift_Register::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void VShift_Register___024root___eval_final(VShift_Register___024root* vlSelf);

VL_ATTR_COLD void VShift_Register::final() {
    VShift_Register___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* VShift_Register::hierName() const { return vlSymsp->name(); }
const char* VShift_Register::modelName() const { return "VShift_Register"; }
unsigned VShift_Register::threads() const { return 1; }
void VShift_Register::prepareClone() const { contextp()->prepareClone(); }
void VShift_Register::atClone() const {
    contextp()->threadPoolpOnClone();
}
