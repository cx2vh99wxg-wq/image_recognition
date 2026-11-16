// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_autopilot.h for the primary calling header

#include "Vtest_hmi_autopilot__pch.h"

VlCoroutine Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__0(Vtest_hmi_autopilot___024root* vlSelf);
VlCoroutine Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__1(Vtest_hmi_autopilot___024root* vlSelf);

void Vtest_hmi_autopilot___024root___eval_initial(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_initial\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(Vtest_hmi_autopilot___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__0(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_hmi_autopilot__DOT__unnamedblk1_3__DOT____Vrepeat2;
    test_hmi_autopilot__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0;
    IData/*31:0*/ test_hmi_autopilot__DOT__unnamedblk1_4__DOT____Vrepeat3;
    test_hmi_autopilot__DOT__unnamedblk1_4__DOT____Vrepeat3 = 0;
    IData/*31:0*/ test_hmi_autopilot__DOT__unnamedblk1_5__DOT____Vrepeat4;
    test_hmi_autopilot__DOT__unnamedblk1_5__DOT____Vrepeat4 = 0;
    IData/*31:0*/ test_hmi_autopilot__DOT__unnamedblk1_6__DOT____Vrepeat5;
    test_hmi_autopilot__DOT__unnamedblk1_6__DOT____Vrepeat5 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__0__number;
    __Vtask_test_hmi_autopilot__DOT__command__0__number = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__0__value;
    __Vtask_test_hmi_autopilot__DOT__command__0__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__command__0__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_autopilot__DOT__command__0__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__1__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__1__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__2__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__2__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__3__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__3__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__4__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__4__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__5__number;
    __Vtask_test_hmi_autopilot__DOT__command__5__number = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__5__value;
    __Vtask_test_hmi_autopilot__DOT__command__5__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__command__5__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_autopilot__DOT__command__5__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__6__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__6__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__7__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__7__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__8__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__8__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__9__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__9__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__10__number;
    __Vtask_test_hmi_autopilot__DOT__command__10__number = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__10__value;
    __Vtask_test_hmi_autopilot__DOT__command__10__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__command__10__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_autopilot__DOT__command__10__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__11__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__11__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__12__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__12__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__13__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__13__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__14__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__14__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_autopilot__DOT__approve__15__count;
    __Vtask_test_hmi_autopilot__DOT__approve__15__count = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__approve__15__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_hmi_autopilot__DOT__approve__15__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*3:0*/ __Vtask_test_hmi_autopilot__DOT__approve__16__count;
    __Vtask_test_hmi_autopilot__DOT__approve__16__count = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__approve__16__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_hmi_autopilot__DOT__approve__16__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*3:0*/ __Vtask_test_hmi_autopilot__DOT__approve__17__count;
    __Vtask_test_hmi_autopilot__DOT__approve__17__count = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__approve__17__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_hmi_autopilot__DOT__approve__17__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__18__number;
    __Vtask_test_hmi_autopilot__DOT__command__18__number = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__18__value;
    __Vtask_test_hmi_autopilot__DOT__command__18__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__command__18__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_autopilot__DOT__command__18__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__19__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__19__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__20__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__20__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__21__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__21__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__22__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__22__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_autopilot__DOT__approve__23__count;
    __Vtask_test_hmi_autopilot__DOT__approve__23__count = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__approve__23__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_hmi_autopilot__DOT__approve__23__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*3:0*/ __Vtask_test_hmi_autopilot__DOT__approve__24__count;
    __Vtask_test_hmi_autopilot__DOT__approve__24__count = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__approve__24__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_hmi_autopilot__DOT__approve__24__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__25__number;
    __Vtask_test_hmi_autopilot__DOT__command__25__number = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__25__value;
    __Vtask_test_hmi_autopilot__DOT__command__25__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__command__25__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_autopilot__DOT__command__25__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__26__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__26__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__27__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__27__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__28__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__28__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__29__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__29__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_autopilot__DOT__approve__30__count;
    __Vtask_test_hmi_autopilot__DOT__approve__30__count = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__approve__30__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1;
    __Vtask_test_hmi_autopilot__DOT__approve__30__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__31__number;
    __Vtask_test_hmi_autopilot__DOT__command__31__number = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__31__value;
    __Vtask_test_hmi_autopilot__DOT__command__31__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__command__31__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_autopilot__DOT__command__31__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__32__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__32__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__33__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__33__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__34__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__34__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__35__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__35__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__36__number;
    __Vtask_test_hmi_autopilot__DOT__command__36__number = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__command__36__value;
    __Vtask_test_hmi_autopilot__DOT__command__36__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_autopilot__DOT__command__36__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_autopilot__DOT__command__36__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__37__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__37__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__38__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__38__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__39__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__39__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_autopilot__DOT__send_byte__40__value;
    __Vtask_test_hmi_autopilot__DOT__send_byte__40__value = 0;
    // Body
    test_hmi_autopilot__DOT__unnamedblk1_3__DOT____Vrepeat2 = 4U;
    while (VL_LTS_III(32, 0U, test_hmi_autopilot__DOT__unnamedblk1_3__DOT____Vrepeat2)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             38);
        test_hmi_autopilot__DOT__unnamedblk1_3__DOT____Vrepeat2 
            = (test_hmi_autopilot__DOT__unnamedblk1_3__DOT____Vrepeat2 
               - (IData)(1U));
    }
    vlSelfRef.test_hmi_autopilot__DOT__reset_n = 1U;
    __Vtask_test_hmi_autopilot__DOT__command__0__value = 0xffU;
    __Vtask_test_hmi_autopilot__DOT__command__0__number = 0x0fU;
    __Vtask_test_hmi_autopilot__DOT__command__0__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_autopilot__DOT__send_byte__1__value = 0x30U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__1__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__2__value = 0x90U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__2__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__3__value 
        = __Vtask_test_hmi_autopilot__DOT__command__0__number;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__3__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__4__value 
        = __Vtask_test_hmi_autopilot__DOT__command__0__value;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__4__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__command__0__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__command__0__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             28);
        __Vtask_test_hmi_autopilot__DOT__command__0__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_autopilot__DOT__command__0__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    if (VL_UNLIKELY(((1U & (((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg)) 
                             | ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                                & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))) 
                            | (IData)(vlSelfRef.test_hmi_autopilot__DOT__requested)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:41: Assertion failed in %m: brake independent, autopilot off\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 41, "", false);
    }
    __Vtask_test_hmi_autopilot__DOT__command__5__value = 1U;
    __Vtask_test_hmi_autopilot__DOT__command__5__number = 0x12U;
    __Vtask_test_hmi_autopilot__DOT__command__5__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_autopilot__DOT__send_byte__6__value = 0x30U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__6__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__7__value = 0x90U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__7__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__8__value 
        = __Vtask_test_hmi_autopilot__DOT__command__5__number;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__8__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__9__value 
        = __Vtask_test_hmi_autopilot__DOT__command__5__value;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__9__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__command__5__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__command__5__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             28);
        __Vtask_test_hmi_autopilot__DOT__command__5__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_autopilot__DOT__command__5__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_autopilot__DOT__requested) 
                      | ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                         & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:43: Assertion failed in %m: invalid payload\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 43, "", false);
    }
    __Vtask_test_hmi_autopilot__DOT__command__10__value = 0xffU;
    __Vtask_test_hmi_autopilot__DOT__command__10__number = 0x12U;
    __Vtask_test_hmi_autopilot__DOT__command__10__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_autopilot__DOT__send_byte__11__value = 0x30U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__11__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__12__value = 0x90U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__12__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__13__value 
        = __Vtask_test_hmi_autopilot__DOT__command__10__number;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__13__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__14__value 
        = __Vtask_test_hmi_autopilot__DOT__command__10__value;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__14__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__command__10__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__command__10__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             28);
        __Vtask_test_hmi_autopilot__DOT__command__10__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_autopilot__DOT__command__10__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    if (VL_UNLIKELY(((1U & (((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__requested)) 
                             | ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                                & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))) 
                            | (~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:45: Assertion failed in %m: request alone must not arm\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 45, "", false);
    }
    vlSelfRef.test_hmi_autopilot__DOT__released = 0U;
    __Vtask_test_hmi_autopilot__DOT__approve__15__count = 1U;
    __Vtask_test_hmi_autopilot__DOT__approve__15__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 
        (0x00000080U | (((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                         << 4U) | (IData)(__Vtask_test_hmi_autopilot__DOT__approve__15__count)));
    __Vtask_test_hmi_autopilot__DOT__approve__15__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__approve__15__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             34);
        __Vtask_test_hmi_autopilot__DOT__approve__15__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_hmi_autopilot__DOT__approve__15__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    vlSelfRef.test_hmi_autopilot__DOT__released = 1U;
    test_hmi_autopilot__DOT__unnamedblk1_4__DOT____Vrepeat3 = 4U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                      & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:48: Assertion failed in %m: previous contact must release before accepting\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 48, "", false);
    }
    while (VL_LTS_III(32, 0U, test_hmi_autopilot__DOT__unnamedblk1_4__DOT____Vrepeat3)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             50);
        test_hmi_autopilot__DOT__unnamedblk1_4__DOT____Vrepeat3 
            = (test_hmi_autopilot__DOT__unnamedblk1_4__DOT____Vrepeat3 
               - (IData)(1U));
    }
    vlSelfRef.test_hmi_autopilot__DOT__released = 0U;
    __Vtask_test_hmi_autopilot__DOT__approve__16__count = 2U;
    __Vtask_test_hmi_autopilot__DOT__approve__16__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 
        (0x00000080U | (((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                         << 4U) | (IData)(__Vtask_test_hmi_autopilot__DOT__approve__16__count)));
    __Vtask_test_hmi_autopilot__DOT__approve__16__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 5U;
    if (VL_UNLIKELY(((1U & (~ ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                               & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:51: Assertion failed in %m: approval should arm\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 51, "", false);
    }
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__approve__16__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             34);
        __Vtask_test_hmi_autopilot__DOT__approve__16__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_hmi_autopilot__DOT__approve__16__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    test_hmi_autopilot__DOT__unnamedblk1_5__DOT____Vrepeat4 = 0x0000002aU;
    if (VL_UNLIKELY(((1U & (~ ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                               & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:54: Assertion failed in %m: renewal must allow a current pulse\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 54, "", false);
    }
    while (VL_LTS_III(32, 0U, test_hmi_autopilot__DOT__unnamedblk1_5__DOT____Vrepeat4)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             55);
        test_hmi_autopilot__DOT__unnamedblk1_5__DOT____Vrepeat4 
            = (test_hmi_autopilot__DOT__unnamedblk1_5__DOT____Vrepeat4 
               - (IData)(1U));
    }
    vlSelfRef.test_hmi_autopilot__DOT__released = 1U;
    __Vtask_test_hmi_autopilot__DOT__approve__17__count = 3U;
    __Vtask_test_hmi_autopilot__DOT__approve__17__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 
        (0x00000080U | (((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                         << 4U) | (IData)(__Vtask_test_hmi_autopilot__DOT__approve__17__count)));
    __Vtask_test_hmi_autopilot__DOT__approve__17__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 5U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                      & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:56: Assertion failed in %m: expired heartbeat must release\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 56, "", false);
    }
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__approve__17__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             34);
        __Vtask_test_hmi_autopilot__DOT__approve__17__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_hmi_autopilot__DOT__approve__17__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_hmi_autopilot__DOT__command__18__value = 0xffU;
    __Vtask_test_hmi_autopilot__DOT__command__18__number = 0x12U;
    __Vtask_test_hmi_autopilot__DOT__command__18__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_autopilot__DOT__send_byte__19__value = 0x30U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                      & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:59: Assertion failed in %m: expired request must not rearm\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 59, "", false);
    }
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__19__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__20__value = 0x90U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__20__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__21__value 
        = __Vtask_test_hmi_autopilot__DOT__command__18__number;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__21__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__22__value 
        = __Vtask_test_hmi_autopilot__DOT__command__18__value;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__22__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__command__18__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__command__18__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             28);
        __Vtask_test_hmi_autopilot__DOT__command__18__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_autopilot__DOT__command__18__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_hmi_autopilot__DOT__approve__23__count = 4U;
    __Vtask_test_hmi_autopilot__DOT__approve__23__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 
        (0x00000080U | (((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                         << 4U) | (IData)(__Vtask_test_hmi_autopilot__DOT__approve__23__count)));
    __Vtask_test_hmi_autopilot__DOT__approve__23__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__approve__23__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             34);
        __Vtask_test_hmi_autopilot__DOT__approve__23__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_hmi_autopilot__DOT__approve__23__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    vlSelfRef.test_hmi_autopilot__DOT__blocked = 1U;
    test_hmi_autopilot__DOT__unnamedblk1_6__DOT____Vrepeat5 = 4U;
    if (VL_UNLIKELY(((1U & (~ ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                               & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:62: Assertion failed in %m: fresh approved request must rearm\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 62, "", false);
    }
    while (VL_LTS_III(32, 0U, test_hmi_autopilot__DOT__unnamedblk1_6__DOT____Vrepeat5)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             64);
        test_hmi_autopilot__DOT__unnamedblk1_6__DOT____Vrepeat5 
            = (test_hmi_autopilot__DOT__unnamedblk1_6__DOT____Vrepeat5 
               - (IData)(1U));
    }
    vlSelfRef.test_hmi_autopilot__DOT__blocked = 0U;
    __Vtask_test_hmi_autopilot__DOT__approve__24__count = 5U;
    __Vtask_test_hmi_autopilot__DOT__approve__24__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 
        (0x00000080U | (((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                         << 4U) | (IData)(__Vtask_test_hmi_autopilot__DOT__approve__24__count)));
    __Vtask_test_hmi_autopilot__DOT__approve__24__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 5U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                      & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:65: Assertion failed in %m: STOP must release\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 65, "", false);
    }
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__approve__24__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             34);
        __Vtask_test_hmi_autopilot__DOT__approve__24__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_hmi_autopilot__DOT__approve__24__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                      & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:68: Assertion failed in %m: STOP release must not rearm\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 68, "", false);
    }
    __Vtask_test_hmi_autopilot__DOT__command__25__value = 0xffU;
    __Vtask_test_hmi_autopilot__DOT__command__25__number = 0x12U;
    __Vtask_test_hmi_autopilot__DOT__command__25__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_autopilot__DOT__send_byte__26__value = 0x30U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__26__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__27__value = 0x90U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__27__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__28__value 
        = __Vtask_test_hmi_autopilot__DOT__command__25__number;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__28__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__29__value 
        = __Vtask_test_hmi_autopilot__DOT__command__25__value;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__29__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__command__25__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__command__25__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             28);
        __Vtask_test_hmi_autopilot__DOT__command__25__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_autopilot__DOT__command__25__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    __Vtask_test_hmi_autopilot__DOT__approve__30__count = 6U;
    __Vtask_test_hmi_autopilot__DOT__approve__30__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 
        (0x00000080U | (((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                         << 4U) | (IData)(__Vtask_test_hmi_autopilot__DOT__approve__30__count)));
    __Vtask_test_hmi_autopilot__DOT__approve__30__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__approve__30__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             34);
        __Vtask_test_hmi_autopilot__DOT__approve__30__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (__Vtask_test_hmi_autopilot__DOT__approve__30__test_hmi_autopilot__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    if (VL_UNLIKELY(((1U & (~ ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                               & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:71: Assertion failed in %m: new request after STOP\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 71, "", false);
    }
    __Vtask_test_hmi_autopilot__DOT__command__31__value = 0xeeU;
    __Vtask_test_hmi_autopilot__DOT__command__31__number = 0x12U;
    __Vtask_test_hmi_autopilot__DOT__command__31__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_autopilot__DOT__send_byte__32__value = 0x30U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__32__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__33__value = 0x90U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__33__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__34__value 
        = __Vtask_test_hmi_autopilot__DOT__command__31__number;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__34__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__35__value 
        = __Vtask_test_hmi_autopilot__DOT__command__31__value;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__35__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__command__31__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__command__31__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             28);
        __Vtask_test_hmi_autopilot__DOT__command__31__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_autopilot__DOT__command__31__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    if (VL_UNLIKELY(((1U & ((((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted) 
                              & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches)) 
                             | (IData)(vlSelfRef.test_hmi_autopilot__DOT__requested)) 
                            | (~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:73: Assertion failed in %m: explicit OFF and brake independence\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 73, "", false);
    }
    __Vtask_test_hmi_autopilot__DOT__command__36__value = 0xeeU;
    __Vtask_test_hmi_autopilot__DOT__command__36__number = 0x0fU;
    __Vtask_test_hmi_autopilot__DOT__command__36__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_autopilot__DOT__send_byte__37__value = 0x30U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__37__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__38__value = 0x90U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__38__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__39__value 
        = __Vtask_test_hmi_autopilot__DOT__command__36__number;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__39__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__send_byte__40__value 
        = __Vtask_test_hmi_autopilot__DOT__command__36__value;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         21);
    vlSelfRef.test_hmi_autopilot__DOT__data = __Vtask_test_hmi_autopilot__DOT__send_byte__40__value;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 1U;
    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                              "@(negedge test_hmi_autopilot.clk)");
    co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_autopilot.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                         22);
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    __Vtask_test_hmi_autopilot__DOT__command__36__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 = 5U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_autopilot__DOT__command__36__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(vlSelf, 
                                                                  "@(negedge test_hmi_autopilot.clk)");
        co_await vlSelfRef.__VtrigSched_h1fa99d08__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_autopilot.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                                             28);
        __Vtask_test_hmi_autopilot__DOT__command__36__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_autopilot__DOT__command__36__test_hmi_autopilot__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    if (VL_UNLIKELY((vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_autopilot.v:75: Assertion failed in %m: existing brake OFF command\n",3, 'M',vlSymsp->name(),"test_hmi_autopilot", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 75, "", false);
    }
    VL_WRITEF_NX("PASS: HMI autopilot request, approval, contact release, lease expiry and STOP\n",0);
    VL_FINISH_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 77, "");
    co_return;
}

VlCoroutine Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__1(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 
                                             4);
        vlSelfRef.test_hmi_autopilot__DOT__clk = (1U 
                                                  & (~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__clk)));
    }
    co_return;
}

void Vtest_hmi_autopilot___024root___eval_triggers_vec__act(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_triggers_vec__act\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                       << 3U) 
                                                      | (((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__clk)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0)) 
                                                         << 2U)) 
                                                     | ((((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__reset_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__reset_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.test_hmi_autopilot__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0 
        = vlSelfRef.test_hmi_autopilot__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__reset_n__0 
        = vlSelfRef.test_hmi_autopilot__DOT__reset_n;
}

bool Vtest_hmi_autopilot___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___trigger_anySet__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        if (in[n]) {
            return (1U);
        }
        n = ((IData)(1U) + n);
    } while ((1U > n));
    return (0U);
}

void Vtest_hmi_autopilot___024root___act_sequent__TOP__0(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___act_sequent__TOP__0\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete 
        = ((((IData)(vlSelfRef.test_hmi_autopilot__DOT__valid) 
             & (3U == (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen))) 
            & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history))) 
           & (0x0007a120U > vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count));
    vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches 
        = ((((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__blocked)) 
             & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync) 
                >> 2U)) & (0x80U == (0xc0U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous)))) 
           & (((3U & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous) 
                      >> 4U)) == (3U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync))) 
              & (0x28U > (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age))));
}

void Vtest_hmi_autopilot___024root___eval_act(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_act\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete 
            = ((((IData)(vlSelfRef.test_hmi_autopilot__DOT__valid) 
                 & (3U == (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen))) 
                & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history))) 
               & (0x0007a120U > vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count));
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches 
            = ((((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__blocked)) 
                 & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync) 
                    >> 2U)) & (0x80U == (0xc0U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous)))) 
               & (((3U & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous) 
                          >> 4U)) == (3U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync))) 
                  & (0x28U > (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age))));
    }
}

void Vtest_hmi_autopilot___024root___nba_sequent__TOP__0(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___nba_sequent__TOP__0\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*23:0*/ __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history;
    __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history = 0;
    IData/*19:0*/ __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count;
    __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count = 0;
    CData/*1:0*/ __Vdly__test_hmi_autopilot__DOT__sequence_code;
    __Vdly__test_hmi_autopilot__DOT__sequence_code = 0;
    CData/*5:0*/ __Vdly__test_hmi_autopilot__DOT__gate__DOT__age;
    __Vdly__test_hmi_autopilot__DOT__gate__DOT__age = 0;
    CData/*0:0*/ __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed;
    __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed = 0;
    // Body
    __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
        = vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history;
    __Vdly__test_hmi_autopilot__DOT__sequence_code 
        = vlSelfRef.test_hmi_autopilot__DOT__sequence_code;
    __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count 
        = vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count;
    __Vdly__test_hmi_autopilot__DOT__gate__DOT__age 
        = vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age;
    __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed 
        = vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__consumed;
    if (vlSelfRef.test_hmi_autopilot__DOT__reset_n) {
        if (vlSelfRef.test_hmi_autopilot__DOT__valid) {
            __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
                = ((0x00ffff00U & (vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
                                   << 8U)) | (IData)(vlSelfRef.test_hmi_autopilot__DOT__data));
            vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen 
                = ((0x0007a120U <= vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count)
                    ? 1U : ((3U == (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen))
                             ? 3U : (3U & ((IData)(1U) 
                                           + (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen)))));
            __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count = 0U;
        } else if ((0x0007a120U > vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count)) {
            __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count 
                = (0x000fffffU & ((IData)(1U) + vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count));
        } else {
            vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen = 0U;
        }
        if (((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__heartbeat_sync) 
             != (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous))) {
            __Vdly__test_hmi_autopilot__DOT__gate__DOT__age = 0U;
        } else if ((0x28U > (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age))) {
            __Vdly__test_hmi_autopilot__DOT__gate__DOT__age 
                = (0x0000003fU & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age)));
        }
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous 
            = vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__heartbeat_sync;
        if (vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete) {
            if ((1U & (~ (vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
                          >> 7U)))) {
                if ((1U & (~ (vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
                              >> 6U)))) {
                    if ((1U & (~ (vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
                                      >> 4U)))) {
                            if ((8U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history)) {
                                if ((4U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history)) {
                                    if ((2U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history)) {
                                        if ((1U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history)) {
                                            if ((0xffU 
                                                 == (IData)(vlSelfRef.test_hmi_autopilot__DOT__data))) {
                                                vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg = 1U;
                                            } else if (
                                                       (0xeeU 
                                                        == (IData)(vlSelfRef.test_hmi_autopilot__DOT__data))) {
                                                vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg = 0U;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync) 
             != (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__last_request))) {
            vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted = 0U;
            __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed = 0U;
        } else if (vlSelfRef.test_hmi_autopilot__DOT__blocked) {
            vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted = 0U;
            __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed 
                = (1U & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync) 
                         >> 2U));
        } else if (vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches) {
            if (((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__consumed)) 
                 & (IData)(vlSelfRef.test_hmi_autopilot__DOT__released))) {
                vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted = 1U;
                __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed = 1U;
            }
        } else {
            vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted = 0U;
        }
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__last_request 
            = vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__heartbeat_sync 
            = vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__heartbeat_meta;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync 
            = vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_meta;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__heartbeat_meta 
            = vlSelfRef.test_hmi_autopilot__DOT__heartbeat;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_meta 
            = (((IData)(vlSelfRef.test_hmi_autopilot__DOT__requested) 
                << 2U) | (IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code));
        if (((IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete) 
             & (0x12U == (0x000000ffU & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history)))) {
            if ((0xeeU != (IData)(vlSelfRef.test_hmi_autopilot__DOT__data))) {
                if ((0xffU == (IData)(vlSelfRef.test_hmi_autopilot__DOT__data))) {
                    __Vdly__test_hmi_autopilot__DOT__sequence_code 
                        = ((2U & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                                  << 1U)) | (1U & (~ 
                                                   ((IData)(vlSelfRef.test_hmi_autopilot__DOT__sequence_code) 
                                                    >> 1U))));
                }
            }
            if ((0xeeU == (IData)(vlSelfRef.test_hmi_autopilot__DOT__data))) {
                vlSelfRef.test_hmi_autopilot__DOT__requested = 0U;
            } else if ((0xffU == (IData)(vlSelfRef.test_hmi_autopilot__DOT__data))) {
                vlSelfRef.test_hmi_autopilot__DOT__requested = 1U;
            }
        }
    } else {
        __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history = 0U;
        __Vdly__test_hmi_autopilot__DOT__sequence_code = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen = 0U;
        __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count = 0U;
        __Vdly__test_hmi_autopilot__DOT__gate__DOT__age = 0x28U;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg = 0U;
        __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__accepted = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__last_request = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__heartbeat_sync = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__heartbeat_meta = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_meta = 0U;
        vlSelfRef.test_hmi_autopilot__DOT__requested = 0U;
    }
    vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count 
        = __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count;
    vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age 
        = __Vdly__test_hmi_autopilot__DOT__gate__DOT__age;
    vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__consumed 
        = __Vdly__test_hmi_autopilot__DOT__gate__DOT__consumed;
    vlSelfRef.test_hmi_autopilot__DOT__sequence_code 
        = __Vdly__test_hmi_autopilot__DOT__sequence_code;
    vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history 
        = __Vdly__test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history;
}

void Vtest_hmi_autopilot___024root___nba_comb__TOP__0(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___nba_comb__TOP__0\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches 
        = ((((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__blocked)) 
             & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync) 
                >> 2U)) & (0x80U == (0xc0U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous)))) 
           & (((3U & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous) 
                      >> 4U)) == (3U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync))) 
              & (0x28U > (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age))));
    vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete 
        = ((((IData)(vlSelfRef.test_hmi_autopilot__DOT__valid) 
             & (3U == (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen))) 
            & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history))) 
           & (0x0007a120U > vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count));
}

void Vtest_hmi_autopilot___024root___eval_nba(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_nba\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_hmi_autopilot___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__approval_matches 
            = ((((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__blocked)) 
                 & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync) 
                    >> 2U)) & (0x80U == (0xc0U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous)))) 
               & (((3U & ((IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__previous) 
                          >> 4U)) == (3U & (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__request_sync))) 
                  & (0x28U > (IData)(vlSelfRef.test_hmi_autopilot__DOT__gate__DOT__age))));
        vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete 
            = ((((IData)(vlSelfRef.test_hmi_autopilot__DOT__valid) 
                 & (3U == (IData)(vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen))) 
                & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history))) 
               & (0x0007a120U > vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count));
    }
}

void Vtest_hmi_autopilot___024root___timing_ready(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___timing_ready\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready("@(negedge test_hmi_autopilot.clk)");
    }
}

void Vtest_hmi_autopilot___024root___timing_resume(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___timing_resume\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h1fa99d08__0.moveToResumeQueue(
                                                          "@(negedge test_hmi_autopilot.clk)");
    vlSelfRef.__VtrigSched_h1fa99d08__0.resume("@(negedge test_hmi_autopilot.clk)");
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_hmi_autopilot___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___trigger_orInto__act_vec_vec\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = (out[n] | in[n]);
        n = ((IData)(1U) + n);
    } while ((0U >= n));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_autopilot___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_hmi_autopilot___024root___eval_phase__act(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_phase__act\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_hmi_autopilot___024root___eval_triggers_vec__act(vlSelf);
    Vtest_hmi_autopilot___024root___timing_ready(vlSelf);
    Vtest_hmi_autopilot___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_autopilot___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_hmi_autopilot___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_hmi_autopilot___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_hmi_autopilot___024root___timing_resume(vlSelf);
        Vtest_hmi_autopilot___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_hmi_autopilot___024root___eval_phase__inact(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_phase__inact\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_hmi_autopilot___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_hmi_autopilot___024root___eval_phase__nba(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_phase__nba\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_hmi_autopilot___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_hmi_autopilot___024root___eval_nba(vlSelf);
        Vtest_hmi_autopilot___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_hmi_autopilot___024root___eval(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_autopilot___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 2, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 2, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_hmi_autopilot___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 2, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_hmi_autopilot___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_hmi_autopilot___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_hmi_autopilot___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0(Vtest_hmi_autopilot___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root____VbeforeTrig_h1fa99d08__0\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((~ (IData)(vlSelfRef.test_hmi_autopilot__DOT__clk)) 
                                   & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0)) 
                                  << 2U)));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0 
        = vlSelfRef.test_hmi_autopilot__DOT__clk;
    if ((4ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1fa99d08__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_hmi_autopilot___024root___eval_debug_assertions(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_debug_assertions\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
