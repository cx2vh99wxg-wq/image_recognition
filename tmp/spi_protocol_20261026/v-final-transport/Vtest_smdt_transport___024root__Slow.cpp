// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_smdt_transport.h for the primary calling header

#include "Vtest_smdt_transport__pch.h"

void Vtest_smdt_transport___024root___ctor_var_reset(Vtest_smdt_transport___024root* vlSelf);

Vtest_smdt_transport___024root::Vtest_smdt_transport___024root(Vtest_smdt_transport__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_smdt_transport___024root___ctor_var_reset(this);
}

void Vtest_smdt_transport___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_smdt_transport___024root::~Vtest_smdt_transport___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
