// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_smdt_prefix.h for the primary calling header

#ifndef VERILATED_VTEST_SMDT_PREFIX___024ROOT_H_
#define VERILATED_VTEST_SMDT_PREFIX___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_smdt_prefix__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_smdt_prefix___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_smdt_prefix__DOT__wr_clk;
    CData/*0:0*/ test_smdt_prefix__DOT__rst_n;
    CData/*0:0*/ test_smdt_prefix__DOT__spi_clk;
    CData/*0:0*/ test_smdt_prefix__DOT__spi_cs;
    CData/*3:0*/ test_smdt_prefix__DOT__master_data;
    CData/*3:0*/ test_smdt_prefix__DOT__spi_data;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__recv_flag;
    CData/*7:0*/ test_smdt_prefix__DOT__slave__DOT__recv_data;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__recv_valid;
    CData/*1:0*/ test_smdt_prefix__DOT__slave__DOT__cmd_reg;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__cmd_lock;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__tx_valid_reg;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__auto_enable_meta;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__auto_enable_sync;
    CData/*2:0*/ test_smdt_prefix__DOT__slave__DOT__status_nibble_count;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__status_read_ready;
    CData/*7:0*/ test_smdt_prefix__DOT__slave__DOT__status_read_data;
    CData/*0:0*/ test_smdt_prefix__DOT__slave__DOT__status_tx_started;
    CData/*3:0*/ test_smdt_prefix__DOT__slave__DOT__status_tx_nibble;
    CData/*7:0*/ test_smdt_prefix__DOT__regs__DOT____Vlvbound_h11419250__0;
    CData/*0:0*/ test_smdt_prefix__DOT__regs__DOT__rx_pose;
    CData/*2:0*/ test_smdt_prefix__DOT__regs__DOT__state;
    CData/*1:0*/ test_smdt_prefix__DOT__regs__DOT__cmd_reg;
    CData/*0:0*/ test_smdt_prefix__DOT__regs__DOT__spi_cs_reg;
    CData/*0:0*/ test_smdt_prefix__DOT__regs__DOT__rx_valid_d0;
    CData/*0:0*/ test_smdt_prefix__DOT__regs__DOT__rx_valid_d1;
    CData/*0:0*/ test_smdt_prefix__DOT__regs__DOT__tx_done_d0;
    CData/*0:0*/ test_smdt_prefix__DOT__regs__DOT__tx_done_d1;
    CData/*7:0*/ test_smdt_prefix__DOT__regs__DOT__wr_data_reg;
    CData/*7:0*/ test_smdt_prefix__DOT__regs__DOT__addr_high;
    CData/*0:0*/ test_smdt_prefix__DOT__regs__DOT__ram_wr_en;
    CData/*1:0*/ test_smdt_prefix__DOT__regs__DOT__byte_cnt;
    CData/*7:0*/ test_smdt_prefix__DOT__regs__DOT__read_addr_high;
    CData/*3:0*/ __VdfgRegularize_h6e95ff9d_0_1;
    CData/*0:0*/ __Vdly__test_smdt_prefix__DOT__slave__DOT__recv_flag;
    CData/*2:0*/ __Vdly__test_smdt_prefix__DOT__slave__DOT__status_nibble_count;
    CData/*0:0*/ __Vdly__test_smdt_prefix__DOT__slave__DOT__cmd_lock;
    CData/*0:0*/ __Vdly__test_smdt_prefix__DOT__slave__DOT__tx_valid_reg;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_smdt_prefix__DOT__rst_n__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_smdt_prefix__DOT__spi_clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_smdt_prefix__DOT__spi_cs__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_smdt_prefix__DOT__wr_clk__0;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    SData/*15:0*/ test_smdt_prefix__DOT__regs__DOT__ram_addr;
    IData/*23:0*/ test_smdt_prefix__DOT__slave__DOT__status_header;
    IData/*23:0*/ test_smdt_prefix__DOT__slave__DOT__next_status_header;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<CData/*7:0*/, 5> test_smdt_prefix__DOT__regs__DOT__mem;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;

    // INTERNAL VARIABLES
    Vtest_smdt_prefix__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_smdt_prefix___024root(Vtest_smdt_prefix__Syms* symsp, const char* namep);
    ~Vtest_smdt_prefix___024root();
    VL_UNCOPYABLE(Vtest_smdt_prefix___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
