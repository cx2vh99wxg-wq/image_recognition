// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_direction_arbiter.h for the primary calling header

#ifndef VERILATED_VTEST_DIRECTION_ARBITER___024ROOT_H_
#define VERILATED_VTEST_DIRECTION_ARBITER___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_direction_arbiter__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_direction_arbiter___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_direction_arbiter__DOT__clk;
    CData/*0:0*/ test_direction_arbiter__DOT__rst_n;
    CData/*3:0*/ test_direction_arbiter__DOT__hmi_code;
    CData/*3:0*/ test_direction_arbiter__DOT__fspi_released;
    CData/*0:0*/ test_direction_arbiter__DOT__fspi_direction_armed;
    CData/*0:0*/ test_direction_arbiter__DOT__movement_blocked;
    CData/*3:0*/ test_direction_arbiter__DOT__released;
    CData/*0:0*/ test_direction_arbiter__DOT__dut__DOT__hmi_traction;
    CData/*0:0*/ test_direction_arbiter__DOT__dut__DOT__hmi_steering;
    CData/*0:0*/ test_direction_arbiter__DOT__dut__DOT__hmi_traction_hold;
    CData/*0:0*/ test_direction_arbiter__DOT__dut__DOT__hmi_steering_hold;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_direction_arbiter__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_direction_arbiter__DOT__rst_n__0;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h1e9f5e4b__0;
    VlTriggerScheduler __VtrigSched_h1e9f5f27__0;

    // INTERNAL VARIABLES
    Vtest_direction_arbiter__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_direction_arbiter___024root(Vtest_direction_arbiter__Syms* symsp, const char* namep);
    ~Vtest_direction_arbiter___024root();
    VL_UNCOPYABLE(Vtest_direction_arbiter___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
