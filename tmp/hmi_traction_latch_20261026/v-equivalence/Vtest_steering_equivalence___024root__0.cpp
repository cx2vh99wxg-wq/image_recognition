// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_steering_equivalence.h for the primary calling header

#include "Vtest_steering_equivalence__pch.h"

VlCoroutine Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__0(Vtest_steering_equivalence___024root* vlSelf);
VlCoroutine Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__1(Vtest_steering_equivalence___024root* vlSelf);

void Vtest_steering_equivalence___024root___eval_initial(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_initial\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtest_steering_equivalence___024root____VbeforeTrig_h931b9c32__0(Vtest_steering_equivalence___024root* vlSelf, const char* __VeventDescription);
void Vtest_steering_equivalence___024root____VbeforeTrig_h931b9f78__0(Vtest_steering_equivalence___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__0(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_steering_equivalence__DOT__i;
    test_steering_equivalence__DOT__i = 0;
    IData/*31:0*/ test_steering_equivalence__DOT__unnamedblk1_1__DOT____Vrepeat0;
    test_steering_equivalence__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ test_steering_equivalence__DOT__unnamedblk1_2__DOT____Vrepeat1;
    test_steering_equivalence__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    IData/*31:0*/ test_steering_equivalence__DOT__unnamedblk1_3__DOT____Vrepeat2;
    test_steering_equivalence__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0;
    // Body
    test_steering_equivalence__DOT__unnamedblk1_1__DOT____Vrepeat0 = 3U;
    while (VL_LTS_III(32, 0U, test_steering_equivalence__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_steering_equivalence___024root____VbeforeTrig_h931b9c32__0(vlSelf, 
                                                                         "@(negedge test_steering_equivalence.clk)");
        co_await vlSelfRef.__VtrigSched_h931b9c32__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_steering_equivalence.clk)", 
                                                             "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                                             28);
        test_steering_equivalence__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (test_steering_equivalence__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    vlSelfRef.test_steering_equivalence__DOT__rst_n = 1U;
    test_steering_equivalence__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000400U, test_steering_equivalence__DOT__i)) {
        Vtest_steering_equivalence___024root____VbeforeTrig_h931b9c32__0(vlSelf, 
                                                                         "@(negedge test_steering_equivalence.clk)");
        co_await vlSelfRef.__VtrigSched_h931b9c32__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_steering_equivalence.clk)", 
                                                             "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                                             32);
        vlSelfRef.test_steering_equivalence__DOT__hmi_code 
            = (0x0000000fU & test_steering_equivalence__DOT__i);
        vlSelfRef.test_steering_equivalence__DOT__fspi_released 
            = (0x0000000fU & (test_steering_equivalence__DOT__i 
                              >> 4U));
        vlSelfRef.test_steering_equivalence__DOT__armed 
            = (1U & (test_steering_equivalence__DOT__i 
                     >> 8U));
        vlSelfRef.test_steering_equivalence__DOT__blocked 
            = (1U & (test_steering_equivalence__DOT__i 
                     >> 9U));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                             34);
        if (VL_UNLIKELY((((((IData)(vlSelfRef.test_steering_equivalence__DOT__old_l) 
                            << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__old_r)) 
                          != (((IData)(vlSelfRef.test_steering_equivalence__DOT__new_l) 
                               << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__new_r)))))) {
            VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:21: Assertion failed in %m: steering changed at %0t HMI=%h FSPI=%h\n",6, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , '#',4,(IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)
                         , '#',4,vlSelfRef.test_steering_equivalence__DOT__fspi_released);
            VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 21, "", false);
        }
        if (VL_UNLIKELY(((((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction) 
                           & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_go))) 
                          & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_back)))))) {
            VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:23: Assertion failed in %m: mutually exclusive HMI traction outputs both asserted\n",3, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000));
            VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 23, "", false);
        }
        vlSelfRef.test_steering_equivalence__DOT__checks 
            = ((IData)(1U) + vlSelfRef.test_steering_equivalence__DOT__checks);
        test_steering_equivalence__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0x0000000cU;
        while (VL_LTS_III(32, 0U, test_steering_equivalence__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
            Vtest_steering_equivalence___024root____VbeforeTrig_h931b9f78__0(vlSelf, 
                                                                             "@(posedge test_steering_equivalence.clk)");
            co_await vlSelfRef.__VtrigSched_h931b9f78__0.trigger(0U, 
                                                                 nullptr, 
                                                                 "@(posedge test_steering_equivalence.clk)", 
                                                                 "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                                                 35);
            co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                                 nullptr, 
                                                 "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                                 35);
            if (VL_UNLIKELY((((((IData)(vlSelfRef.test_steering_equivalence__DOT__old_l) 
                                << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__old_r)) 
                              != (((IData)(vlSelfRef.test_steering_equivalence__DOT__new_l) 
                                   << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__new_r)))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:21: Assertion failed in %m: steering changed at %0t HMI=%h FSPI=%h\n",6, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',4,(IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)
                             , '#',4,vlSelfRef.test_steering_equivalence__DOT__fspi_released);
                VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 21, "", false);
            }
            if (VL_UNLIKELY(((((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction) 
                               & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_go))) 
                              & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_back)))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:23: Assertion failed in %m: mutually exclusive HMI traction outputs both asserted\n",3, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 23, "", false);
            }
            vlSelfRef.test_steering_equivalence__DOT__checks 
                = ((IData)(1U) + vlSelfRef.test_steering_equivalence__DOT__checks);
            test_steering_equivalence__DOT__unnamedblk1_2__DOT____Vrepeat1 
                = (test_steering_equivalence__DOT__unnamedblk1_2__DOT____Vrepeat1 
                   - (IData)(1U));
        }
        test_steering_equivalence__DOT__i = ((IData)(1U) 
                                             + test_steering_equivalence__DOT__i);
    }
    test_steering_equivalence__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00004e20U, test_steering_equivalence__DOT__i)) {
        Vtest_steering_equivalence___024root____VbeforeTrig_h931b9c32__0(vlSelf, 
                                                                         "@(negedge test_steering_equivalence.clk)");
        co_await vlSelfRef.__VtrigSched_h931b9c32__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_steering_equivalence.clk)", 
                                                             "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                                             39);
        vlSelfRef.test_steering_equivalence__DOT__rng 
            = (vlSelfRef.test_steering_equivalence__DOT__rng 
               ^ VL_SHIFTL_III(32,32,32, vlSelfRef.test_steering_equivalence__DOT__rng, 0x0000000dU));
        vlSelfRef.test_steering_equivalence__DOT__rng 
            = (vlSelfRef.test_steering_equivalence__DOT__rng 
               ^ VL_SHIFTR_III(32,32,32, vlSelfRef.test_steering_equivalence__DOT__rng, 0x00000011U));
        vlSelfRef.test_steering_equivalence__DOT__rng 
            = (vlSelfRef.test_steering_equivalence__DOT__rng 
               ^ VL_SHIFTL_III(32,32,32, vlSelfRef.test_steering_equivalence__DOT__rng, 5U));
        vlSelfRef.test_steering_equivalence__DOT__hmi_code 
            = (0x0000000fU & vlSelfRef.test_steering_equivalence__DOT__rng);
        vlSelfRef.test_steering_equivalence__DOT__fspi_released 
            = (0x0000000fU & (vlSelfRef.test_steering_equivalence__DOT__rng 
                              >> 4U));
        vlSelfRef.test_steering_equivalence__DOT__armed 
            = (1U & (vlSelfRef.test_steering_equivalence__DOT__rng 
                     >> 8U));
        vlSelfRef.test_steering_equivalence__DOT__blocked 
            = (1U & (vlSelfRef.test_steering_equivalence__DOT__rng 
                     >> 9U));
        vlSelfRef.test_steering_equivalence__DOT__hold_cycles 
            = ((IData)(1U) + (0x0000000fU & (vlSelfRef.test_steering_equivalence__DOT__rng 
                                             >> 0x0cU)));
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                             43);
        if (VL_UNLIKELY((((((IData)(vlSelfRef.test_steering_equivalence__DOT__old_l) 
                            << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__old_r)) 
                          != (((IData)(vlSelfRef.test_steering_equivalence__DOT__new_l) 
                               << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__new_r)))))) {
            VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:21: Assertion failed in %m: steering changed at %0t HMI=%h FSPI=%h\n",6, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , '#',4,(IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)
                         , '#',4,vlSelfRef.test_steering_equivalence__DOT__fspi_released);
            VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 21, "", false);
        }
        if (VL_UNLIKELY(((((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction) 
                           & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_go))) 
                          & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_back)))))) {
            VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:23: Assertion failed in %m: mutually exclusive HMI traction outputs both asserted\n",3, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000));
            VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 23, "", false);
        }
        vlSelfRef.test_steering_equivalence__DOT__checks 
            = ((IData)(1U) + vlSelfRef.test_steering_equivalence__DOT__checks);
        test_steering_equivalence__DOT__unnamedblk1_3__DOT____Vrepeat2 
            = vlSelfRef.test_steering_equivalence__DOT__hold_cycles;
        while (VL_LTS_III(32, 0U, test_steering_equivalence__DOT__unnamedblk1_3__DOT____Vrepeat2)) {
            Vtest_steering_equivalence___024root____VbeforeTrig_h931b9f78__0(vlSelf, 
                                                                             "@(posedge test_steering_equivalence.clk)");
            co_await vlSelfRef.__VtrigSched_h931b9f78__0.trigger(0U, 
                                                                 nullptr, 
                                                                 "@(posedge test_steering_equivalence.clk)", 
                                                                 "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                                                 44);
            co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                                 nullptr, 
                                                 "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                                 44);
            if (VL_UNLIKELY((((((IData)(vlSelfRef.test_steering_equivalence__DOT__old_l) 
                                << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__old_r)) 
                              != (((IData)(vlSelfRef.test_steering_equivalence__DOT__new_l) 
                                   << 1U) | (IData)(vlSelfRef.test_steering_equivalence__DOT__new_r)))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:21: Assertion failed in %m: steering changed at %0t HMI=%h FSPI=%h\n",6, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',64,VL_TIME_UNITED_Q(1000)
                             , '#',4,(IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)
                             , '#',4,vlSelfRef.test_steering_equivalence__DOT__fspi_released);
                VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 21, "", false);
            }
            if (VL_UNLIKELY(((((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction) 
                               & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_go))) 
                              & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__new_back)))))) {
                VL_WRITEF_NX("[%0t] %%Fatal: test_steering_equivalence.v:23: Assertion failed in %m: mutually exclusive HMI traction outputs both asserted\n",3, 'M',vlSymsp->name(),"test_steering_equivalence.check", 'T',-9
                             , '#',64,VL_TIME_UNITED_Q(1000));
                VL_STOP_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 23, "", false);
            }
            vlSelfRef.test_steering_equivalence__DOT__checks 
                = ((IData)(1U) + vlSelfRef.test_steering_equivalence__DOT__checks);
            test_steering_equivalence__DOT__unnamedblk1_3__DOT____Vrepeat2 
                = (test_steering_equivalence__DOT__unnamedblk1_3__DOT____Vrepeat2 
                   - (IData)(1U));
        }
        test_steering_equivalence__DOT__i = ((IData)(1U) 
                                             + test_steering_equivalence__DOT__i);
    }
    VL_WRITEF_NX("PASS: steering identical to prior working arbiter over 21024 scenarios, %0d samples; HMI traction mutually exclusive\n",1
                 , '~',32,vlSelfRef.test_steering_equivalence__DOT__checks);
    VL_FINISH_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 47, "");
    co_return;
}

VlCoroutine Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__1(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 
                                             3);
        vlSelfRef.test_steering_equivalence__DOT__clk 
            = (1U & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__clk)));
    }
    co_return;
}

void Vtest_steering_equivalence___024root___eval_triggers_vec__act(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_triggers_vec__act\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                       << 3U) 
                                                      | (((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__clk)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0)) 
                                                         << 2U)) 
                                                     | ((((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__rst_n)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__rst_n__0)) 
                                                         << 1U) 
                                                        | ((IData)(vlSelfRef.test_steering_equivalence__DOT__clk) 
                                                           & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0 
        = vlSelfRef.test_steering_equivalence__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__rst_n__0 
        = vlSelfRef.test_steering_equivalence__DOT__rst_n;
}

bool Vtest_steering_equivalence___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___trigger_anySet__act\n"); );
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

void Vtest_steering_equivalence___024root___act_comb__TOP__0(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___act_comb__TOP__0\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering;
    test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering = 0;
    CData/*0:0*/ test_steering_equivalence__DOT__current__DOT__hmi_owns_steering;
    test_steering_equivalence__DOT__current__DOT__hmi_owns_steering = 0;
    // Body
    vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_code_changed 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code) 
           != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__previous_hmi_code));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (1U 
                                                & ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                                                   | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                                                      >> 2U)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = (1U 
                                                & ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                                                   | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                                                      >> 3U)));
    vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction 
        = ((0x0aU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
           | (0x0bU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)));
    vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering 
        = ((0x0cU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
           | (0x0dU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)));
    vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction 
        = ((1U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched)) 
           | ((2U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched)) 
              | ((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction) 
                 | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_hold) 
                    | (0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = ((0x0eU 
                                                 == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
                                                | (IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering));
    vlSelfRef.test_steering_equivalence__DOT__new_go 
        = (1U & ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
                 | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction)
                     ? (1U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched))
                     : ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                        | (IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released)))));
    vlSelfRef.test_steering_equivalence__DOT__new_back 
        = (1U & ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
                 | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction)
                     ? (2U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched))
                     : ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                        | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                           >> 1U)))));
    test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering_hold) 
           | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4));
    test_steering_equivalence__DOT__current__DOT__hmi_owns_steering 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_steering_hold) 
           | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4));
    vlSelfRef.test_steering_equivalence__DOT__old_l 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering)
               ? (0x0cU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)));
    vlSelfRef.test_steering_equivalence__DOT__old_r 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering)
               ? (0x0dU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)));
    vlSelfRef.test_steering_equivalence__DOT__new_l 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__current__DOT__hmi_owns_steering)
               ? (0x0cU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)));
    vlSelfRef.test_steering_equivalence__DOT__new_r 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__current__DOT__hmi_owns_steering)
               ? (0x0dU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)));
}

void Vtest_steering_equivalence___024root___eval_act(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_act\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x000000000000000dULL & vlSelfRef.__VactTriggered[0U])) {
        Vtest_steering_equivalence___024root___act_comb__TOP__0(vlSelf);
    }
}

extern const VlUnpacked<CData/*1:0*/, 256> Vtest_steering_equivalence__ConstPool__TABLE_hc16a1441_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtest_steering_equivalence__ConstPool__TABLE_h4751f6bb_0;
extern const VlUnpacked<CData/*0:0*/, 256> Vtest_steering_equivalence__ConstPool__TABLE_h81f3961f_0;

void Vtest_steering_equivalence___024root___nba_sequent__TOP__0(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___nba_sequent__TOP__0\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    CData/*7:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    CData/*2:0*/ __Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count;
    __Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count = 0;
    CData/*0:0*/ __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed;
    __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 0;
    // Body
    __Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count 
        = vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count;
    __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed 
        = vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_press_consumed;
    if (vlSelfRef.test_steering_equivalence__DOT__rst_n) {
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__previous_hmi_code 
            = vlSelfRef.test_steering_equivalence__DOT__hmi_code;
        if (vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_code_changed) {
            __Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count = 0U;
        } else if ((7U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count))) {
            __Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count)));
        }
        if (vlSelfRef.test_steering_equivalence__DOT__blocked) {
            vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 0U;
            __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 1U;
        } else if (vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_code_changed) {
            __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 0U;
        } else if (((7U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count)) 
                    & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_press_consumed)))) {
            __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 1U;
            if ((0x0aU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))) {
                vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 1U;
            } else if ((0x0bU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))) {
                vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 2U;
            }
        }
    } else {
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__previous_hmi_code = 0U;
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 0U;
        __Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count = 0U;
        __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 0U;
    }
    __Vtableidx1 = (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering) 
                     << 7U) | (((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                                << 3U) | (((0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
                                           << 2U) | 
                                          (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction) 
                                            << 1U) 
                                           | (IData)(vlSelfRef.test_steering_equivalence__DOT__rst_n)))));
    if ((2U & Vtest_steering_equivalence__ConstPool__TABLE_hc16a1441_0
         [__Vtableidx1])) {
        vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering_hold 
            = Vtest_steering_equivalence__ConstPool__TABLE_h4751f6bb_0
            [__Vtableidx1];
    }
    __Vtableidx2 = (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering) 
                     << 7U) | (((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                                << 3U) | (((0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
                                           << 2U) | 
                                          (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction) 
                                            << 1U) 
                                           | (IData)(vlSelfRef.test_steering_equivalence__DOT__rst_n)))));
    if ((1U & Vtest_steering_equivalence__ConstPool__TABLE_hc16a1441_0
         [__Vtableidx2])) {
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_hold 
            = Vtest_steering_equivalence__ConstPool__TABLE_h81f3961f_0
            [__Vtableidx2];
    }
    if ((2U & Vtest_steering_equivalence__ConstPool__TABLE_hc16a1441_0
         [__Vtableidx2])) {
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_steering_hold 
            = Vtest_steering_equivalence__ConstPool__TABLE_h4751f6bb_0
            [__Vtableidx2];
    }
    vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count 
        = __Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count;
    vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_press_consumed 
        = __Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed;
}

void Vtest_steering_equivalence___024root___nba_comb__TOP__0(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___nba_comb__TOP__0\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (1U 
                                                & ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                                                   | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                                                      >> 2U)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = (1U 
                                                & ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                                                   | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                                                      >> 3U)));
    vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction 
        = ((0x0aU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
           | (0x0bU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)));
    vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering 
        = ((0x0cU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
           | (0x0dU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = ((0x0eU 
                                                 == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
                                                | (IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering));
}

void Vtest_steering_equivalence___024root___nba_comb__TOP__1(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___nba_comb__TOP__1\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering;
    test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering = 0;
    CData/*0:0*/ test_steering_equivalence__DOT__current__DOT__hmi_owns_steering;
    test_steering_equivalence__DOT__current__DOT__hmi_owns_steering = 0;
    // Body
    vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_code_changed 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code) 
           != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__previous_hmi_code));
    vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction 
        = ((1U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched)) 
           | ((2U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched)) 
              | ((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction) 
                 | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_hold) 
                    | (0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))))));
    test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering_hold) 
           | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4));
    test_steering_equivalence__DOT__current__DOT__hmi_owns_steering 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_steering_hold) 
           | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4));
    vlSelfRef.test_steering_equivalence__DOT__new_go 
        = (1U & ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
                 | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction)
                     ? (1U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched))
                     : ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                        | (IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released)))));
    vlSelfRef.test_steering_equivalence__DOT__new_back 
        = (1U & ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
                 | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction)
                     ? (2U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched))
                     : ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                        | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                           >> 1U)))));
    vlSelfRef.test_steering_equivalence__DOT__old_l 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering)
               ? (0x0cU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)));
    vlSelfRef.test_steering_equivalence__DOT__old_r 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering)
               ? (0x0dU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)));
    vlSelfRef.test_steering_equivalence__DOT__new_l 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__current__DOT__hmi_owns_steering)
               ? (0x0cU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)));
    vlSelfRef.test_steering_equivalence__DOT__new_r 
        = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
           | ((IData)(test_steering_equivalence__DOT__current__DOT__hmi_owns_steering)
               ? (0x0dU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
               : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)));
}

void Vtest_steering_equivalence___024root___eval_nba(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_nba\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vinline__nba_sequent__TOP__0___Vtableidx1;
    __Vinline__nba_sequent__TOP__0___Vtableidx1 = 0;
    CData/*7:0*/ __Vinline__nba_sequent__TOP__0___Vtableidx2;
    __Vinline__nba_sequent__TOP__0___Vtableidx2 = 0;
    CData/*2:0*/ __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count;
    __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count = 0;
    CData/*0:0*/ __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed;
    __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 0;
    CData/*0:0*/ __Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering;
    __Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering = 0;
    CData/*0:0*/ __Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__current__DOT__hmi_owns_steering;
    __Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__current__DOT__hmi_owns_steering = 0;
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count 
            = vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count;
        __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed 
            = vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_press_consumed;
        if (vlSelfRef.test_steering_equivalence__DOT__rst_n) {
            vlSelfRef.test_steering_equivalence__DOT__current__DOT__previous_hmi_code 
                = vlSelfRef.test_steering_equivalence__DOT__hmi_code;
            if (vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_code_changed) {
                __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count = 0U;
            } else if ((7U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count))) {
                __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count 
                    = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count)));
            }
            if (vlSelfRef.test_steering_equivalence__DOT__blocked) {
                vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 0U;
                __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 1U;
            } else if (vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_code_changed) {
                __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 0U;
            } else if (((7U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count)) 
                        & (~ (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_press_consumed)))) {
                __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 1U;
                if ((0x0aU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))) {
                    vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 1U;
                } else if ((0x0bU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))) {
                    vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 2U;
                }
            }
        } else {
            vlSelfRef.test_steering_equivalence__DOT__current__DOT__previous_hmi_code = 0U;
            vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = 0U;
            __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count = 0U;
            __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed = 0U;
        }
        __Vinline__nba_sequent__TOP__0___Vtableidx1 
            = (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering) 
                << 7U) | (((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                           << 3U) | (((0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
                                      << 2U) | (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction) 
                                                 << 1U) 
                                                | (IData)(vlSelfRef.test_steering_equivalence__DOT__rst_n)))));
        if ((2U & Vtest_steering_equivalence__ConstPool__TABLE_hc16a1441_0
             [__Vinline__nba_sequent__TOP__0___Vtableidx1])) {
            vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering_hold 
                = Vtest_steering_equivalence__ConstPool__TABLE_h4751f6bb_0
                [__Vinline__nba_sequent__TOP__0___Vtableidx1];
        }
        __Vinline__nba_sequent__TOP__0___Vtableidx2 
            = (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering) 
                << 7U) | (((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                           << 3U) | (((0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
                                      << 2U) | (((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction) 
                                                 << 1U) 
                                                | (IData)(vlSelfRef.test_steering_equivalence__DOT__rst_n)))));
        if ((1U & Vtest_steering_equivalence__ConstPool__TABLE_hc16a1441_0
             [__Vinline__nba_sequent__TOP__0___Vtableidx2])) {
            vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_hold 
                = Vtest_steering_equivalence__ConstPool__TABLE_h81f3961f_0
                [__Vinline__nba_sequent__TOP__0___Vtableidx2];
        }
        if ((2U & Vtest_steering_equivalence__ConstPool__TABLE_hc16a1441_0
             [__Vinline__nba_sequent__TOP__0___Vtableidx2])) {
            vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_steering_hold 
                = Vtest_steering_equivalence__ConstPool__TABLE_h4751f6bb_0
                [__Vinline__nba_sequent__TOP__0___Vtableidx2];
        }
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_stable_count 
            = __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_stable_count;
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__traction_press_consumed 
            = __Vinline__nba_sequent__TOP__0___Vdly__test_steering_equivalence__DOT__current__DOT__traction_press_consumed;
    }
    if ((0x000000000000000dULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            (1U & ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                   | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                      >> 2U)));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = 
            (1U & ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                   | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                      >> 3U)));
        vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction 
            = ((0x0aU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
               | (0x0bU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)));
        vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering 
            = ((0x0cU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
               | (0x0dU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = 
            ((0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code)) 
             | (IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering));
    }
    if ((0x000000000000000fULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_code_changed 
            = ((IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code) 
               != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__previous_hmi_code));
        vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction 
            = ((1U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched)) 
               | ((2U == (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched)) 
                  | ((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_traction) 
                     | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_hold) 
                        | (0x0eU == (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))))));
        __Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering 
            = ((IData)(vlSelfRef.test_steering_equivalence__DOT__previous__DOT__hmi_steering_hold) 
               | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4));
        __Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__current__DOT__hmi_owns_steering 
            = ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_steering_hold) 
               | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4));
        vlSelfRef.test_steering_equivalence__DOT__new_go 
            = (1U & ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
                     | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction)
                         ? (1U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched))
                         : ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                            | (IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released)))));
        vlSelfRef.test_steering_equivalence__DOT__new_back 
            = (1U & ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
                     | ((IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_owns_traction)
                         ? (2U != (IData)(vlSelfRef.test_steering_equivalence__DOT__current__DOT__hmi_traction_latched))
                         : ((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__armed)) 
                            | ((IData)(vlSelfRef.test_steering_equivalence__DOT__fspi_released) 
                               >> 1U)))));
        vlSelfRef.test_steering_equivalence__DOT__old_l 
            = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
               | (__Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering
                   ? (0x0cU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
                   : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)));
        vlSelfRef.test_steering_equivalence__DOT__old_r 
            = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
               | (__Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__previous__DOT__hmi_owns_steering
                   ? (0x0dU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
                   : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)));
        vlSelfRef.test_steering_equivalence__DOT__new_l 
            = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
               | (__Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__current__DOT__hmi_owns_steering
                   ? (0x0cU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
                   : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)));
        vlSelfRef.test_steering_equivalence__DOT__new_r 
            = ((IData)(vlSelfRef.test_steering_equivalence__DOT__blocked) 
               | (__Vinline__nba_comb__TOP__1_test_steering_equivalence__DOT__current__DOT__hmi_owns_steering
                   ? (0x0dU != (IData)(vlSelfRef.test_steering_equivalence__DOT__hmi_code))
                   : (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)));
    }
}

void Vtest_steering_equivalence___024root___timing_ready(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___timing_ready\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((4ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h931b9c32__0.ready("@(negedge test_steering_equivalence.clk)");
    }
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h931b9f78__0.ready("@(posedge test_steering_equivalence.clk)");
    }
}

void Vtest_steering_equivalence___024root___timing_resume(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___timing_resume\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h931b9c32__0.moveToResumeQueue(
                                                          "@(negedge test_steering_equivalence.clk)");
    vlSelfRef.__VtrigSched_h931b9f78__0.moveToResumeQueue(
                                                          "@(posedge test_steering_equivalence.clk)");
    vlSelfRef.__VtrigSched_h931b9c32__0.resume("@(negedge test_steering_equivalence.clk)");
    vlSelfRef.__VtrigSched_h931b9f78__0.resume("@(posedge test_steering_equivalence.clk)");
    if ((8ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_steering_equivalence___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_steering_equivalence___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_steering_equivalence___024root___eval_phase__act(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_phase__act\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_steering_equivalence___024root___eval_triggers_vec__act(vlSelf);
    Vtest_steering_equivalence___024root___timing_ready(vlSelf);
    Vtest_steering_equivalence___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_steering_equivalence___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_steering_equivalence___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_steering_equivalence___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_steering_equivalence___024root___timing_resume(vlSelf);
        Vtest_steering_equivalence___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_steering_equivalence___024root___eval_phase__inact(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_phase__inact\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_steering_equivalence___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_steering_equivalence___024root___eval_phase__nba(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_phase__nba\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_steering_equivalence___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_steering_equivalence___024root___eval_nba(vlSelf);
        Vtest_steering_equivalence___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_steering_equivalence___024root___eval(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_steering_equivalence___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 2, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 2, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_steering_equivalence___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 2, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_steering_equivalence___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_steering_equivalence___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_steering_equivalence___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_steering_equivalence___024root____VbeforeTrig_h931b9c32__0(Vtest_steering_equivalence___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root____VbeforeTrig_h931b9c32__0\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__clk)) 
                                    & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0)) 
                                   << 2U) | ((IData)(vlSelfRef.test_steering_equivalence__DOT__clk) 
                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0 
        = vlSelfRef.test_steering_equivalence__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h931b9f78__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h931b9f78__0.ready(__VeventDescription);
    }
    if ((4ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h931b9c32__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h931b9c32__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h931b9c32__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_steering_equivalence___024root____VbeforeTrig_h931b9f78__0(Vtest_steering_equivalence___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root____VbeforeTrig_h931b9f78__0\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((((~ (IData)(vlSelfRef.test_steering_equivalence__DOT__clk)) 
                                    & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0)) 
                                   << 2U) | ((IData)(vlSelfRef.test_steering_equivalence__DOT__clk) 
                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0 
        = vlSelfRef.test_steering_equivalence__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h931b9f78__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h931b9f78__0.ready(__VeventDescription);
    }
    if ((4ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h931b9c32__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h931b9c32__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h931b9c32__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_steering_equivalence___024root___eval_debug_assertions(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_debug_assertions\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
