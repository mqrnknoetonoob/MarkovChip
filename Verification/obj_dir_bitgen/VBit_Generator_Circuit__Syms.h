// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef _VBIT_GENERATOR_CIRCUIT__SYMS_H_
#define _VBIT_GENERATOR_CIRCUIT__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODULE CLASSES
#include "VBit_Generator_Circuit.h"

// SYMS CLASS
class VBit_Generator_Circuit__Syms : public VerilatedSyms {
  public:
    
    // LOCAL STATE
    const char* __Vm_namep;
    bool __Vm_activity;  ///< Used by trace routines to determine change occurred
    uint32_t __Vm_baseCode;  ///< Used by trace routines when tracing multiple models
    bool __Vm_didInit;
    
    // SUBCELL STATE
    VBit_Generator_Circuit*        TOPp;
    
    // CREATORS
    VBit_Generator_Circuit__Syms(VBit_Generator_Circuit* topp, const char* namep);
    ~VBit_Generator_Circuit__Syms() {}
    
    // METHODS
    inline const char* name() { return __Vm_namep; }
    
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
