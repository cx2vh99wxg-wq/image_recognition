// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTEST_SMDT_PREFIX__SYMS_H_
#define VERILATED_VTEST_SMDT_PREFIX__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtest_smdt_prefix.h"

// INCLUDE MODULE CLASSES
#include "Vtest_smdt_prefix___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vtest_smdt_prefix__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtest_smdt_prefix* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtest_smdt_prefix___024root    TOP;

    // CONSTRUCTORS
    Vtest_smdt_prefix__Syms(VerilatedContext* contextp, const char* namep, Vtest_smdt_prefix* modelp);
    ~Vtest_smdt_prefix__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
