// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_fspi_control.h for the primary calling header

#include "Vtest_fspi_control__pch.h"

void Vtest_fspi_control___024root___ctor_var_reset(Vtest_fspi_control___024root* vlSelf);

Vtest_fspi_control___024root::Vtest_fspi_control___024root(Vtest_fspi_control__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_fspi_control___024root___ctor_var_reset(this);
}

void Vtest_fspi_control___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_fspi_control___024root::~Vtest_fspi_control___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
