// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_gear_feedback.h for the primary calling header

#include "Vtest_hmi_gear_feedback__pch.h"

VlCoroutine Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__0(Vtest_hmi_gear_feedback___024root* vlSelf);
VlCoroutine Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__1(Vtest_hmi_gear_feedback___024root* vlSelf);
VlCoroutine Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__2(Vtest_hmi_gear_feedback___024root* vlSelf);

void Vtest_hmi_gear_feedback___024root___eval_initial(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_initial\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__1(vlSelf);
    Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__2(vlSelf);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccc69a__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccd0ab__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bb97608__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bcac0b3__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0684b4__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0367bd__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0f6716__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c1a851e__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c2716f3__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__0(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__0__first;
    __Vtask_test_hmi_gear_feedback__DOT__frame__0__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__0__gear;
    __Vtask_test_hmi_gear_feedback__DOT__frame__0__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__flag;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__flag = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__2__first;
    __Vtask_test_hmi_gear_feedback__DOT__frame__2__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__2__gear;
    __Vtask_test_hmi_gear_feedback__DOT__frame__2__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__flag;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__flag = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__4__first;
    __Vtask_test_hmi_gear_feedback__DOT__frame__4__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__4__gear;
    __Vtask_test_hmi_gear_feedback__DOT__frame__4__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__5__first;
    __Vtask_test_hmi_gear_feedback__DOT__frame__5__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__5__gear;
    __Vtask_test_hmi_gear_feedback__DOT__frame__5__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__6__first;
    __Vtask_test_hmi_gear_feedback__DOT__frame__6__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__6__gear;
    __Vtask_test_hmi_gear_feedback__DOT__frame__6__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__7__first;
    __Vtask_test_hmi_gear_feedback__DOT__frame__7__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__7__gear;
    __Vtask_test_hmi_gear_feedback__DOT__frame__7__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__8__first;
    __Vtask_test_hmi_gear_feedback__DOT__frame__8__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__frame__8__gear;
    __Vtask_test_hmi_gear_feedback__DOT__frame__8__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first;
    __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__text_only__9__gear;
    __Vtask_test_hmi_gear_feedback__DOT__text_only__9__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__flag;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__flag = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first;
    __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__text_only__11__gear;
    __Vtask_test_hmi_gear_feedback__DOT__text_only__11__gear = 0;
    IData/*31:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first = 0;
    CData/*7:0*/ __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__flag;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__flag = 0;
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                         45);
    vlSelfRef.test_hmi_gear_feedback__DOT__rst_n = 1U;
    while ((0x00000018U != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccc69a__0(vlSelf, 
                                                                      "@( (32'sh18 == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5bccc69a__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'sh18 == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             45);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 9U;
    while ((0x0000003dU != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccd0ab__0(vlSelf, 
                                                                      "@( (32'sh3d == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5bccd0ab__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'sh3d == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             45);
    }
    __Vtask_test_hmi_gear_feedback__DOT__frame__0__gear = 0x3fU;
    __Vtask_test_hmi_gear_feedback__DOT__frame__0__first = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__0__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__0__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] = 0x70616765U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
        = (0x00ffffffU | (((0x3fU == (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__0__gear))
                            ? 0x30U : (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__0__gear)) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] = 0x76616c3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] = 0x75616c2eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] = 0x72416374U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] = 0x61476561U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] = 0x65302e76U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] = 0x00706167U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0018U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0019U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0020U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0021U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0022U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0023U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0024U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0025U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0026U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0027U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0028U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0029U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0030U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          24\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0031U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          25\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0032U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          26\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__0__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__0__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          10: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000aU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          11: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000bU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          12: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000cU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          13: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000dU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          14: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000eU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          15: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000fU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          16: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0010U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          17: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0011U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          18: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0012U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          19: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0013U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          20: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0014U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          21: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0015U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          22: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0016U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          23: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0017U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__0__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__flag = 0x30U;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first = 0x00000033U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
        = (0x00ffffffU | ((IData)(__Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__flag) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] = 0x41636b3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] = 0x00006170U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first)] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__1__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 8U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_active = 1U;
    while ((0x0000007aU != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bb97608__0(vlSelf, 
                                                                      "@( (32'sh7a == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5bb97608__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'sh7a == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             47);
    }
    __Vtask_test_hmi_gear_feedback__DOT__frame__2__gear = 0x31U;
    __Vtask_test_hmi_gear_feedback__DOT__frame__2__first = 0x0000003dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__2__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__2__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] = 0x70616765U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
        = (0x00ffffffU | (((0x3fU == (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__2__gear))
                            ? 0x30U : (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__2__gear)) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] = 0x76616c3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] = 0x75616c2eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] = 0x72416374U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] = 0x61476561U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] = 0x65302e76U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] = 0x00706167U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0018U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0019U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0020U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0021U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0022U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0023U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0024U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0025U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0026U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0027U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0028U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0029U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0030U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          24\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0031U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          25\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0032U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          26\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__2__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__2__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          10: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000aU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          11: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000bU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          12: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000cU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          13: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000dU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          14: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000eU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          15: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000fU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          16: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0010U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          17: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0011U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          18: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0012U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          19: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0013U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          20: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0014U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          21: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0015U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          22: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0016U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          23: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0017U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__2__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__flag = 0x31U;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first = 0x00000070U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
        = (0x00ffffffU | ((IData)(__Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__flag) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] = 0x41636b3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] = 0x00006170U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first)] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__3__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 9U;
    while ((0x000000b7U != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bcac0b3__0(vlSelf, 
                                                                      "@( (32'shb7 == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5bcac0b3__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'shb7 == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             48);
    }
    __Vtask_test_hmi_gear_feedback__DOT__frame__4__gear = 0x32U;
    __Vtask_test_hmi_gear_feedback__DOT__frame__4__first = 0x0000007aU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__4__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__4__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] = 0x70616765U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
        = (0x00ffffffU | (((0x3fU == (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__4__gear))
                            ? 0x30U : (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__4__gear)) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] = 0x76616c3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] = 0x75616c2eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] = 0x72416374U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] = 0x61476561U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] = 0x65302e76U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] = 0x00706167U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0018U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0019U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0020U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0021U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0022U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0023U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0024U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0025U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0026U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0027U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0028U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0029U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0030U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          24\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0031U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          25\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0032U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          26\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__4__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__4__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          10: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000aU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          11: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000bU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          12: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000cU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          13: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000dU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          14: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000eU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          15: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000fU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          16: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0010U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          17: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0011U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          18: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0012U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          19: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0013U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          20: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0014U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          21: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0015U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          22: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0016U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          23: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0017U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__4__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 0x0bU;
    while ((0x000000f4U != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0684b4__0(vlSelf, 
                                                                      "@( (32'shf4 == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5c0684b4__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'shf4 == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             49);
    }
    __Vtask_test_hmi_gear_feedback__DOT__frame__5__gear = 0x33U;
    __Vtask_test_hmi_gear_feedback__DOT__frame__5__first = 0x000000b7U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__5__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__5__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] = 0x70616765U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
        = (0x00ffffffU | (((0x3fU == (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__5__gear))
                            ? 0x30U : (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__5__gear)) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] = 0x76616c3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] = 0x75616c2eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] = 0x72416374U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] = 0x61476561U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] = 0x65302e76U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] = 0x00706167U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0018U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0019U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0020U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0021U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0022U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0023U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0024U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0025U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0026U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0027U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0028U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0029U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0030U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          24\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0031U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          25\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0032U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          26\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__5__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__5__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          10: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000aU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          11: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000bU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          12: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000cU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          13: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000dU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          14: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000eU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          15: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000fU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          16: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0010U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          17: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0011U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          18: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0012U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          19: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0013U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          20: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0014U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          21: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0015U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          22: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0016U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          23: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0017U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__5__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 0x0aU;
    while ((0x00000131U != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0367bd__0(vlSelf, 
                                                                      "@( (32'sh131 == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5c0367bd__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'sh131 == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             50);
    }
    __Vtask_test_hmi_gear_feedback__DOT__frame__6__gear = 0x31U;
    __Vtask_test_hmi_gear_feedback__DOT__frame__6__first = 0x000000f4U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__6__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__6__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] = 0x70616765U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
        = (0x00ffffffU | (((0x3fU == (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__6__gear))
                            ? 0x30U : (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__6__gear)) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] = 0x76616c3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] = 0x75616c2eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] = 0x72416374U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] = 0x61476561U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] = 0x65302e76U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] = 0x00706167U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0018U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0019U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0020U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0021U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0022U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0023U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0024U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0025U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0026U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0027U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0028U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0029U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0030U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          24\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0031U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          25\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0032U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          26\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__6__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__6__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          10: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000aU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          11: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000bU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          12: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000cU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          13: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000dU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          14: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000eU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          15: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000fU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          16: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0010U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          17: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0011U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          18: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0012U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          19: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0013U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          20: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0014U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          21: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0015U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          22: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0016U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          23: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0017U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__6__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 0x0eU;
    while ((0x0000016eU != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0f6716__0(vlSelf, 
                                                                      "@( (32'sh16e == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5c0f6716__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'sh16e == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             51);
    }
    __Vtask_test_hmi_gear_feedback__DOT__frame__7__gear = 0x32U;
    __Vtask_test_hmi_gear_feedback__DOT__frame__7__first = 0x00000131U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__7__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__7__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] = 0x70616765U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
        = (0x00ffffffU | (((0x3fU == (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__7__gear))
                            ? 0x30U : (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__7__gear)) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] = 0x76616c3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] = 0x75616c2eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] = 0x72416374U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] = 0x61476561U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] = 0x65302e76U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] = 0x00706167U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0018U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0019U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0020U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0021U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0022U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0023U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0024U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0025U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0026U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0027U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0028U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0029U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0030U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          24\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0031U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          25\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0032U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          26\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__7__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__7__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          10: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000aU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          11: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000bU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          12: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000cU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          13: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000dU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          14: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000eU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          15: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000fU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          16: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0010U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          17: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0011U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          18: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0012U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          19: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0013U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          20: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0014U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          21: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0015U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          22: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0016U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          23: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0017U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__7__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 0x0cU;
    while ((0x000001abU != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c1a851e__0(vlSelf, 
                                                                      "@( (32'sh1ab == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5c1a851e__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'sh1ab == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             52);
    }
    __Vtask_test_hmi_gear_feedback__DOT__frame__8__gear = 0x33U;
    __Vtask_test_hmi_gear_feedback__DOT__frame__8__first = 0x0000016eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__8__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__8__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] = 0x70616765U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
        = (0x00ffffffU | (((0x3fU == (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__8__gear))
                            ? 0x30U : (IData)(__Vtask_test_hmi_gear_feedback__DOT__frame__8__gear)) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] = 0x76616c3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] = 0x75616c2eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] = 0x72416374U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] = 0x61476561U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] = 0x65302e76U;
    vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] = 0x00706167U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0018U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0019U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[6U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x001fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0020U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0021U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0022U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0023U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0024U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0025U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0026U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0027U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0028U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0029U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x002fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0030U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          24\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0031U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          25\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0032U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__number[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:21: Assertion failed in %m: slider actual numeric byte          26\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 21, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__8__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__frame__8__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          10: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000aU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          11: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000bU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          12: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000cU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          13: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000dU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          14: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000eU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          15: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x000fU) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          16: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0010U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          17: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0011U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          18: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0012U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          19: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0013U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          20: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0014U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          21: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0015U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          22: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0016U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:24: Assertion failed in %m: HMI status frame byte          23: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(0x0017U) 
                                      + __Vtask_test_hmi_gear_feedback__DOT__frame__8__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 24, "", false);
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__slider_present = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_active = 0U;
    while ((0x000001efU != vlSelfRef.test_hmi_gear_feedback__DOT__count)) {
        Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c2716f3__0(vlSelf, 
                                                                      "@( (32'sh1ef == test_hmi_gear_feedback.count))");
        co_await vlSelfRef.__VtrigSched_h5c2716f3__0.trigger(1U, 
                                                             nullptr, 
                                                             "@( (32'sh1ef == test_hmi_gear_feedback.count))", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                                             54);
    }
    __Vtask_test_hmi_gear_feedback__DOT__text_only__9__gear = 0x33U;
    __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first = 0x000001abU;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__text_only__9__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__text_only__9__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] = 0x70616765U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__9__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__flag = 0x30U;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first = 0x000001c3U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
        = (0x00ffffffU | ((IData)(__Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__flag) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] = 0x41636b3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] = 0x00006170U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first)] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__10__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    __Vtask_test_hmi_gear_feedback__DOT__text_only__11__gear = 0x33U;
    __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first = 0x000001cdU;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
        = (IData)((0x0000b5b522ffffffULL | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__text_only__11__gear)) 
                                            << 0x00000030U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
        = (0x22000000U | (IData)(((0x0000b5b522ffffffULL 
                                   | ((QData)((IData)(__Vtask_test_hmi_gear_feedback__DOT__text_only__11__gear)) 
                                      << 0x00000030U)) 
                                  >> 0x00000020U)));
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] = 0x7478743dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] = 0x6561722eU;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] = 0x302e7447U;
    vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] = 0x70616765U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first)] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           0\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           1\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           2\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[5U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           3\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           4\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           5\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           6\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           7\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           8\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte           9\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000aU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          10\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000bU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          11\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000cU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          12\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000dU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          13\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000eU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          14\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x000fU) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          15\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0010U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          16\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0011U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          17\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0012U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          18\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0013U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          19\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0014U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          20\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0015U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          21\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0016U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          22\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(0x0017U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__text_only__11__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__text_only__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:41: Assertion failed in %m: legacy TFT byte          23\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback.text_only", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 41, "", false);
    }
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__flag = 0x30U;
    __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first = 0x000001e5U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
        = (0x00ffffffU | ((IData)(__Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__flag) 
                          << 0x00000018U));
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] = 0x41636b3dU;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] = 0x00006170U;
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first)] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           0: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first)]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(1U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           1: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(1U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(2U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           2: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(2U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(3U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           3: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(3U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(4U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           4: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(4U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(5U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[1U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           5: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(5U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(6U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                          >> 0x00000018U))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           6: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(6U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(7U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 0x00000010U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           7: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(7U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(8U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (0x000000ffU & (vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U] 
                                         >> 8U)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           8: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(8U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    if (VL_UNLIKELY(((vlSelfRef.test_hmi_gear_feedback__DOT__captured
                      [(0x000003ffU & ((IData)(9U) 
                                       + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))] 
                      != (0x000000ffU & vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected[0U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:33: Assertion failed in %m: Autopilot acknowledgment byte           9: %h\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback.autopilot_frame", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_gear_feedback__DOT__captured
                     [(0x000003ffU & ((IData)(9U) + __Vtask_test_hmi_gear_feedback__DOT__autopilot_frame__12__first))]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 33, "", false);
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                         56);
    vlSelfRef.test_hmi_gear_feedback__DOT__rst_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                         56);
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__txd)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:56: Assertion failed in %m: reset must release UART TX high\n",3, 'M',vlSymsp->name(),"test_hmi_gear_feedback", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 56, "", false);
    }
    VL_WRITEF_NX("HMI feedback: GB2312 gear, optional slider, Autopilot acknowledgment and reset PASS\n",0);
    VL_FINISH_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 57, "");
    co_return;
}

VlCoroutine Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__1(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000000077359400ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                         59);
    VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_gear_feedback.v:59: Assertion failed in %m: HMI UART feedback timeout count=%d\n",4, 'M',vlSymsp->name(),"test_hmi_gear_feedback", 'T',-9
                 , '#',64,VL_TIME_UNITED_Q(1000), '~',32,vlSelfRef.test_hmi_gear_feedback__DOT__count);
    VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 59, "", false);
    co_return;
}

VlCoroutine Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__2(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_initial__TOP__Vtiming__2\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 
                                             3);
        vlSelfRef.test_hmi_gear_feedback__DOT__clk 
            = (1U & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__clk)));
    }
    co_return;
}

void Vtest_hmi_gear_feedback___024root___eval_triggers_vec__act(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_triggers_vec__act\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vtrigprevexpr_hcbfad22d__0;
    __Vtrigprevexpr_hcbfad22d__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcbfae7c2__0;
    __Vtrigprevexpr_hcbfae7c2__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcc0e817f__0;
    __Vtrigprevexpr_hcc0e817f__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcbfcd82a__0;
    __Vtrigprevexpr_hcbfcd82a__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcc309423__0;
    __Vtrigprevexpr_hcc309423__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcc347338__0;
    __Vtrigprevexpr_hcc347338__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcc287291__0;
    __Vtrigprevexpr_hcc287291__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcc2c9499__0;
    __Vtrigprevexpr_hcc2c9499__0 = 0;
    CData/*0:0*/ __Vtrigprevexpr_hcc20226a__0;
    __Vtrigprevexpr_hcc20226a__0 = 0;
    // Body
    __Vtrigprevexpr_hcbfad22d__0 = (0x00000018U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcbfae7c2__0 = (0x0000003dU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcc0e817f__0 = (0x0000007aU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcbfcd82a__0 = (0x000000b7U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcc309423__0 = (0x000000f4U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcc347338__0 = (0x00000131U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcc287291__0 = (0x0000016eU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcc2c9499__0 = (0x000001abU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __Vtrigprevexpr_hcc20226a__0 = (0x000001efU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((((IData)(__Vtrigprevexpr_hcc20226a__0) 
                                                          != (IData)(vlSelfRef.__Vtrigprevexpr_hcc20226a__1)) 
                                                         << 3U) 
                                                        | (((IData)(__Vtrigprevexpr_hcc2c9499__0) 
                                                            != (IData)(vlSelfRef.__Vtrigprevexpr_hcc2c9499__1)) 
                                                           << 2U)) 
                                                       | ((((IData)(__Vtrigprevexpr_hcc287291__0) 
                                                            != (IData)(vlSelfRef.__Vtrigprevexpr_hcc287291__1)) 
                                                           << 1U) 
                                                          | ((IData)(__Vtrigprevexpr_hcc347338__0) 
                                                             != (IData)(vlSelfRef.__Vtrigprevexpr_hcc347338__1)))) 
                                                      << 8U) 
                                                     | (((((((IData)(__Vtrigprevexpr_hcc309423__0) 
                                                             != (IData)(vlSelfRef.__Vtrigprevexpr_hcc309423__1)) 
                                                            << 3U) 
                                                           | (((IData)(__Vtrigprevexpr_hcbfcd82a__0) 
                                                               != (IData)(vlSelfRef.__Vtrigprevexpr_hcbfcd82a__1)) 
                                                              << 2U)) 
                                                          | ((((IData)(__Vtrigprevexpr_hcc0e817f__0) 
                                                               != (IData)(vlSelfRef.__Vtrigprevexpr_hcc0e817f__1)) 
                                                              << 1U) 
                                                             | ((IData)(__Vtrigprevexpr_hcbfae7c2__0) 
                                                                != (IData)(vlSelfRef.__Vtrigprevexpr_hcbfae7c2__1)))) 
                                                         << 4U) 
                                                        | (((((IData)(__Vtrigprevexpr_hcbfad22d__0) 
                                                              != (IData)(vlSelfRef.__Vtrigprevexpr_hcbfad22d__1)) 
                                                             << 3U) 
                                                            | (vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                               << 2U)) 
                                                           | ((((~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n)) 
                                                                & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__rst_n__0)) 
                                                               << 1U) 
                                                              | ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__clk) 
                                                                 & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__clk__0)))))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__clk__0 
        = vlSelfRef.test_hmi_gear_feedback__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__rst_n__0 
        = vlSelfRef.test_hmi_gear_feedback__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr_hcbfad22d__1 = __Vtrigprevexpr_hcbfad22d__0;
    vlSelfRef.__Vtrigprevexpr_hcbfae7c2__1 = __Vtrigprevexpr_hcbfae7c2__0;
    vlSelfRef.__Vtrigprevexpr_hcc0e817f__1 = __Vtrigprevexpr_hcc0e817f__0;
    vlSelfRef.__Vtrigprevexpr_hcbfcd82a__1 = __Vtrigprevexpr_hcbfcd82a__0;
    vlSelfRef.__Vtrigprevexpr_hcc309423__1 = __Vtrigprevexpr_hcc309423__0;
    vlSelfRef.__Vtrigprevexpr_hcc347338__1 = __Vtrigprevexpr_hcc347338__0;
    vlSelfRef.__Vtrigprevexpr_hcc287291__1 = __Vtrigprevexpr_hcc287291__0;
    vlSelfRef.__Vtrigprevexpr_hcc2c9499__1 = __Vtrigprevexpr_hcc2c9499__0;
    vlSelfRef.__Vtrigprevexpr_hcc20226a__1 = __Vtrigprevexpr_hcc20226a__0;
}

bool Vtest_hmi_gear_feedback___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___trigger_anySet__act\n"); );
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

void Vtest_hmi_gear_feedback___024root___nba_sequent__TOP__0(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___nba_sequent__TOP__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__Vfuncout;
    __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__Vfuncout = 0;
    CData/*3:0*/ __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value;
    __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value = 0;
    CData/*7:0*/ __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__Vfuncout;
    __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__Vfuncout = 0;
    CData/*5:0*/ __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n;
    __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n = 0;
    CData/*7:0*/ __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__gear;
    __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__gear = 0;
    CData/*0:0*/ __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__active;
    __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__active = 0;
    IData/*23:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__refresh_count;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__refresh_count = 0;
    CData/*5:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index = 0;
    CData/*7:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__gear_character;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__gear_character = 0;
    CData/*1:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 0;
    CData/*0:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot = 0;
    CData/*0:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot = 0;
    SData/*15:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__byte_gap;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__byte_gap = 0;
    CData/*7:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t = 0;
    CData/*0:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx_busy;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx_busy = 0;
    SData/*15:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt = 0;
    CData/*3:0*/ __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt = 0;
    CData/*7:0*/ __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t;
    __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t = 0;
    VlWide<5>/*159:0*/ __Vtemp_1;
    VlWide<6>/*191:0*/ __Vtemp_2;
    // Body
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt;
    vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__data 
        = vlSelfRef.test_hmi_gear_feedback__DOT__data;
    vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt 
        = vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt;
    vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt 
        = vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt;
    __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t 
        = vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__refresh_count 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__refresh_count;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__index;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__gear_character 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_character;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__state;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__byte_gap 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__byte_gap;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx_busy 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t 
        = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t;
    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx_busy 
        = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
           && ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_enable) 
               || ((1U & (~ ((9U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt)) 
                             & (0x003cU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt))))) 
                   && (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy))));
    if (vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) {
        if (vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy) {
            __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt 
                = ((0x003fU > (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt))
                    ? (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt)))
                    : 0U);
            if ((0x003fU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt))) {
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt 
                    = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt)));
            }
        } else {
            __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt = 0U;
            __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt = 0U;
        }
        vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__data 
            = (((9U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt)) 
                & (0x001fU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt)))
                ? (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t)
                : (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__data));
        if (vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_flag) {
            vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt 
                = ((0x003fU > (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt))
                    ? (0x0000ffffU & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt)))
                    : 0U);
            if ((0x003fU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt))) {
                vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt 
                    = (0x0000000fU & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt)));
            }
            if ((0x001fU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt))) {
                if ((8U & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt))) {
                    if ((1U & (~ ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                  >> 2U)))) {
                        if ((1U & (~ ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                      >> 1U)))) {
                            if ((1U & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt)))) {
                                __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t 
                                    = (((~ ((IData)(1U) 
                                            << (7U 
                                                & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                   - (IData)(1U))))) 
                                        & (IData)(__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t)) 
                                       | (0x00ffU & 
                                          ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2) 
                                           << (7U & 
                                               ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                - (IData)(1U))))));
                            }
                        }
                    }
                } else if ((4U & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt))) {
                    __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t 
                        = (((~ ((IData)(1U) << (7U 
                                                & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                   - (IData)(1U))))) 
                            & (IData)(__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t)) 
                           | (0x00ffU & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2) 
                                         << (7U & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                   - (IData)(1U))))));
                } else if ((2U & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt))) {
                    __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t 
                        = (((~ ((IData)(1U) << (7U 
                                                & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                   - (IData)(1U))))) 
                            & (IData)(__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t)) 
                           | (0x00ffU & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2) 
                                         << (7U & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                   - (IData)(1U))))));
                } else if ((1U & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt))) {
                    __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t 
                        = (((~ ((IData)(1U) << (7U 
                                                & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                   - (IData)(1U))))) 
                            & (IData)(__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t)) 
                           | (0x00ffU & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2) 
                                         << (7U & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt) 
                                                   - (IData)(1U))))));
                }
            } else {
                __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t 
                    = vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t;
            }
        } else {
            vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt = 0U;
            vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt = 0U;
            __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t = 0U;
        }
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t 
            = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_enable)
                ? (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_data)
                : (((9U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt)) 
                    & (0x003cU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt)))
                    ? 0U : (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t)));
        vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_enable = 0U;
        if ((0U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__state))) {
            if ((0x00004e1fU <= vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__refresh_count)) {
                __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value 
                    = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_sync;
                __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__Vfuncout 
                    = ((8U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                        ? ((4U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                            ? ((2U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                                ? ((1U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                                    ? 0x3fU : 0x32U)
                                : ((1U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                                    ? 0x3fU : 0x33U))
                            : ((2U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                                ? ((1U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                                    ? 0x33U : 0x31U)
                                : ((1U & (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__value))
                                    ? 0x32U : 0x31U)))
                        : 0x3fU);
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__refresh_count = 0U;
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index = 0U;
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__gear_character 
                    = __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__gear_ascii__13__Vfuncout;
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 1U;
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot 
                    = vlSelfRef.test_hmi_gear_feedback__DOT__slider_present;
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot 
                    = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__autopilot_sync;
            } else {
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__refresh_count 
                    = (0x00ffffffU & ((IData)(1U) + vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__refresh_count));
            }
        } else if ((1U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__state))) {
            if ((1U & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy)))) {
                __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__active 
                    = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot;
                __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__gear 
                    = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_character;
                __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n 
                    = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__index;
                vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_enable = 1U;
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 2U;
                __Vtemp_1[0U] = 0x78743d22U;
                __Vtemp_1[1U] = 0x61722e74U;
                __Vtemp_1[2U] = 0x2e744765U;
                __Vtemp_1[3U] = 0x61676530U;
                __Vtemp_1[4U] = 0x00000070U;
                __Vtemp_2[0U] = 0x76616c3dU;
                __Vtemp_2[1U] = 0x75616c2eU;
                __Vtemp_2[2U] = 0x72416374U;
                __Vtemp_2[3U] = 0x61476561U;
                __Vtemp_2[4U] = 0x65302e76U;
                __Vtemp_2[5U] = 0x00706167U;
                __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__Vfuncout 
                    = (0x000000ffU & ((0x11U > (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n))
                                       ? ((0x87U >= 
                                           (0x000000ffU 
                                            & (((IData)(0x87U) 
                                                - VL_SHIFTL_III(8,32,32, (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n), 3U)) 
                                               - (IData)(7U))))
                                           ? (((0U 
                                                == 
                                                (0x0000001fU 
                                                 & (((IData)(0x87U) 
                                                     - 
                                                     VL_SHIFTL_III(8,32,32, (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n), 3U)) 
                                                    - (IData)(7U))))
                                                ? 0U
                                                : (__Vtemp_1
                                                   [
                                                   (((IData)(7U) 
                                                     + 
                                                     (0x000000ffU 
                                                      & (((IData)(0x87U) 
                                                          - 
                                                          VL_SHIFTL_III(8,32,32, (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n), 3U)) 
                                                         - (IData)(7U)))) 
                                                    >> 5U)] 
                                                   << 
                                                   ((IData)(0x00000020U) 
                                                    - 
                                                    (0x0000001fU 
                                                     & (((IData)(0x87U) 
                                                         - 
                                                         VL_SHIFTL_III(8,32,32, (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n), 3U)) 
                                                        - (IData)(7U)))))) 
                                              | (__Vtemp_1
                                                 [(7U 
                                                   & ((((IData)(0x87U) 
                                                        - 
                                                        VL_SHIFTL_III(8,32,32, (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n), 3U)) 
                                                       - (IData)(7U)) 
                                                      >> 5U))] 
                                                 >> 
                                                 (0x0000001fU 
                                                  & (((IData)(0x87U) 
                                                      - 
                                                      VL_SHIFTL_III(8,32,32, (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n), 3U)) 
                                                     - (IData)(7U)))))
                                           : 0U) : 
                                      (((0x18U <= (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n)) 
                                        & (0x2eU >= (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n)))
                                        ? ((0xb7U >= 
                                            (0x000000ffU 
                                             & (((IData)(0xb7U) 
                                                 - 
                                                 VL_SHIFTL_III(8,32,32, 
                                                               ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                - (IData)(0x00000018U)), 3U)) 
                                                - (IData)(7U))))
                                            ? (((0U 
                                                 == 
                                                 (0x0000001fU 
                                                  & (((IData)(0xb7U) 
                                                      - 
                                                      VL_SHIFTL_III(8,32,32, 
                                                                    ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                     - (IData)(0x00000018U)), 3U)) 
                                                     - (IData)(7U))))
                                                 ? 0U
                                                 : 
                                                (__Vtemp_2
                                                 [(
                                                   ((IData)(7U) 
                                                    + 
                                                    (0x000000ffU 
                                                     & (((IData)(0xb7U) 
                                                         - 
                                                         VL_SHIFTL_III(8,32,32, 
                                                                       ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                        - (IData)(0x00000018U)), 3U)) 
                                                        - (IData)(7U)))) 
                                                   >> 5U)] 
                                                 << 
                                                 ((IData)(0x00000020U) 
                                                  - 
                                                  (0x0000001fU 
                                                   & (((IData)(0xb7U) 
                                                       - 
                                                       VL_SHIFTL_III(8,32,32, 
                                                                     ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                      - (IData)(0x00000018U)), 3U)) 
                                                      - (IData)(7U)))))) 
                                               | (__Vtemp_2
                                                  [
                                                  (7U 
                                                   & ((((IData)(0xb7U) 
                                                        - 
                                                        VL_SHIFTL_III(8,32,32, 
                                                                      ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                       - (IData)(0x00000018U)), 3U)) 
                                                       - (IData)(7U)) 
                                                      >> 5U))] 
                                                  >> 
                                                  (0x0000001fU 
                                                   & (((IData)(0xb7U) 
                                                       - 
                                                       VL_SHIFTL_III(8,32,32, 
                                                                     ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                      - (IData)(0x00000018U)), 3U)) 
                                                      - (IData)(7U)))))
                                            : 0U) : 
                                       (((0x33U <= (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n)) 
                                         & (0x38U >= (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n)))
                                         ? ((0x2fU 
                                             >= (0x0000003fU 
                                                 & (((IData)(0x2fU) 
                                                     - 
                                                     VL_SHIFTL_III(6,32,32, 
                                                                   ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                    - (IData)(0x00000033U)), 3U)) 
                                                    - (IData)(7U))))
                                             ? (IData)(
                                                       (0x0000617041636b3dULL 
                                                        >> 
                                                        (0x0000003fU 
                                                         & (((IData)(0x2fU) 
                                                             - 
                                                             VL_SHIFTL_III(6,32,32, 
                                                                           ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n) 
                                                                            - (IData)(0x00000033U)), 3U)) 
                                                            - (IData)(7U)))))
                                             : 0U) : 
                                        ((0x11U == (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n))
                                          ? (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__gear)
                                          : (((0x12U 
                                               == (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n)) 
                                              || (0x13U 
                                                  == (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n)))
                                              ? 0xb5U
                                              : ((0x14U 
                                                  == (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n))
                                                  ? 0x22U
                                                  : 
                                                 ((0x2fU 
                                                   == (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n))
                                                   ? 
                                                  ((0x3fU 
                                                    == (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__gear))
                                                    ? 0x30U
                                                    : (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__gear))
                                                   : 
                                                  ((0x39U 
                                                    == (IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__n))
                                                    ? 
                                                   ((IData)(__Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__active)
                                                     ? 0x31U
                                                     : 0x30U)
                                                    : 0xffU)))))))));
                vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_data 
                    = __Vfunc_test_hmi_gear_feedback__DOT__dut__DOT__frame_byte__14__Vfuncout;
            }
        } else if ((2U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__state))) {
            if (vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy) {
                __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 3U;
            }
        } else if ((3U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__state))) {
            if ((1U & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy)))) {
                if ((0x0040U <= (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__byte_gap))) {
                    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__byte_gap = 0U;
                    if ((0x3cU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__index))) {
                        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 0U;
                    } else if (((0x32U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__index)) 
                                | ((0x17U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__index)) 
                                   & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot))))) {
                        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index = 0x33U;
                        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 1U;
                    } else {
                        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index 
                            = (0x0000003fU & ((IData)(1U) 
                                              + (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__index)));
                        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 1U;
                    }
                } else {
                    __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__byte_gap 
                        = (0x0000ffffU & ((IData)(1U) 
                                          + (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__byte_gap)));
                }
            }
        }
        vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_sync 
            = vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_meta;
        vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_meta 
            = vlSelfRef.test_hmi_gear_feedback__DOT__status;
        vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_flag 
            = (((~ ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_flag) 
                    | (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d1))) 
                & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2)) 
               | ((~ ((9U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt)) 
                      & (0x001fU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt)))) 
                  & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_flag)));
    } else {
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt = 0U;
        vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__data = 0U;
        vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt = 0U;
        vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__refresh_count = 0x00004e1fU;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__byte_gap = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__gear_character = 0x3fU;
        vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_data = 0U;
        vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_enable = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot = 0U;
        __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot = 0U;
        vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_sync = 0U;
        vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_meta = 0U;
        vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_flag = 0U;
    }
    vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t 
        = __Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt;
    vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2 
        = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d1));
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__refresh_count 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__refresh_count;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__index 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__index;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__gear_character 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__gear_character;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__state 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__state;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__byte_gap 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__byte_gap;
    vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d1 
        = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d0));
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__autopilot_sync 
        = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__autopilot_meta));
    vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d0 
        = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__txd));
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__autopilot_meta 
        = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_active));
    vlSelfRef.test_hmi_gear_feedback__DOT__txd = ((1U 
                                                   & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n))) 
                                                  || ((1U 
                                                       & (~ (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy))) 
                                                      || (1U 
                                                          & ((8U 
                                                              & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt))
                                                              ? 
                                                             ((1U 
                                                               & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt) 
                                                                  >> 2U)) 
                                                              || ((1U 
                                                                   & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt) 
                                                                      >> 1U)) 
                                                                  || ((1U 
                                                                       & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt)) 
                                                                      || (1U 
                                                                          & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t) 
                                                                             >> 
                                                                             (7U 
                                                                              & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt) 
                                                                                - (IData)(1U))))))))
                                                              : 
                                                             ((4U 
                                                               & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt))
                                                               ? 
                                                              ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t) 
                                                               >> 
                                                               (7U 
                                                                & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt) 
                                                                   - (IData)(1U))))
                                                               : 
                                                              ((2U 
                                                                & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt))
                                                                ? 
                                                               ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t) 
                                                                >> 
                                                                (7U 
                                                                 & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt) 
                                                                    - (IData)(1U))))
                                                                : 
                                                               ((1U 
                                                                 & (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt)) 
                                                                && (1U 
                                                                    & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t) 
                                                                       >> 
                                                                       (7U 
                                                                        & ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt) 
                                                                           - (IData)(1U))))))))))));
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx_busy 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx_busy;
    vlSelfRef.test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt 
        = __Vdly__test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt;
}

void Vtest_hmi_gear_feedback___024root___nba_sequent__TOP__1(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___nba_sequent__TOP__1\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.test_hmi_gear_feedback__DOT__valid) {
        vlSelfRef.test_hmi_gear_feedback__DOT__captured[(0x000003ffU 
                                                         & vlSelfRef.test_hmi_gear_feedback__DOT__count)] 
            = vlSelfRef.test_hmi_gear_feedback__DOT__data;
        vlSelfRef.test_hmi_gear_feedback__DOT__count 
            = ((IData)(1U) + vlSelfRef.test_hmi_gear_feedback__DOT__count);
    }
}

void Vtest_hmi_gear_feedback___024root___nba_sequent__TOP__2(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___nba_sequent__TOP__2\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_gear_feedback__DOT__data = vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__data;
    vlSelfRef.test_hmi_gear_feedback__DOT__valid = 
        ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
         && ((9U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt)) 
             & (0x001fU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt))));
    vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt 
        = vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt;
    vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt 
        = vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt;
}

void Vtest_hmi_gear_feedback___024root___eval_nba(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_nba\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_hmi_gear_feedback___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if (vlSelfRef.test_hmi_gear_feedback__DOT__valid) {
            vlSelfRef.test_hmi_gear_feedback__DOT__captured[(0x000003ffU 
                                                             & vlSelfRef.test_hmi_gear_feedback__DOT__count)] 
                = vlSelfRef.test_hmi_gear_feedback__DOT__data;
            vlSelfRef.test_hmi_gear_feedback__DOT__count 
                = ((IData)(1U) + vlSelfRef.test_hmi_gear_feedback__DOT__count);
        }
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_gear_feedback__DOT__data 
            = vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__data;
        vlSelfRef.test_hmi_gear_feedback__DOT__valid 
            = ((IData)(vlSelfRef.test_hmi_gear_feedback__DOT__rst_n) 
               && ((9U == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt)) 
                   & (0x001fU == (IData)(vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt))));
        vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt 
            = vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt;
        vlSelfRef.test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt 
            = vlSelfRef.__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt;
    }
}

void Vtest_hmi_gear_feedback___024root___timing_ready(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___timing_ready\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5bccc69a__0.ready("@( (32'sh18 == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000010ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5bccd0ab__0.ready("@( (32'sh3d == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5bb97608__0.ready("@( (32'sh7a == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000040ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5bcac0b3__0.ready("@( (32'shb7 == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000080ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5c0684b4__0.ready("@( (32'shf4 == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000100ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5c0367bd__0.ready("@( (32'sh131 == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000200ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5c0f6716__0.ready("@( (32'sh16e == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000400ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5c1a851e__0.ready("@( (32'sh1ab == test_hmi_gear_feedback.count))");
    }
    if ((0x0000000000000800ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h5c2716f3__0.ready("@( (32'sh1ef == test_hmi_gear_feedback.count))");
    }
}

void Vtest_hmi_gear_feedback___024root___timing_resume(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___timing_resume\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h5bccc69a__0.moveToResumeQueue(
                                                          "@( (32'sh18 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5bccd0ab__0.moveToResumeQueue(
                                                          "@( (32'sh3d == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5bb97608__0.moveToResumeQueue(
                                                          "@( (32'sh7a == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5bcac0b3__0.moveToResumeQueue(
                                                          "@( (32'shb7 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c0684b4__0.moveToResumeQueue(
                                                          "@( (32'shf4 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c0367bd__0.moveToResumeQueue(
                                                          "@( (32'sh131 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c0f6716__0.moveToResumeQueue(
                                                          "@( (32'sh16e == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c1a851e__0.moveToResumeQueue(
                                                          "@( (32'sh1ab == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c2716f3__0.moveToResumeQueue(
                                                          "@( (32'sh1ef == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5bccc69a__0.resume("@( (32'sh18 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5bccd0ab__0.resume("@( (32'sh3d == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5bb97608__0.resume("@( (32'sh7a == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5bcac0b3__0.resume("@( (32'shb7 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c0684b4__0.resume("@( (32'shf4 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c0367bd__0.resume("@( (32'sh131 == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c0f6716__0.resume("@( (32'sh16e == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c1a851e__0.resume("@( (32'sh1ab == test_hmi_gear_feedback.count))");
    vlSelfRef.__VtrigSched_h5c2716f3__0.resume("@( (32'sh1ef == test_hmi_gear_feedback.count))");
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_hmi_gear_feedback___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_hmi_gear_feedback___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_hmi_gear_feedback___024root___eval_phase__act(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_phase__act\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_hmi_gear_feedback___024root___eval_triggers_vec__act(vlSelf);
    Vtest_hmi_gear_feedback___024root___timing_ready(vlSelf);
    Vtest_hmi_gear_feedback___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_gear_feedback___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_hmi_gear_feedback___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_hmi_gear_feedback___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_hmi_gear_feedback___024root___timing_resume(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_hmi_gear_feedback___024root___eval_phase__inact(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_phase__inact\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_hmi_gear_feedback___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_hmi_gear_feedback___024root___eval_phase__nba(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_phase__nba\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_hmi_gear_feedback___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_hmi_gear_feedback___024root___eval_nba(vlSelf);
        Vtest_hmi_gear_feedback___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_hmi_gear_feedback___024root___eval(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_gear_feedback___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 2, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 2, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_hmi_gear_feedback___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_gear_feedback.v", 2, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_hmi_gear_feedback___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_hmi_gear_feedback___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_hmi_gear_feedback___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccc69a__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccc69a__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcbfad22d__0;
    __Vtrigprevexpr_hcbfad22d__0 = 0;
    // Body
    __Vtrigprevexpr_hcbfad22d__0 = (0x00000018U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcbfad22d__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcbfad22d__1)) 
                                  << 3U)));
    vlSelfRef.__Vtrigprevexpr_hcbfad22d__1 = __Vtrigprevexpr_hcbfad22d__0;
    if ((8ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5bccc69a__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccd0ab__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bccd0ab__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcbfae7c2__0;
    __Vtrigprevexpr_hcbfae7c2__0 = 0;
    // Body
    __Vtrigprevexpr_hcbfae7c2__0 = (0x0000003dU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcbfae7c2__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcbfae7c2__1)) 
                                  << 4U)));
    vlSelfRef.__Vtrigprevexpr_hcbfae7c2__1 = __Vtrigprevexpr_hcbfae7c2__0;
    if ((0x0000000000000010ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5bccd0ab__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bb97608__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bb97608__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcc0e817f__0;
    __Vtrigprevexpr_hcc0e817f__0 = 0;
    // Body
    __Vtrigprevexpr_hcc0e817f__0 = (0x0000007aU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcc0e817f__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcc0e817f__1)) 
                                  << 5U)));
    vlSelfRef.__Vtrigprevexpr_hcc0e817f__1 = __Vtrigprevexpr_hcc0e817f__0;
    if ((0x0000000000000020ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5bb97608__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bcac0b3__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5bcac0b3__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcbfcd82a__0;
    __Vtrigprevexpr_hcbfcd82a__0 = 0;
    // Body
    __Vtrigprevexpr_hcbfcd82a__0 = (0x000000b7U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcbfcd82a__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcbfcd82a__1)) 
                                  << 6U)));
    vlSelfRef.__Vtrigprevexpr_hcbfcd82a__1 = __Vtrigprevexpr_hcbfcd82a__0;
    if ((0x0000000000000040ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5bcac0b3__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0684b4__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0684b4__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcc309423__0;
    __Vtrigprevexpr_hcc309423__0 = 0;
    // Body
    __Vtrigprevexpr_hcc309423__0 = (0x000000f4U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcc309423__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcc309423__1)) 
                                  << 7U)));
    vlSelfRef.__Vtrigprevexpr_hcc309423__1 = __Vtrigprevexpr_hcc309423__0;
    if ((0x0000000000000080ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5c0684b4__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0367bd__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0367bd__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcc347338__0;
    __Vtrigprevexpr_hcc347338__0 = 0;
    // Body
    __Vtrigprevexpr_hcc347338__0 = (0x00000131U == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcc347338__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcc347338__1)) 
                                  << 8U)));
    vlSelfRef.__Vtrigprevexpr_hcc347338__1 = __Vtrigprevexpr_hcc347338__0;
    if ((0x0000000000000100ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5c0367bd__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0f6716__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c0f6716__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcc287291__0;
    __Vtrigprevexpr_hcc287291__0 = 0;
    // Body
    __Vtrigprevexpr_hcc287291__0 = (0x0000016eU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcc287291__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcc287291__1)) 
                                  << 9U)));
    vlSelfRef.__Vtrigprevexpr_hcc287291__1 = __Vtrigprevexpr_hcc287291__0;
    if ((0x0000000000000200ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5c0f6716__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c1a851e__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c1a851e__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcc2c9499__0;
    __Vtrigprevexpr_hcc2c9499__0 = 0;
    // Body
    __Vtrigprevexpr_hcc2c9499__0 = (0x000001abU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcc2c9499__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcc2c9499__1)) 
                                  << 0x0000000aU)));
    vlSelfRef.__Vtrigprevexpr_hcc2c9499__1 = __Vtrigprevexpr_hcc2c9499__0;
    if ((0x0000000000000400ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5c1a851e__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c2716f3__0(Vtest_hmi_gear_feedback___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root____VbeforeTrig_h5c2716f3__0\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    CData/*0:0*/ __Vtrigprevexpr_hcc20226a__0;
    __Vtrigprevexpr_hcc20226a__0 = 0;
    // Body
    __Vtrigprevexpr_hcc20226a__0 = (0x000001efU == vlSelfRef.test_hmi_gear_feedback__DOT__count);
    __VTmp[0U] = (QData)((IData)((((IData)(__Vtrigprevexpr_hcc20226a__0) 
                                   != (IData)(vlSelfRef.__Vtrigprevexpr_hcc20226a__1)) 
                                  << 0x0000000bU)));
    vlSelfRef.__Vtrigprevexpr_hcc20226a__1 = __Vtrigprevexpr_hcc20226a__0;
    if ((0x0000000000000800ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h5c2716f3__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_hmi_gear_feedback___024root___eval_debug_assertions(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_debug_assertions\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
