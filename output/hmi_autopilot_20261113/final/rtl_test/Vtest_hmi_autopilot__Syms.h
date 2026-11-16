// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTEST_HMI_AUTOPILOT__SYMS_H_
#define VERILATED_VTEST_HMI_AUTOPILOT__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtest_hmi_autopilot.h"

// INCLUDE MODULE CLASSES
#include "Vtest_hmi_autopilot___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_autopilot__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtest_hmi_autopilot* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtest_hmi_autopilot___024root  TOP;

    // CONSTRUCTORS
    Vtest_hmi_autopilot__Syms(VerilatedContext* contextp, const char* namep, Vtest_hmi_autopilot* modelp);
    ~Vtest_hmi_autopilot__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
