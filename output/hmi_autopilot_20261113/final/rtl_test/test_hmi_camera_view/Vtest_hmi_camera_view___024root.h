// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_hmi_camera_view.h for the primary calling header

#ifndef VERILATED_VTEST_HMI_CAMERA_VIEW___024ROOT_H_
#define VERILATED_VTEST_HMI_CAMERA_VIEW___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_hmi_camera_view__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_camera_view___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ test_hmi_camera_view__DOT__clk;
        CData/*0:0*/ test_hmi_camera_view__DOT__rst_n;
        CData/*0:0*/ test_hmi_camera_view__DOT__uart_rx_valid;
        CData/*7:0*/ test_hmi_camera_view__DOT__uart_rx;
        CData/*3:0*/ test_hmi_camera_view__DOT__view_code;
        CData/*0:0*/ test_hmi_camera_view__DOT__spi_clk;
        CData/*0:0*/ test_hmi_camera_view__DOT__spi_cs;
        CData/*0:0*/ test_hmi_camera_view__DOT__host_drives;
        CData/*3:0*/ test_hmi_camera_view__DOT__host_data;
        CData/*3:0*/ test_hmi_camera_view__DOT__bus;
        CData/*3:0*/ test_hmi_camera_view__DOT__expect_spi__Vstatic__hi;
        CData/*3:0*/ test_hmi_camera_view__DOT__expect_spi__Vstatic__lo;
        CData/*3:0*/ test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
        CData/*1:0*/ test_hmi_camera_view__DOT__decoder__DOT__bytes_seen;
        CData/*0:0*/ test_hmi_camera_view__DOT__decoder__DOT__frame_complete;
        CData/*3:0*/ test_hmi_camera_view__DOT__preview__DOT__previous_code;
        CData/*0:0*/ test_hmi_camera_view__DOT__transport__DOT__recv_flag;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__recv_data;
        CData/*1:0*/ test_hmi_camera_view__DOT__transport__DOT__cmd_reg;
        CData/*0:0*/ test_hmi_camera_view__DOT__transport__DOT__cmd_lock;
        CData/*0:0*/ test_hmi_camera_view__DOT__transport__DOT__auto_enable_meta;
        CData/*0:0*/ test_hmi_camera_view__DOT__transport__DOT__auto_enable_sync;
        CData/*2:0*/ test_hmi_camera_view__DOT__transport__DOT__status_nibble_count;
        CData/*0:0*/ test_hmi_camera_view__DOT__transport__DOT__status_read_ready;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__status_read_data;
        CData/*0:0*/ test_hmi_camera_view__DOT__transport__DOT__status_tx_started;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble;
        CData/*5:0*/ test_hmi_camera_view__DOT__transport__DOT__speed_meta;
        CData/*5:0*/ test_hmi_camera_view__DOT__transport__DOT__speed_sync;
        CData/*4:0*/ test_hmi_camera_view__DOT__transport__DOT__extended_index;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__extended_nibble;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__speed_snapshot;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__speed_crc;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__control_crc;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__camera_view_meta;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__camera_view_sync;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__camera_view_previous;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__autopilot_meta;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__autopilot_sync;
        CData/*3:0*/ test_hmi_camera_view__DOT__transport__DOT__autopilot_previous;
        CData/*0:0*/ test_hmi_camera_view__DOT__transport__DOT__extended_active;
        CData/*2:0*/ test_hmi_camera_view__DOT__transport__DOT__sampled_gear;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__sampled_speed;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__sampled_control_crc;
        CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__autopilot_status;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_1;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_2;
        CData/*0:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag;
        CData/*2:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count;
        CData/*0:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__rst_n__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_cs__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*8:0*/ test_hmi_camera_view__DOT__preview__DOT__cycles_left;
        SData/*15:0*/ test_hmi_camera_view__DOT__transport__DOT__control_meta;
    };
    struct {
        SData/*15:0*/ test_hmi_camera_view__DOT__transport__DOT__control_sync;
        SData/*15:0*/ test_hmi_camera_view__DOT__transport__DOT__control_snapshot;
        SData/*15:0*/ test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot;
        SData/*15:0*/ test_hmi_camera_view__DOT__transport__DOT__autopilot_packet;
        SData/*8:0*/ __Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left;
        IData/*23:0*/ test_hmi_camera_view__DOT__decoder__DOT__frame_history;
        IData/*19:0*/ test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count;
        IData/*23:0*/ test_hmi_camera_view__DOT__transport__DOT__status_header;
        IData/*23:0*/ test_hmi_camera_view__DOT__transport__DOT__next_status_header;
        IData/*31:0*/ test_hmi_camera_view__DOT__transport__DOT__wheel_meta;
        IData/*31:0*/ test_hmi_camera_view__DOT__transport__DOT__wheel_sync;
        IData/*31:0*/ test_hmi_camera_view__DOT__transport__DOT__wheel_previous;
        IData/*31:0*/ test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot;
        IData/*23:0*/ __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
        VlUnpacked<CData/*7:0*/, 15> test_hmi_camera_view__DOT__expect_spi__Vstatic__packet;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h4bbda687__0;
    VlTriggerScheduler __VtrigSched_h4bbda7ab__0;

    // INTERNAL VARIABLES
    Vtest_hmi_camera_view__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_hmi_camera_view___024root(Vtest_hmi_camera_view__Syms* symsp, const char* namep);
    ~Vtest_hmi_camera_view___024root();
    VL_UNCOPYABLE(Vtest_hmi_camera_view___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
