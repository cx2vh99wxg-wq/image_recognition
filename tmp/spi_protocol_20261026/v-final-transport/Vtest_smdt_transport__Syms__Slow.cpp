// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vtest_smdt_transport__pch.h"

Vtest_smdt_transport__Syms::Vtest_smdt_transport__Syms(VerilatedContext* contextp, const char* namep, Vtest_smdt_transport* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup top module instance
    , TOP{this, namep}
{
    // Check resources
    Verilated::stackCheck(342);
    // Setup sub module instances
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-9);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    // Setup scopes
}

Vtest_smdt_transport__Syms::~Vtest_smdt_transport__Syms() {
    // Tear down scopes
    // Tear down sub module instances
}
