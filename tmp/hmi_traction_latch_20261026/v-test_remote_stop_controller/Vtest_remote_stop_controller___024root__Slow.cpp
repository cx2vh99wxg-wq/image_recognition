// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_remote_stop_controller.h for the primary calling header

#include "Vtest_remote_stop_controller__pch.h"

void Vtest_remote_stop_controller___024root___ctor_var_reset(Vtest_remote_stop_controller___024root* vlSelf);

Vtest_remote_stop_controller___024root::Vtest_remote_stop_controller___024root(Vtest_remote_stop_controller__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_remote_stop_controller___024root___ctor_var_reset(this);
}

void Vtest_remote_stop_controller___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_remote_stop_controller___024root::~Vtest_remote_stop_controller___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
