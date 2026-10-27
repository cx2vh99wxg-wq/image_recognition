// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_lane_permission.h for the primary calling header

#include "Vtest_lane_permission__pch.h"

void Vtest_lane_permission___024root___ctor_var_reset(Vtest_lane_permission___024root* vlSelf);

Vtest_lane_permission___024root::Vtest_lane_permission___024root(Vtest_lane_permission__Syms* symsp, const char* namep)
    : __VdlySched{*symsp->_vm_contextp__}
 {
    vlSymsp = symsp;
    vlNamep = strdup(namep);
    // Reset structure values
    Vtest_lane_permission___024root___ctor_var_reset(this);
}

void Vtest_lane_permission___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vtest_lane_permission___024root::~Vtest_lane_permission___024root() {
    VL_DO_DANGLING(std::free(const_cast<char*>(vlNamep)), vlNamep);
}
