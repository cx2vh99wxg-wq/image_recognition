// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_direction_arbiter.h for the primary calling header

#include "Vtest_direction_arbiter__pch.h"

void Vtest_direction_arbiter___024root___ctor_var_reset(Vtest_direction_arbiter___024root* vlSelf);

Vtest_direction_arbiter___024root::Vtest_direction_arbiter___024root(Vtest_direction_arbiter__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_direction_arbiter___024root___ctor_var_reset(this);
}

void Vtest_direction_arbiter___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_direction_arbiter___024root::~Vtest_direction_arbiter___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
