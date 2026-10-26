// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_steering_equivalence.h for the primary calling header

#include "Vtest_steering_equivalence__pch.h"

void Vtest_steering_equivalence___024root___timing_ready(Vtest_steering_equivalence___024root* vlSelf);

VL_ATTR_COLD void Vtest_steering_equivalence___024root___eval_static(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_static\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_steering_equivalence__DOT__clk = 0U;
    vlSelfRef.test_steering_equivalence__DOT__rst_n = 0U;
    vlSelfRef.test_steering_equivalence__DOT__hmi_code = 0U;
    vlSelfRef.test_steering_equivalence__DOT__fspi_released = 0x0fU;
    vlSelfRef.test_steering_equivalence__DOT__armed = 1U;
    vlSelfRef.test_steering_equivalence__DOT__blocked = 0U;
    vlSelfRef.test_steering_equivalence__DOT__checks = 0U;
    vlSelfRef.test_steering_equivalence__DOT__rng = 0x8b65e410U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__rst_n__0 = 0U;
    Vtest_steering_equivalence___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_steering_equivalence___024root___eval_static__TOP(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_static__TOP\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_steering_equivalence__DOT__clk = 0U;
    vlSelfRef.test_steering_equivalence__DOT__rst_n = 0U;
    vlSelfRef.test_steering_equivalence__DOT__hmi_code = 0U;
    vlSelfRef.test_steering_equivalence__DOT__fspi_released = 0x0fU;
    vlSelfRef.test_steering_equivalence__DOT__armed = 1U;
    vlSelfRef.test_steering_equivalence__DOT__blocked = 0U;
    vlSelfRef.test_steering_equivalence__DOT__checks = 0U;
    vlSelfRef.test_steering_equivalence__DOT__rng = 0x8b65e410U;
}

VL_ATTR_COLD void Vtest_steering_equivalence___024root___eval_final(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_final\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_steering_equivalence___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_steering_equivalence___024root___eval_phase__stl(Vtest_steering_equivalence___024root* vlSelf);

VL_ATTR_COLD void Vtest_steering_equivalence___024root___eval_settle(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_settle\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_steering_equivalence___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("/mnt/d/Study_Object/image_recognition/image_recognition/tmp/hmi_traction_latch_20261026/test_steering_equivalence.v", 2, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_steering_equivalence___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_steering_equivalence___024root___eval_triggers_vec__stl(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_triggers_vec__stl\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_steering_equivalence___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_steering_equivalence___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_steering_equivalence___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_steering_equivalence___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___trigger_anySet__stl\n"); );
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

void Vtest_steering_equivalence___024root___act_comb__TOP__0(Vtest_steering_equivalence___024root* vlSelf);

VL_ATTR_COLD void Vtest_steering_equivalence___024root___eval_stl(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_stl\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_steering_equivalence___024root___act_comb__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_steering_equivalence___024root___eval_phase__stl(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___eval_phase__stl\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_steering_equivalence___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_steering_equivalence___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_steering_equivalence___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_steering_equivalence___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_steering_equivalence___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_steering_equivalence___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_steering_equivalence___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_steering_equivalence.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_steering_equivalence.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(negedge test_steering_equivalence.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_steering_equivalence___024root___ctor_var_reset(Vtest_steering_equivalence___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_steering_equivalence___024root___ctor_var_reset\n"); );
    Vtest_steering_equivalence__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_steering_equivalence__DOT__old_l = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9381945897675756669ull);
    vlSelf->test_steering_equivalence__DOT__old_r = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3196316668076688552ull);
    vlSelf->test_steering_equivalence__DOT__new_l = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9768720605234189520ull);
    vlSelf->test_steering_equivalence__DOT__new_r = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10905090388650672107ull);
    vlSelf->test_steering_equivalence__DOT__new_go = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8866851311322837412ull);
    vlSelf->test_steering_equivalence__DOT__new_back = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17745062306248393043ull);
    vlSelf->test_steering_equivalence__DOT__hold_cycles = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 4880942229724392285ull);
    vlSelf->test_steering_equivalence__DOT__previous__DOT__hmi_traction = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1402010799249303570ull);
    vlSelf->test_steering_equivalence__DOT__previous__DOT__hmi_steering = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10699953502944939152ull);
    vlSelf->test_steering_equivalence__DOT__previous__DOT__hmi_steering_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2781087422986816960ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__hmi_traction_latched = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 17606843981415890758ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__previous_hmi_code = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 1236512224550543789ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__traction_stable_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 12112336503147691376ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__traction_press_consumed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 193554831057053584ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__hmi_code_changed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7099208448298029419ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__hmi_traction_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1331072930386789069ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__hmi_steering_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8473931651380249683ull);
    vlSelf->test_steering_equivalence__DOT__current__DOT__hmi_owns_traction = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 662740522910601426ull);
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_3 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_4 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_steering_equivalence__DOT__rst_n__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
