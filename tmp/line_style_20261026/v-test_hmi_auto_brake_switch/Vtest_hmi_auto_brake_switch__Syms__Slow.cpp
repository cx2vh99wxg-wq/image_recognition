// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vtest_hmi_auto_brake_switch__pch.h"

Vtest_hmi_auto_brake_switch__Syms::Vtest_hmi_auto_brake_switch__Syms(VerilatedContext* contextp, const char* namep, Vtest_hmi_auto_brake_switch* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup top module instance
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(12524);
    // Setup sub module instances
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    // Setup scopes
}

Vtest_hmi_auto_brake_switch__Syms::~Vtest_hmi_auto_brake_switch__Syms() {
    // Tear down scopes
    // Tear down sub module instances
}
