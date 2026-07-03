// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VSHIFT_REGISTER__SYMS_H_
#define VERILATED_VSHIFT_REGISTER__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "VShift_Register.h"

// INCLUDE MODULE CLASSES
#include "VShift_Register___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)VShift_Register__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    VShift_Register* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    VShift_Register___024root      TOP;

    // CONSTRUCTORS
    VShift_Register__Syms(VerilatedContext* contextp, const char* namep, VShift_Register* modelp);
    ~VShift_Register__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
