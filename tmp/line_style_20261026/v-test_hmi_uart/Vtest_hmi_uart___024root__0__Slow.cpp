// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_uart.h for the primary calling header

#include "Vtest_hmi_uart__pch.h"

void Vtest_hmi_uart___024root___timing_ready(Vtest_hmi_uart___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_uart___024root___eval_static(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___eval_static\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_uart__DOT__clk = 0U;
    vlSelfRef.test_hmi_uart__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_uart__DOT__uart_rx = 0U;
    vlSelfRef.test_hmi_uart__DOT__uart_rx_valid = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_default_hmi__DOT__cnn_level_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__mode_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__blc_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__awb_reg = 0x7fU;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__liangdu_reg = 0x40U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__baohedu_reg = 0x50U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__awb_en_reg = 1U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__binarization_reg = 0x7fU;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__cnn_level_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__sobel_reg = 0x28U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_uart__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_uart__DOT__rst_n__0 = 0U;
    Vtest_hmi_uart___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_hmi_uart___024root___eval_static__TOP(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___eval_static__TOP\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_uart__DOT__clk = 0U;
    vlSelfRef.test_hmi_uart__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_uart__DOT__uart_rx = 0U;
    vlSelfRef.test_hmi_uart__DOT__uart_rx_valid = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_default_hmi__DOT__cnn_level_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__mode_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__blc_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__awb_reg = 0x7fU;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__liangdu_reg = 0x40U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__baohedu_reg = 0x50U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__awb_en_reg = 1U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__binarization_reg = 0x7fU;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__cnn_level_reg = 0U;
    vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__sobel_reg = 0x28U;
}

VL_ATTR_COLD void Vtest_hmi_uart___024root___eval_final(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___eval_final\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_uart___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_hmi_uart___024root___eval_phase__stl(Vtest_hmi_uart___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_uart___024root___eval_settle(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___eval_settle\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_uart___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_uart.v", 2, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_hmi_uart___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_hmi_uart___024root___eval_triggers_vec__stl(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___eval_triggers_vec__stl\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_hmi_uart___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_uart___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_uart___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_hmi_uart___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_hmi_uart___024root___eval_stl(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___eval_stl\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        vlSelfRef.test_hmi_uart__DOT__u_default_hmi__DOT__frame_complete 
            = ((IData)(vlSelfRef.test_hmi_uart__DOT__uart_rx_valid) 
               & ((3U == (IData)(vlSelfRef.test_hmi_uart__DOT__u_default_hmi__DOT__bytes_seen)) 
                  & ((0x0007a120U > vlSelfRef.test_hmi_uart__DOT__u_default_hmi__DOT__frame_gap_count) 
                     & ((0x30U == (IData)(vlSelfRef.test_hmi_uart__DOT__u_default_hmi__DOT__previous_3)) 
                        & (0x90U == (IData)(vlSelfRef.test_hmi_uart__DOT__u_default_hmi__DOT__previous_2))))));
        vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__frame_complete 
            = ((IData)(vlSelfRef.test_hmi_uart__DOT__uart_rx_valid) 
               & ((3U == (IData)(vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__bytes_seen)) 
                  & ((0x00000018U > vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__frame_gap_count) 
                     & ((0x30U == (IData)(vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__previous_3)) 
                        & (0x90U == (IData)(vlSelfRef.test_hmi_uart__DOT__u_hmi__DOT__previous_2))))));
    }
}

VL_ATTR_COLD bool Vtest_hmi_uart___024root___eval_phase__stl(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___eval_phase__stl\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_hmi_uart___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_uart___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_hmi_uart___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_hmi_uart___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_hmi_uart___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_uart___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_uart___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_hmi_uart.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_hmi_uart.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(negedge test_hmi_uart.clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_hmi_uart___024root___ctor_var_reset(Vtest_hmi_uart___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_uart___024root___ctor_var_reset\n"); );
    Vtest_hmi_uart__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_uart__DOT__u_default_hmi__DOT__previous_3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15159224012412467543ull);
    vlSelf->test_hmi_uart__DOT__u_default_hmi__DOT__previous_2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17009762135995733379ull);
    vlSelf->test_hmi_uart__DOT__u_default_hmi__DOT__previous_1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8699971123546051200ull);
    vlSelf->test_hmi_uart__DOT__u_default_hmi__DOT__bytes_seen = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 9439070611730862912ull);
    vlSelf->test_hmi_uart__DOT__u_default_hmi__DOT__frame_gap_count = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 9484337686856942779ull);
    vlSelf->test_hmi_uart__DOT__u_default_hmi__DOT__frame_complete = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8240025834150512345ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__cb_min_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2349328649924993174ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__cb_max_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9327859890326364158ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__cr_min_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10105787226520386337ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__cr_max_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11819956973909440003ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__isp_judge_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2089415971646684928ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__previous_3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13658735229494329845ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__previous_2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18181329334504020549ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__previous_1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1671244044651158254ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__bytes_seen = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 11402245602112884713ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__frame_gap_count = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 13516163948208466533ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__frame_complete = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16440301264010932481ull);
    vlSelf->test_hmi_uart__DOT__u_hmi__DOT__traction_lease_count = VL_SCOPED_RAND_RESET_I(27, __VscopeHash, 4552107949475097270ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_uart__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_uart__DOT__rst_n__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
