// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_remote_stop_controller.h for the primary calling header

#ifndef VERILATED_VTEST_REMOTE_STOP_CONTROLLER___024ROOT_H_
#define VERILATED_VTEST_REMOTE_STOP_CONTROLLER___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_remote_stop_controller__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_remote_stop_controller___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_remote_stop_controller__DOT__clk;
    CData/*0:0*/ test_remote_stop_controller__DOT__rst_n;
    CData/*0:0*/ test_remote_stop_controller__DOT__brake_request;
    CData/*0:0*/ test_remote_stop_controller__DOT__rearm_command;
    CData/*0:0*/ test_remote_stop_controller__DOT__fspi_directions_idle;
    CData/*0:0*/ test_remote_stop_controller__DOT__stop_pressed;
    CData/*0:0*/ test_remote_stop_controller__DOT__movement_blocked;
    CData/*3:0*/ test_remote_stop_controller__DOT__hmi_raw_code;
    CData/*3:0*/ test_remote_stop_controller__DOT__hmi_code_sync;
    CData/*0:0*/ test_remote_stop_controller__DOT__hmi_rearm_filtered;
    CData/*0:0*/ test_remote_stop_controller__DOT__cdc_stop_pressed;
    CData/*3:0*/ test_remote_stop_controller__DOT__hmi_cdc__DOT__code_meta;
    CData/*2:0*/ test_remote_stop_controller__DOT__hmi_cdc__DOT__e_stable_count;
    CData/*0:0*/ test_remote_stop_controller__DOT__dut__DOT__brake_latched;
    CData/*0:0*/ test_remote_stop_controller__DOT__dut__DOT__brake_event_armed;
    CData/*0:0*/ test_remote_stop_controller__DOT__dut__DOT__rearm_previous;
    CData/*0:0*/ test_remote_stop_controller__DOT__dut__DOT__fspi_armed;
    CData/*0:0*/ test_remote_stop_controller__DOT__cdc_dut__DOT__brake_latched;
    CData/*0:0*/ test_remote_stop_controller__DOT__cdc_dut__DOT__brake_event_armed;
    CData/*0:0*/ test_remote_stop_controller__DOT__cdc_dut__DOT__rearm_previous;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_remote_stop_controller__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_remote_stop_controller__DOT__rst_n__0;
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
    VlTriggerScheduler __VtrigSched_h0eaceda2__0;
    VlTriggerScheduler __VtrigSched_h0eacea68__0;

    // INTERNAL VARIABLES
    Vtest_remote_stop_controller__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_remote_stop_controller___024root(Vtest_remote_stop_controller__Syms* symsp, const char* namep);
    ~Vtest_remote_stop_controller___024root();
    VL_UNCOPYABLE(Vtest_remote_stop_controller___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
