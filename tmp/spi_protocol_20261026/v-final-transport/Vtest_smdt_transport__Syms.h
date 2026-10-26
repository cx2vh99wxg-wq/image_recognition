// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTEST_SMDT_TRANSPORT__SYMS_H_
#define VERILATED_VTEST_SMDT_TRANSPORT__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtest_smdt_transport.h"

// INCLUDE MODULE CLASSES
#include "Vtest_smdt_transport___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vtest_smdt_transport__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtest_smdt_transport* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtest_smdt_transport___024root TOP;

    // CONSTRUCTORS
    Vtest_smdt_transport__Syms(VerilatedContext* contextp, const char* namep, Vtest_smdt_transport* modelp);
    ~Vtest_smdt_transport__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
