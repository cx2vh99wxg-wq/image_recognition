// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_lane_permission.h for the primary calling header

#ifndef VERILATED_VTEST_LANE_PERMISSION___024ROOT_H_
#define VERILATED_VTEST_LANE_PERMISSION___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_lane_permission__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_lane_permission___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_lane_permission__DOT__clk;
    CData/*0:0*/ test_lane_permission__DOT__rst_n;
    CData/*7:0*/ test_lane_permission__DOT__permission;
    CData/*3:0*/ test_lane_permission__DOT__hmi;
    CData/*3:0*/ test_lane_permission__DOT__fspi;
    CData/*0:0*/ test_lane_permission__DOT__stop;
    CData/*0:0*/ test_lane_permission__DOT__go;
    CData/*0:0*/ test_lane_permission__DOT__back;
    CData/*0:0*/ test_lane_permission__DOT__left;
    CData/*0:0*/ test_lane_permission__DOT__right;
    CData/*7:0*/ test_lane_permission__DOT__guard__DOT__meta;
    CData/*7:0*/ test_lane_permission__DOT__guard__DOT__synced;
    CData/*7:0*/ test_lane_permission__DOT__guard__DOT__candidate;
    CData/*3:0*/ test_lane_permission__DOT__guard__DOT__stable;
    CData/*0:0*/ test_lane_permission__DOT__guard__DOT__heartbeat;
    CData/*0:0*/ test_lane_permission__DOT__guard__DOT__alive;
    CData/*1:0*/ test_lane_permission__DOT__guard__DOT__deny;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_traction;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_steering;
    CData/*1:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched;
    CData/*3:0*/ test_lane_permission__DOT__arbiter__DOT__previous_hmi_code;
    CData/*2:0*/ test_lane_permission__DOT__arbiter__DOT__traction_stable_count;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__traction_press_consumed;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_code_changed;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_traction_hold;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_steering_hold;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_lane_permission__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_lane_permission__DOT__rst_n__0;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    IData/*24:0*/ test_lane_permission__DOT__guard__DOT__age;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h15e6f7c2__0;

    // INTERNAL VARIABLES
    Vtest_lane_permission__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_lane_permission___024root(Vtest_lane_permission__Syms* symsp, const char* namep);
    ~Vtest_lane_permission___024root();
    VL_UNCOPYABLE(Vtest_lane_permission___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
