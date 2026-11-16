// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_hmi_autopilot.h for the primary calling header

#ifndef VERILATED_VTEST_HMI_AUTOPILOT___024ROOT_H_
#define VERILATED_VTEST_HMI_AUTOPILOT___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_hmi_autopilot__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_autopilot___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_hmi_autopilot__DOT__clk;
    CData/*0:0*/ test_hmi_autopilot__DOT__reset_n;
    CData/*7:0*/ test_hmi_autopilot__DOT__data;
    CData/*7:0*/ test_hmi_autopilot__DOT__heartbeat;
    CData/*0:0*/ test_hmi_autopilot__DOT__valid;
    CData/*0:0*/ test_hmi_autopilot__DOT__released;
    CData/*0:0*/ test_hmi_autopilot__DOT__blocked;
    CData/*0:0*/ test_hmi_autopilot__DOT__requested;
    CData/*1:0*/ test_hmi_autopilot__DOT__sequence_code;
    CData/*0:0*/ test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg;
    CData/*1:0*/ test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen;
    CData/*0:0*/ test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete;
    CData/*2:0*/ test_hmi_autopilot__DOT__gate__DOT__request_meta;
    CData/*2:0*/ test_hmi_autopilot__DOT__gate__DOT__request_sync;
    CData/*7:0*/ test_hmi_autopilot__DOT__gate__DOT__heartbeat_meta;
    CData/*7:0*/ test_hmi_autopilot__DOT__gate__DOT__heartbeat_sync;
    CData/*7:0*/ test_hmi_autopilot__DOT__gate__DOT__previous;
    CData/*5:0*/ test_hmi_autopilot__DOT__gate__DOT__age;
    CData/*0:0*/ test_hmi_autopilot__DOT__gate__DOT__accepted;
    CData/*0:0*/ test_hmi_autopilot__DOT__gate__DOT__consumed;
    CData/*2:0*/ test_hmi_autopilot__DOT__gate__DOT__last_request;
    CData/*0:0*/ test_hmi_autopilot__DOT__gate__DOT__approval_matches;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__reset_n__0;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    IData/*23:0*/ test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history;
    IData/*19:0*/ test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h1fa99d08__0;

    // INTERNAL VARIABLES
    Vtest_hmi_autopilot__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_hmi_autopilot___024root(Vtest_hmi_autopilot__Syms* symsp, const char* namep);
    ~Vtest_hmi_autopilot___024root();
    VL_UNCOPYABLE(Vtest_hmi_autopilot___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
