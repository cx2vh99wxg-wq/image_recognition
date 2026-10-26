// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_uart.h for the primary calling header

#include "Vtest_hmi_uart__pch.h"

void Vtest_hmi_uart___024root___ctor_var_reset(Vtest_hmi_uart___024root* vlSelf);

Vtest_hmi_uart___024root::Vtest_hmi_uart___024root(Vtest_hmi_uart__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_hmi_uart___024root___ctor_var_reset(this);
}

void Vtest_hmi_uart___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_hmi_uart___024root::~Vtest_hmi_uart___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
