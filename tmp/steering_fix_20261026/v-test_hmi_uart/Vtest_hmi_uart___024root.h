// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_hmi_uart.h for the primary calling header

#ifndef VERILATED_VTEST_HMI_UART___024ROOT_H_
#define VERILATED_VTEST_HMI_UART___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_hmi_uart__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_uart___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_hmi_uart__DOT__clk;
    CData/*0:0*/ test_hmi_uart__DOT__rst_n;
    CData/*7:0*/ test_hmi_uart__DOT__uart_rx;
    CData/*0:0*/ test_hmi_uart__DOT__uart_rx_valid;
    CData/*3:0*/ test_hmi_uart__DOT__u_default_hmi__DOT__cnn_level_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_default_hmi__DOT__previous_3;
    CData/*7:0*/ test_hmi_uart__DOT__u_default_hmi__DOT__previous_2;
    CData/*7:0*/ test_hmi_uart__DOT__u_default_hmi__DOT__previous_1;
    CData/*1:0*/ test_hmi_uart__DOT__u_default_hmi__DOT__bytes_seen;
    CData/*0:0*/ test_hmi_uart__DOT__u_default_hmi__DOT__frame_complete;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__mode_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__blc_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__awb_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__liangdu_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__baohedu_reg;
    CData/*0:0*/ test_hmi_uart__DOT__u_hmi__DOT__awb_en_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__binarization_reg;
    CData/*3:0*/ test_hmi_uart__DOT__u_hmi__DOT__cnn_level_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__sobel_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__cb_min_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__cb_max_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__cr_min_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__cr_max_reg;
    CData/*0:0*/ test_hmi_uart__DOT__u_hmi__DOT__isp_judge_reg;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__previous_3;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__previous_2;
    CData/*7:0*/ test_hmi_uart__DOT__u_hmi__DOT__previous_1;
    CData/*1:0*/ test_hmi_uart__DOT__u_hmi__DOT__bytes_seen;
    CData/*0:0*/ test_hmi_uart__DOT__u_hmi__DOT__frame_complete;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_uart__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_uart__DOT__rst_n__0;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    IData/*19:0*/ test_hmi_uart__DOT__u_default_hmi__DOT__frame_gap_count;
    IData/*19:0*/ test_hmi_uart__DOT__u_hmi__DOT__frame_gap_count;
    IData/*26:0*/ test_hmi_uart__DOT__u_hmi__DOT__traction_lease_count;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h3f2cd376__0;

    // INTERNAL VARIABLES
    Vtest_hmi_uart__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_hmi_uart___024root(Vtest_hmi_uart__Syms* symsp, const char* namep);
    ~Vtest_hmi_uart___024root();
    VL_UNCOPYABLE(Vtest_hmi_uart___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
