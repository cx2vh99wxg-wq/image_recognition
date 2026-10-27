// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_hmi_serial_release.h for the primary calling header

#ifndef VERILATED_VTEST_HMI_SERIAL_RELEASE___024ROOT_H_
#define VERILATED_VTEST_HMI_SERIAL_RELEASE___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_hmi_serial_release__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_serial_release___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_hmi_serial_release__DOT__clk_50m;
    CData/*0:0*/ test_hmi_serial_release__DOT__clk_25m;
    CData/*0:0*/ test_hmi_serial_release__DOT__rst_n;
    CData/*0:0*/ test_hmi_serial_release__DOT__uart_rxd;
    CData/*0:0*/ test_hmi_serial_release__DOT__uart_rx_valid;
    CData/*7:0*/ test_hmi_serial_release__DOT__uart_rx_data;
    CData/*3:0*/ test_hmi_serial_release__DOT__hmi_code_sync;
    CData/*0:0*/ test_hmi_serial_release__DOT__rearm_command;
    CData/*0:0*/ test_hmi_serial_release__DOT__movement_blocked;
    CData/*0:0*/ test_hmi_serial_release__DOT__brake_request;
    CData/*3:0*/ test_hmi_serial_release__DOT__fspi_released;
    CData/*0:0*/ test_hmi_serial_release__DOT__stop_pad;
    CData/*3:0*/ test_hmi_serial_release__DOT__direction_pad;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_uart_rx__DOT__uart_rxd_d0;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_uart_rx__DOT__uart_rxd_d1;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_uart_rx__DOT__uart_rxd_d2;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_uart_rx__DOT__rx_flag;
    CData/*3:0*/ test_hmi_serial_release__DOT__u_uart_rx__DOT__rx_cnt;
    CData/*7:0*/ test_hmi_serial_release__DOT__u_uart_rx__DOT__rx_data_t;
    CData/*3:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__cnn_level_reg;
    CData/*7:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_3;
    CData/*7:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_2;
    CData/*7:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_1;
    CData/*1:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__bytes_seen;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_complete;
    CData/*3:0*/ test_hmi_serial_release__DOT__u_hmi_control_synchronizer__DOT__code_meta;
    CData/*2:0*/ test_hmi_serial_release__DOT__u_hmi_control_synchronizer__DOT__e_stable_count;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering;
    CData/*1:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched;
    CData/*3:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__previous_hmi_code;
    CData/*2:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__traction_stable_count;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__traction_press_consumed;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_code_changed;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_hold;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering_hold;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_stop__DOT__brake_latched;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_stop__DOT__brake_event_armed;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_stop__DOT__rearm_previous;
    CData/*0:0*/ test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed;
    CData/*3:0*/ __Vdly__test_hmi_serial_release__DOT__u_uart_lcd__DOT__cnn_level_reg;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__clk_50m__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__rst_n__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__clk_25m__0;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    SData/*15:0*/ test_hmi_serial_release__DOT__u_uart_rx__DOT__baud_cnt;
    IData/*31:0*/ test_hmi_serial_release__DOT__send_byte__Vstatic__bit_index;
    IData/*19:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_gap_count;
    IData/*26:0*/ test_hmi_serial_release__DOT__u_uart_lcd__DOT__traction_lease_count;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_hafc44e07__0;
    VlTriggerScheduler __VtrigSched_ha4c9642b__0;
    VlTriggerScheduler __VtrigSched_hafc44f2b__0;

    // INTERNAL VARIABLES
    Vtest_hmi_serial_release__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_hmi_serial_release___024root(Vtest_hmi_serial_release__Syms* symsp, const char* namep);
    ~Vtest_hmi_serial_release___024root();
    VL_UNCOPYABLE(Vtest_hmi_serial_release___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
