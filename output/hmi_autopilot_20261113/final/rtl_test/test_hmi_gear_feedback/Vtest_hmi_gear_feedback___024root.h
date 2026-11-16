// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_hmi_gear_feedback.h for the primary calling header

#ifndef VERILATED_VTEST_HMI_GEAR_FEEDBACK___024ROOT_H_
#define VERILATED_VTEST_HMI_GEAR_FEEDBACK___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_hmi_gear_feedback__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_gear_feedback___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_hmi_gear_feedback__DOT__clk;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__rst_n;
    CData/*3:0*/ test_hmi_gear_feedback__DOT__status;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__slider_present;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__txd;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__valid;
    CData/*7:0*/ test_hmi_gear_feedback__DOT__data;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__autopilot_active;
    CData/*3:0*/ test_hmi_gear_feedback__DOT__dut__DOT__gear_meta;
    CData/*3:0*/ test_hmi_gear_feedback__DOT__dut__DOT__gear_sync;
    CData/*1:0*/ test_hmi_gear_feedback__DOT__dut__DOT__state;
    CData/*5:0*/ test_hmi_gear_feedback__DOT__dut__DOT__index;
    CData/*7:0*/ test_hmi_gear_feedback__DOT__dut__DOT__gear_character;
    CData/*7:0*/ test_hmi_gear_feedback__DOT__dut__DOT__tx_data;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__dut__DOT__tx_enable;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__dut__DOT__autopilot_meta;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__dut__DOT__autopilot_sync;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__dut__DOT__tx_busy;
    CData/*7:0*/ test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t;
    CData/*3:0*/ test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d0;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d1;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2;
    CData/*0:0*/ test_hmi_gear_feedback__DOT__receiver__DOT__rx_flag;
    CData/*3:0*/ test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt;
    CData/*7:0*/ test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t;
    CData/*7:0*/ __Vdly__test_hmi_gear_feedback__DOT__data;
    CData/*3:0*/ __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__rst_n__0;
    CData/*0:0*/ __Vtrigprevexpr_hcbfad22d__1;
    CData/*0:0*/ __Vtrigprevexpr_hcbfae7c2__1;
    CData/*0:0*/ __Vtrigprevexpr_hcc0e817f__1;
    CData/*0:0*/ __Vtrigprevexpr_hcbfcd82a__1;
    CData/*0:0*/ __Vtrigprevexpr_hcc309423__1;
    CData/*0:0*/ __Vtrigprevexpr_hcc347338__1;
    CData/*0:0*/ __Vtrigprevexpr_hcc287291__1;
    CData/*0:0*/ __Vtrigprevexpr_hcc2c9499__1;
    CData/*0:0*/ __Vtrigprevexpr_hcc20226a__1;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    SData/*15:0*/ test_hmi_gear_feedback__DOT__dut__DOT__byte_gap;
    SData/*15:0*/ test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt;
    SData/*15:0*/ test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt;
    SData/*15:0*/ __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt;
    IData/*31:0*/ test_hmi_gear_feedback__DOT__count;
    VlWide<6>/*191:0*/ test_hmi_gear_feedback__DOT__frame__Vstatic__expected;
    VlWide<7>/*215:0*/ test_hmi_gear_feedback__DOT__frame__Vstatic__number;
    VlWide<3>/*79:0*/ test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected;
    VlWide<6>/*191:0*/ test_hmi_gear_feedback__DOT__text_only__Vstatic__expected;
    IData/*23:0*/ test_hmi_gear_feedback__DOT__dut__DOT__refresh_count;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<CData/*7:0*/, 1024> test_hmi_gear_feedback__DOT__captured;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h5bccc69a__0;
    VlTriggerScheduler __VtrigSched_h5bccd0ab__0;
    VlTriggerScheduler __VtrigSched_h5bb97608__0;
    VlTriggerScheduler __VtrigSched_h5bcac0b3__0;
    VlTriggerScheduler __VtrigSched_h5c0684b4__0;
    VlTriggerScheduler __VtrigSched_h5c0367bd__0;
    VlTriggerScheduler __VtrigSched_h5c0f6716__0;
    VlTriggerScheduler __VtrigSched_h5c1a851e__0;
    VlTriggerScheduler __VtrigSched_h5c2716f3__0;

    // INTERNAL VARIABLES
    Vtest_hmi_gear_feedback__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_hmi_gear_feedback___024root(Vtest_hmi_gear_feedback__Syms* symsp, const char* namep);
    ~Vtest_hmi_gear_feedback___024root();
    VL_UNCOPYABLE(Vtest_hmi_gear_feedback___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
