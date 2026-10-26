// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTEST_HMI_AUTO_BRAKE_SWITCH__SYMS_H_
#define VERILATED_VTEST_HMI_AUTO_BRAKE_SWITCH__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtest_hmi_auto_brake_switch.h"

// INCLUDE MODULE CLASSES
#include "Vtest_hmi_auto_brake_switch___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_auto_brake_switch__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtest_hmi_auto_brake_switch* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtest_hmi_auto_brake_switch___024root TOP;

    // CONSTRUCTORS
    Vtest_hmi_auto_brake_switch__Syms(VerilatedContext* contextp, const char* namep, Vtest_hmi_auto_brake_switch* modelp);
    ~Vtest_hmi_auto_brake_switch__Syms();

    // METHODS
    const char* name() const { return TOP.vlNamep; }
};

#endif  // guard
