// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_direction_arbiter.h for the primary calling header

#include "Vtest_direction_arbiter__pch.h"

void Vtest_direction_arbiter___024root___timing_ready(Vtest_direction_arbiter___024root* vlSelf);

VL_ATTR_COLD void Vtest_direction_arbiter___024root___eval_static(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___eval_static\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_direction_arbiter__DOT__clk = 0U;
    vlSelfRef.test_direction_arbiter__DOT__rst_n = 0U;
    vlSelfRef.test_direction_arbiter__DOT__hmi_code = 0U;
    vlSelfRef.test_direction_arbiter__DOT__fspi_released = 0x0fU;
    vlSelfRef.test_direction_arbiter__DOT__fspi_direction_armed = 1U;
    vlSelfRef.test_direction_arbiter__DOT__movement_blocked = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_direction_arbiter__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_direction_arbiter__DOT__rst_n__0 = 0U;
    Vtest_direction_arbiter___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_direction_arbiter___024root___eval_static__TOP(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___eval_static__TOP\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_direction_arbiter__DOT__clk = 0U;
    vlSelfRef.test_direction_arbiter__DOT__rst_n = 0U;
    vlSelfRef.test_direction_arbiter__DOT__hmi_code = 0U;
    vlSelfRef.test_direction_arbiter__DOT__fspi_released = 0x0fU;
    vlSelfRef.test_direction_arbiter__DOT__fspi_direction_armed = 1U;
    vlSelfRef.test_direction_arbiter__DOT__movement_blocked = 0U;
}

VL_ATTR_COLD void Vtest_direction_arbiter___024root___eval_final(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___eval_final\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_direction_arbiter___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_direction_arbiter___024root___eval_phase__stl(Vtest_direction_arbiter___024root* vlSelf);

VL_ATTR_COLD void Vtest_direction_arbiter___024root___eval_settle(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___eval_settle\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_direction_arbiter___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_direction_arbiter.v", 3, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_direction_arbiter___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_direction_arbiter___024root___eval_triggers_vec__stl(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___eval_triggers_vec__stl\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_direction_arbiter___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_direction_arbiter___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_direction_arbiter___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_direction_arbiter___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___trigger_anySet__stl\n"); );
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

void Vtest_direction_arbiter___024root___act_comb__TOP__0(Vtest_direction_arbiter___024root* vlSelf);

VL_ATTR_COLD void Vtest_direction_arbiter___024root___eval_stl(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___eval_stl\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_direction_arbiter___024root___act_comb__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_direction_arbiter___024root___eval_phase__stl(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___eval_phase__stl\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_direction_arbiter___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_direction_arbiter___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_direction_arbiter___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_direction_arbiter___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_direction_arbiter___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_direction_arbiter___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_direction_arbiter___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_direction_arbiter.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_direction_arbiter.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(negedge test_direction_arbiter.clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_direction_arbiter___024root___ctor_var_reset(Vtest_direction_arbiter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_direction_arbiter___024root___ctor_var_reset\n"); );
    Vtest_direction_arbiter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_direction_arbiter__DOT__released = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 6614580877682097346ull);
    vlSelf->test_direction_arbiter__DOT__dut__DOT__hmi_traction = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8915321058624038053ull);
    vlSelf->test_direction_arbiter__DOT__dut__DOT__hmi_steering = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8266740601272760732ull);
    vlSelf->test_direction_arbiter__DOT__dut__DOT__hmi_traction_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16444077579905181007ull);
    vlSelf->test_direction_arbiter__DOT__dut__DOT__hmi_steering_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5215287743684764602ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_direction_arbiter__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_direction_arbiter__DOT__rst_n__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
