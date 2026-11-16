// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_autopilot.h for the primary calling header

#include "Vtest_hmi_autopilot__pch.h"

void Vtest_hmi_autopilot___024root___timing_ready(Vtest_hmi_autopilot___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_autopilot___024root___eval_static(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_static\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_autopilot__DOT__clk = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__reset_n = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__data = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__released = 1U;
    vlSelfRef.test_hmi_autopilot__DOT__blocked = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__reset_n__0 = 0U;
    Vtest_hmi_autopilot___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_hmi_autopilot___024root___eval_static__TOP(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_static__TOP\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_autopilot__DOT__clk = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__reset_n = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__data = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__heartbeat = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__valid = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__released = 1U;
    vlSelfRef.test_hmi_autopilot__DOT__blocked = 0U;
    vlSelfRef.test_hmi_autopilot__DOT__uart_decoder__DOT__auto_brake_enable_reg = 0U;
}

VL_ATTR_COLD void Vtest_hmi_autopilot___024root___eval_final(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_final\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_autopilot___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_hmi_autopilot___024root___eval_phase__stl(Vtest_hmi_autopilot___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_autopilot___024root___eval_settle(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_settle\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_autopilot___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_autopilot.v", 2, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_hmi_autopilot___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_hmi_autopilot___024root___eval_triggers_vec__stl(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_triggers_vec__stl\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_hmi_autopilot___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_autopilot___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_autopilot___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_hmi_autopilot___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_hmi_autopilot___024root___eval_stl(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_stl\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
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

VL_ATTR_COLD bool Vtest_hmi_autopilot___024root___eval_phase__stl(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___eval_phase__stl\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_hmi_autopilot___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_autopilot___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_hmi_autopilot___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_hmi_autopilot___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_hmi_autopilot___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_autopilot___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_autopilot___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_hmi_autopilot.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_hmi_autopilot.reset_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(negedge test_hmi_autopilot.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_hmi_autopilot___024root___ctor_var_reset(Vtest_hmi_autopilot___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_autopilot___024root___ctor_var_reset\n"); );
    Vtest_hmi_autopilot__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_autopilot__DOT__requested = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4240401839707494231ull);
    vlSelf->test_hmi_autopilot__DOT__sequence_code = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 7271589588528267663ull);
    vlSelf->test_hmi_autopilot__DOT__uart_decoder__DOT__frame_history = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 1916176823046744987ull);
    vlSelf->test_hmi_autopilot__DOT__uart_decoder__DOT__bytes_seen = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 17799206016803172837ull);
    vlSelf->test_hmi_autopilot__DOT__uart_decoder__DOT__frame_gap_count = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 3333011081581305489ull);
    vlSelf->test_hmi_autopilot__DOT__uart_decoder__DOT__frame_complete = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13336418540949013611ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__request_meta = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 11950912481735611973ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__request_sync = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 8543730425799540196ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__heartbeat_meta = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13201232735162176164ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__heartbeat_sync = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7275408629048941174ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__previous = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6362893246982531258ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__age = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 798807284599944099ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__accepted = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1576503817200617360ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__consumed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6761807847100027487ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__last_request = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 7995184800486887875ull);
    vlSelf->test_hmi_autopilot__DOT__gate__DOT__approval_matches = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2737708045106153339ull);
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_autopilot__DOT__reset_n__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
