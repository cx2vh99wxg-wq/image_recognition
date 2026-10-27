// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_lane_permission.h for the primary calling header

#include "Vtest_lane_permission__pch.h"

VlCoroutine Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__0(Vtest_lane_permission___024root* vlSelf);
VlCoroutine Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__1(Vtest_lane_permission___024root* vlSelf);

void Vtest_lane_permission___024root___eval_initial(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_initial\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(Vtest_lane_permission___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__0(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_lane_permission__DOT__unnamedblk1_2__DOT____Vrepeat1;
    test_lane_permission__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__0__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__0__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__1__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__1__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__2__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__2__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__3__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__3__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__4__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__4__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__5__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__5__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__6__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__6__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__7__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__7__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__8__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__8__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__9__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__9__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__10__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__10__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__11__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__11__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__12__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__12__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__13__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__13__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ __Vtask_test_lane_permission__DOT__step__14__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_lane_permission__DOT__step__14__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    // Body
    __Vtask_test_lane_permission__DOT__step__0__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__0__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__0__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__0__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    vlSelfRef.test_lane_permission__DOT__rst_n = 1U;
    __Vtask_test_lane_permission__DOT__step__1__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__1__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__1__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__1__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__1__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    vlSelfRef.test_lane_permission__DOT__hmi = 0x0aU;
    __Vtask_test_lane_permission__DOT__step__2__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__2__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__2__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__2__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__2__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    vlSelfRef.test_lane_permission__DOT__hmi = 0U;
    __Vtask_test_lane_permission__DOT__step__3__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__3__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__3__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__3__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__3__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY(((1U & ((IData)(vlSelfRef.test_lane_permission__DOT__go) 
                            | (~ (IData)(vlSelfRef.test_lane_permission__DOT__back))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:15: Assertion failed in %m: GO latch baseline\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 15, "", false);
    }
    vlSelfRef.test_lane_permission__DOT__permission = 0xc1U;
    __Vtask_test_lane_permission__DOT__step__4__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__4__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__4__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__4__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__4__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY(((1U & ((~ ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                                & (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny))) 
                            | ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                               & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny) 
                                  >> 1U))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:16: Assertion failed in %m: left permission byte\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 16, "", false);
    }
    vlSelfRef.test_lane_permission__DOT__hmi = 0x0cU;
    __Vtask_test_lane_permission__DOT__step__5__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__5__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__5__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__5__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__5__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    vlSelfRef.test_lane_permission__DOT__hmi = 0x0dU;
    __Vtask_test_lane_permission__DOT__step__6__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__6__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    if (VL_UNLIKELY(((1U & (((~ (IData)(vlSelfRef.test_lane_permission__DOT__left)) 
                             | (~ (IData)(vlSelfRef.test_lane_permission__DOT__right))) 
                            | (IData)(vlSelfRef.test_lane_permission__DOT__go)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:17: Assertion failed in %m: solid left must block without clearing GO\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 17, "", false);
    }
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__6__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__6__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__6__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY(((1U & (((~ (IData)(vlSelfRef.test_lane_permission__DOT__left)) 
                             | (IData)(vlSelfRef.test_lane_permission__DOT__right)) 
                            | (IData)(vlSelfRef.test_lane_permission__DOT__go)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:18: Assertion failed in %m: dashed right still allowed\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 18, "", false);
    }
    vlSelfRef.test_lane_permission__DOT__permission = 0x43U;
    __Vtask_test_lane_permission__DOT__step__7__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__7__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__7__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__7__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__7__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    vlSelfRef.test_lane_permission__DOT__hmi = 0x0cU;
    __Vtask_test_lane_permission__DOT__step__8__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__8__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__8__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__8__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__8__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY(((1U & (((~ (IData)(vlSelfRef.test_lane_permission__DOT__left)) 
                             | (~ (IData)(vlSelfRef.test_lane_permission__DOT__right))) 
                            | (IData)(vlSelfRef.test_lane_permission__DOT__go)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:19: Assertion failed in %m: both solid\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 19, "", false);
    }
    vlSelfRef.test_lane_permission__DOT__hmi = 0U;
    vlSelfRef.test_lane_permission__DOT__fspi = 0x0fU;
    __Vtask_test_lane_permission__DOT__step__9__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__9__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__9__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__9__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__9__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    vlSelfRef.test_lane_permission__DOT__fspi = 0x0bU;
    __Vtask_test_lane_permission__DOT__step__10__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__10__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__10__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__10__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__10__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    vlSelfRef.test_lane_permission__DOT__stop = 1U;
    __Vtask_test_lane_permission__DOT__step__11__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__11__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    if (VL_UNLIKELY((vlSelfRef.test_lane_permission__DOT__left))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:20: Assertion failed in %m: curve following must bypass crossing permission\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 20, "", false);
    }
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__11__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__11__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__11__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY(((1U & ((((~ (IData)(vlSelfRef.test_lane_permission__DOT__left)) 
                              | (~ (IData)(vlSelfRef.test_lane_permission__DOT__right))) 
                             | (~ (IData)(vlSelfRef.test_lane_permission__DOT__go))) 
                            | (~ (IData)(vlSelfRef.test_lane_permission__DOT__back))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:21: Assertion failed in %m: STOP highest priority\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 21, "", false);
    }
    vlSelfRef.test_lane_permission__DOT__stop = 0U;
    vlSelfRef.test_lane_permission__DOT__fspi = 0x0fU;
    vlSelfRef.test_lane_permission__DOT__hmi = 0U;
    __Vtask_test_lane_permission__DOT__step__12__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__12__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__12__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__12__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__12__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.test_lane_permission__DOT__go)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:22: Assertion failed in %m: STOP clears traction intent\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 22, "", false);
    }
    test_lane_permission__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0x000000faU;
    while (VL_LTS_III(32, 0U, test_lane_permission__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             23);
        test_lane_permission__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (test_lane_permission__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         23);
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                      & (IData)((0U != (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:23: Assertion failed in %m: heartbeat timeout\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 23, "", false);
    }
    vlSelfRef.test_lane_permission__DOT__hmi = 0x0cU;
    __Vtask_test_lane_permission__DOT__step__13__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__13__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__13__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__13__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__13__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY((vlSelfRef.test_lane_permission__DOT__left))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:24: Assertion failed in %m: unknown legacy manual fallback\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 24, "", false);
    }
    vlSelfRef.test_lane_permission__DOT__permission = 0xc2U;
    __Vtask_test_lane_permission__DOT__step__14__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_lane_permission__DOT__step__14__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000016U;
    while (VL_LTS_III(32, 0U, __Vtask_test_lane_permission__DOT__step__14__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(vlSelf, 
                                                                    "@(posedge test_lane_permission.clk)");
        co_await vlSelfRef.__VtrigSched_h15e6f7c2__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_lane_permission.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                                             12);
        __Vtask_test_lane_permission__DOT__step__14__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_lane_permission__DOT__step__14__test_lane_permission__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                         12);
    if (VL_UNLIKELY(((1U & (((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                             & (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny)) 
                            | (~ ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                                  & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny) 
                                     >> 1U)))))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_lane_permission.v:25: Assertion failed in %m: fresh heartbeat right\n",3, 'M',vlSymsp->name(),"test_lane_permission", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 25, "", false);
    }
    VL_WRITEF_NX("lane permission, HMI/FSPI separation, traction, STOP, timeout PASS\n",0);
    VL_FINISH_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 26, "");
    co_return;
}

VlCoroutine Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__1(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 
                                             3);
        vlSelfRef.test_lane_permission__DOT__clk = 
            (1U & (~ (IData)(vlSelfRef.test_lane_permission__DOT__clk)));
    }
    co_return;
}

void Vtest_lane_permission___024root___eval_triggers_vec__act(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_triggers_vec__act\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                      << 2U) 
                                                     | ((((~ (IData)(vlSelfRef.test_lane_permission__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_lane_permission__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.test_lane_permission__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_lane_permission__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_lane_permission__DOT__clk__0 
        = vlSelfRef.test_lane_permission__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_lane_permission__DOT__rst_n__0 
        = vlSelfRef.test_lane_permission__DOT__rst_n;
}

bool Vtest_lane_permission___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___trigger_anySet__act\n"); );
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

void Vtest_lane_permission___024root___act_comb__TOP__0(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___act_comb__TOP__0\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction;
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction = 0;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering;
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering = 0;
    // Body
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_code_changed 
        = ((IData)(vlSelfRef.test_lane_permission__DOT__hmi) 
           != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__previous_hmi_code));
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction 
        = ((0x0aU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
           | (0x0bU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)));
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering 
        = ((0x0cU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
           | (0x0dU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)));
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction 
        = ((1U == (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched)) 
           | ((2U == (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched)) 
              | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction) 
                 | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_hold) 
                    | (0x0eU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))))));
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering 
        = ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering) 
           | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering_hold) 
              | (0x0eU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))));
    vlSelfRef.test_lane_permission__DOT__go = (1U & 
                                               ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction)
                                                    ? 
                                                   (1U 
                                                    != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched))
                                                    : (IData)(vlSelfRef.test_lane_permission__DOT__fspi))));
    vlSelfRef.test_lane_permission__DOT__back = (1U 
                                                 & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                    | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction)
                                                        ? 
                                                       (2U 
                                                        != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched))
                                                        : 
                                                       ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                                        >> 1U))));
    vlSelfRef.test_lane_permission__DOT__left = (1U 
                                                 & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                    | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering)
                                                        ? 
                                                       ((0x0cU 
                                                         != (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
                                                        | ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                                                           & (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny)))
                                                        : 
                                                       ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                                        >> 2U))));
    vlSelfRef.test_lane_permission__DOT__right = (1U 
                                                  & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                     | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering)
                                                         ? 
                                                        ((0x0dU 
                                                          != (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
                                                         | ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                                                            & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny) 
                                                               >> 1U)))
                                                         : 
                                                        ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                                         >> 3U))));
}

void Vtest_lane_permission___024root___eval_act(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_act\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((5ULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_lane_permission___024root___act_comb__TOP__0(vlSelf);
    }
}

extern const VlUnpacked<CData/*1:0*/, 256> Vtest_lane_permission__ConstPool__TABLE_hc16a1441_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtest_lane_permission__ConstPool__TABLE_h81f3961f_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtest_lane_permission__ConstPool__TABLE_h4751f6bb_0;

void Vtest_lane_permission___024root___nba_sequent__TOP__0(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___nba_sequent__TOP__0\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    CData/*7:0*/ __Vdly__test_lane_permission__DOT__guard__DOT__candidate;
    __Vdly__test_lane_permission__DOT__guard__DOT__candidate = 0;
    CData/*3:0*/ __Vdly__test_lane_permission__DOT__guard__DOT__stable;
    __Vdly__test_lane_permission__DOT__guard__DOT__stable = 0;
    CData/*0:0*/ __Vdly__test_lane_permission__DOT__guard__DOT__alive;
    __Vdly__test_lane_permission__DOT__guard__DOT__alive = 0;
    CData/*2:0*/ __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_stable_count;
    __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_stable_count = 0;
    CData/*0:0*/ __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed;
    __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed = 0;
    // Body
    __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_stable_count 
        = vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_stable_count;
    __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed 
        = vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_press_consumed;
    __Vdly__test_lane_permission__DOT__guard__DOT__candidate 
        = vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate;
    __Vdly__test_lane_permission__DOT__guard__DOT__stable 
        = vlSelfRef.test_lane_permission__DOT__guard__DOT__stable;
    __Vdly__test_lane_permission__DOT__guard__DOT__alive 
        = vlSelfRef.test_lane_permission__DOT__guard__DOT__alive;
    if (vlSelfRef.test_lane_permission__DOT__rst_n) {
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__previous_hmi_code 
            = vlSelfRef.test_lane_permission__DOT__hmi;
        if (vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_code_changed) {
            __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_stable_count = 0U;
        } else if ((7U != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_stable_count))) {
            __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_stable_count 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_stable_count)));
        }
        if (vlSelfRef.test_lane_permission__DOT__stop) {
            vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched = 0U;
            __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed = 1U;
        } else if (vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_code_changed) {
            __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed = 0U;
        } else if (((7U == (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_stable_count)) 
                    & (~ (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_press_consumed)))) {
            __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed = 1U;
            if ((0x0aU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))) {
                vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched = 1U;
            } else if ((0x0bU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))) {
                vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched = 2U;
            }
        }
        if (((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__synced) 
             != (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate))) {
            __Vdly__test_lane_permission__DOT__guard__DOT__candidate 
                = vlSelfRef.test_lane_permission__DOT__guard__DOT__synced;
            __Vdly__test_lane_permission__DOT__guard__DOT__stable = 0U;
        } else if ((0x0fU != (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__stable))) {
            __Vdly__test_lane_permission__DOT__guard__DOT__stable 
                = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__stable)));
        }
        if ((0x000000c8U > vlSelfRef.test_lane_permission__DOT__guard__DOT__age)) {
            vlSelfRef.test_lane_permission__DOT__guard__DOT__age 
                = (0x01ffffffU & ((IData)(1U) + vlSelfRef.test_lane_permission__DOT__guard__DOT__age));
        } else {
            __Vdly__test_lane_permission__DOT__guard__DOT__alive = 0U;
            vlSelfRef.test_lane_permission__DOT__guard__DOT__deny = 0U;
        }
        if ((((8U == (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__stable)) 
              & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate) 
                 >> 6U)) & ((~ (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive)) 
                            | ((1U & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate) 
                                      >> 7U)) != (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__heartbeat))))) {
            vlSelfRef.test_lane_permission__DOT__guard__DOT__heartbeat 
                = (1U & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate) 
                         >> 7U));
            vlSelfRef.test_lane_permission__DOT__guard__DOT__age = 0U;
            __Vdly__test_lane_permission__DOT__guard__DOT__alive = 1U;
            vlSelfRef.test_lane_permission__DOT__guard__DOT__deny 
                = (3U & (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate));
        }
        if (((8U == (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__stable)) 
             & (~ ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate) 
                   >> 6U)))) {
            __Vdly__test_lane_permission__DOT__guard__DOT__alive = 0U;
            vlSelfRef.test_lane_permission__DOT__guard__DOT__deny = 0U;
        }
        vlSelfRef.test_lane_permission__DOT__guard__DOT__synced 
            = vlSelfRef.test_lane_permission__DOT__guard__DOT__meta;
        vlSelfRef.test_lane_permission__DOT__guard__DOT__meta 
            = vlSelfRef.test_lane_permission__DOT__permission;
    } else {
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__previous_hmi_code = 0U;
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched = 0U;
        __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_stable_count = 0U;
        __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed = 0U;
        __Vdly__test_lane_permission__DOT__guard__DOT__alive = 0U;
        __Vdly__test_lane_permission__DOT__guard__DOT__candidate = 0U;
        __Vdly__test_lane_permission__DOT__guard__DOT__stable = 0U;
        vlSelfRef.test_lane_permission__DOT__guard__DOT__age = 0U;
        vlSelfRef.test_lane_permission__DOT__guard__DOT__heartbeat = 0U;
        vlSelfRef.test_lane_permission__DOT__guard__DOT__deny = 0U;
        vlSelfRef.test_lane_permission__DOT__guard__DOT__synced = 0U;
        vlSelfRef.test_lane_permission__DOT__guard__DOT__meta = 0U;
    }
    __Vtableidx1 = (((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering) 
                     << 7U) | (((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                << 3U) | (((0x0eU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
                                           << 2U) | 
                                          (((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction) 
                                            << 1U) 
                                           | (IData)(vlSelfRef.test_lane_permission__DOT__rst_n)))));
    if ((1U & Vtest_lane_permission__ConstPool__TABLE_hc16a1441_0
         [__Vtableidx1])) {
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_hold 
            = Vtest_lane_permission__ConstPool__TABLE_h81f3961f_0
            [__Vtableidx1];
    }
    if ((2U & Vtest_lane_permission__ConstPool__TABLE_hc16a1441_0
         [__Vtableidx1])) {
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering_hold 
            = Vtest_lane_permission__ConstPool__TABLE_h4751f6bb_0
            [__Vtableidx1];
    }
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_stable_count 
        = __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_stable_count;
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__traction_press_consumed 
        = __Vdly__test_lane_permission__DOT__arbiter__DOT__traction_press_consumed;
    vlSelfRef.test_lane_permission__DOT__guard__DOT__candidate 
        = __Vdly__test_lane_permission__DOT__guard__DOT__candidate;
    vlSelfRef.test_lane_permission__DOT__guard__DOT__stable 
        = __Vdly__test_lane_permission__DOT__guard__DOT__stable;
    vlSelfRef.test_lane_permission__DOT__guard__DOT__alive 
        = __Vdly__test_lane_permission__DOT__guard__DOT__alive;
}

void Vtest_lane_permission___024root___nba_comb__TOP__0(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___nba_comb__TOP__0\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction 
        = ((0x0aU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
           | (0x0bU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)));
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering 
        = ((0x0cU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
           | (0x0dU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)));
}

void Vtest_lane_permission___024root___nba_comb__TOP__1(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___nba_comb__TOP__1\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction;
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction = 0;
    CData/*0:0*/ test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering;
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering = 0;
    // Body
    vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_code_changed 
        = ((IData)(vlSelfRef.test_lane_permission__DOT__hmi) 
           != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__previous_hmi_code));
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction 
        = ((1U == (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched)) 
           | ((2U == (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched)) 
              | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction) 
                 | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_hold) 
                    | (0x0eU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))))));
    test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering 
        = ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering) 
           | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering_hold) 
              | (0x0eU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))));
    vlSelfRef.test_lane_permission__DOT__go = (1U & 
                                               ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction)
                                                    ? 
                                                   (1U 
                                                    != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched))
                                                    : (IData)(vlSelfRef.test_lane_permission__DOT__fspi))));
    vlSelfRef.test_lane_permission__DOT__back = (1U 
                                                 & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                    | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction)
                                                        ? 
                                                       (2U 
                                                        != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched))
                                                        : 
                                                       ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                                        >> 1U))));
    vlSelfRef.test_lane_permission__DOT__left = (1U 
                                                 & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                    | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering)
                                                        ? 
                                                       ((0x0cU 
                                                         != (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
                                                        | ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                                                           & (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny)))
                                                        : 
                                                       ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                                        >> 2U))));
    vlSelfRef.test_lane_permission__DOT__right = (1U 
                                                  & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                     | ((IData)(test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering)
                                                         ? 
                                                        ((0x0dU 
                                                          != (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
                                                         | ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                                                            & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny) 
                                                               >> 1U)))
                                                         : 
                                                        ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                                         >> 3U))));
}

void Vtest_lane_permission___024root___eval_nba(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_nba\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction;
    __Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction = 0;
    CData/*0:0*/ __Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering;
    __Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering = 0;
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_lane_permission___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((5ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction 
            = ((0x0aU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
               | (0x0bU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)));
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering 
            = ((0x0cU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
               | (0x0dU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi)));
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_code_changed 
            = ((IData)(vlSelfRef.test_lane_permission__DOT__hmi) 
               != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__previous_hmi_code));
        __Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction 
            = ((1U == (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched)) 
               | ((2U == (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched)) 
                  | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction) 
                     | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_hold) 
                        | (0x0eU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))))));
        __Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering 
            = ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering) 
               | ((IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_steering_hold) 
                  | (0x0eU == (IData)(vlSelfRef.test_lane_permission__DOT__hmi))));
        vlSelfRef.test_lane_permission__DOT__go = (1U 
                                                   & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                                                      | (__Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction
                                                          ? 
                                                         (1U 
                                                          != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched))
                                                          : (IData)(vlSelfRef.test_lane_permission__DOT__fspi))));
        vlSelfRef.test_lane_permission__DOT__back = 
            (1U & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                   | (__Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_traction
                       ? (2U != (IData)(vlSelfRef.test_lane_permission__DOT__arbiter__DOT__hmi_traction_latched))
                       : ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                          >> 1U))));
        vlSelfRef.test_lane_permission__DOT__left = 
            (1U & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                   | (__Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering
                       ? ((0x0cU != (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
                          | ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                             & (IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny)))
                       : ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                          >> 2U))));
        vlSelfRef.test_lane_permission__DOT__right 
            = (1U & ((IData)(vlSelfRef.test_lane_permission__DOT__stop) 
                     | (__Vinline__nba_comb__TOP__1_test_lane_permission__DOT__arbiter__DOT__hmi_owns_steering
                         ? ((0x0dU != (IData)(vlSelfRef.test_lane_permission__DOT__hmi)) 
                            | ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__alive) 
                               & ((IData)(vlSelfRef.test_lane_permission__DOT__guard__DOT__deny) 
                                  >> 1U))) : ((IData)(vlSelfRef.test_lane_permission__DOT__fspi) 
                                              >> 3U))));
    }
}

void Vtest_lane_permission___024root___timing_ready(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___timing_ready\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready("@(posedge test_lane_permission.clk)");
    }
}

void Vtest_lane_permission___024root___timing_resume(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___timing_resume\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h15e6f7c2__0.moveToResumeQueue(
                                                          "@(posedge test_lane_permission.clk)");
    vlSelfRef.__VtrigSched_h15e6f7c2__0.resume("@(posedge test_lane_permission.clk)");
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_lane_permission___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_lane_permission___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_lane_permission___024root___eval_phase__act(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_phase__act\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_lane_permission___024root___eval_triggers_vec__act(vlSelf);
    Vtest_lane_permission___024root___timing_ready(vlSelf);
    Vtest_lane_permission___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_lane_permission___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_lane_permission___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_lane_permission___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_lane_permission___024root___timing_resume(vlSelf);
        Vtest_lane_permission___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_lane_permission___024root___eval_phase__inact(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_phase__inact\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_lane_permission___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_lane_permission___024root___eval_phase__nba(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_phase__nba\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_lane_permission___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_lane_permission___024root___eval_nba(vlSelf);
        Vtest_lane_permission___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_lane_permission___024root___eval(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_lane_permission___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 2, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 2, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_lane_permission___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_lane_permission.v", 2, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_lane_permission___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_lane_permission___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_lane_permission___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0(Vtest_lane_permission___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root____VbeforeTrig_h15e6f7c2__0\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((IData)(vlSelfRef.test_lane_permission__DOT__clk) 
                                  & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_lane_permission__DOT__clk__0)))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_lane_permission__DOT__clk__0 
        = vlSelfRef.test_lane_permission__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h15e6f7c2__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_lane_permission___024root___eval_debug_assertions(Vtest_lane_permission___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_lane_permission___024root___eval_debug_assertions\n"); );
    Vtest_lane_permission__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
