// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_hmi_auto_brake_switch.h for the primary calling header

#ifndef VERILATED_VTEST_HMI_AUTO_BRAKE_SWITCH___024ROOT_H_
#define VERILATED_VTEST_HMI_AUTO_BRAKE_SWITCH___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_hmi_auto_brake_switch__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_hmi_auto_brake_switch___024root final {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__uart_clk;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__pad_clk;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__rst_n;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__uart_data;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__uart_valid;
        CData/*3:0*/ test_hmi_auto_brake_switch__DOT__hmi_code_sync;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__gated_request;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__raw_brake_request;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__use_spi_brake_request;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__rearm_command;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__stop_pressed;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__movement_blocked;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__spi_clk;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__spi_cs;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__master_drive;
        CData/*3:0*/ test_hmi_auto_brake_switch__DOT__master_data;
        CData/*3:0*/ test_hmi_auto_brake_switch__DOT__spi_data;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__register_clk;
        CData/*4:0*/ test_hmi_auto_brake_switch__DOT__fspi_button_meta;
        CData/*4:0*/ test_hmi_auto_brake_switch__DOT__fspi_button_sync;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__legacy_read;
        CData/*3:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1;
        CData/*1:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete;
        CData/*1:0*/ test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe;
        CData/*3:0*/ test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__code_meta;
        CData/*2:0*/ test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_latched;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_event_armed;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_stop__DOT__rearm_previous;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_hold;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering_hold;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid;
        CData/*1:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_meta;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_sync;
        CData/*2:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started;
        CData/*3:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT____Vlvbound_h11419250__0;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose;
        CData/*2:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state;
        CData/*1:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__cmd_reg;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__spi_cs_reg;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d0;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d1;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d0;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d1;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__addr_high;
        CData/*0:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en;
    };
    struct {
        CData/*1:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt;
        CData/*7:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__read_addr_high;
        CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__518__function_code;
        CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__518__value;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_0;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_1;
        CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_3;
        CData/*3:0*/ __VdfgRegularize_h6e95ff9d_0_4;
        CData/*1:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe;
        CData/*0:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag;
        CData/*2:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count;
        CData/*0:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock;
        CData/*0:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VstlPhaseResult;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__rst_n__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_cs__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__register_clk__0;
        CData/*0:0*/ __VactPhaseResult;
        CData/*0:0*/ __VinactPhaseResult;
        CData/*0:0*/ __VnbaPhaseResult;
        SData/*15:0*/ test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr;
        IData/*19:0*/ test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count;
        IData/*23:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header;
        IData/*23:0*/ test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header;
        IData/*31:0*/ __VactIterCount;
        IData/*31:0*/ __VinactIterCount;
        IData/*31:0*/ __Vi;
        VlUnpacked<CData/*7:0*/, 5> test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem;
        VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
        VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
        VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    };
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h1ff2e788__0;
    VlTriggerScheduler __VtrigSched_h482006c0__0;

    // INTERNAL VARIABLES
    Vtest_hmi_auto_brake_switch__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_hmi_auto_brake_switch___024root(Vtest_hmi_auto_brake_switch__Syms* symsp, const char* namep);
    ~Vtest_hmi_auto_brake_switch___024root();
    VL_UNCOPYABLE(Vtest_hmi_auto_brake_switch___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
