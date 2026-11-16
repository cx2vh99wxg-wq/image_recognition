// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_gear_feedback.h for the primary calling header

#include "Vtest_hmi_gear_feedback__pch.h"

void Vtest_hmi_gear_feedback___024root___timing_ready(Vtest_hmi_gear_feedback___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_gear_feedback___024root___eval_static(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_static\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_gear_feedback__DOT__clk = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__slider_present = 1U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_active = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__count = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    VL_SCOPED_RAND_RESET_W(192, vlSelf->test_hmi_gear_feedback__DOT__frame__Vstatic__expected, __VscopeHash, 11965959520148425685ull);
    VL_SCOPED_RAND_RESET_W(216, vlSelf->test_hmi_gear_feedback__DOT__frame__Vstatic__number, __VscopeHash, 18219655617106164169ull);
    VL_SCOPED_RAND_RESET_W(80, vlSelf->test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected, __VscopeHash, 3677631570950449763ull);
    VL_SCOPED_RAND_RESET_W(192, vlSelf->test_hmi_gear_feedback__DOT__text_only__Vstatic__expected, __VscopeHash, 16319182959061128695ull);
    vlSelfRef.__VactTriggered[0U] = (8ULL | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000010ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000020ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000040ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000080ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000100ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000200ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000400ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__VactTriggered[0U] = (0x0000000000000800ULL 
                                     | vlSelfRef.__VactTriggered[0U]);
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__rst_n__0 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcbfad22d__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcbfae7c2__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcc0e817f__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcbfcd82a__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcc309423__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcc347338__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcc287291__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcc2c9499__1 = 0U;
    vlSelfRef.__Vtrigprevexpr_hcc20226a__1 = 0U;
    Vtest_hmi_gear_feedback___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_hmi_gear_feedback___024root___eval_static__TOP(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_static__TOP\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_gear_feedback__DOT__clk = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__status = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__slider_present = 1U;
    vlSelfRef.test_hmi_gear_feedback__DOT__autopilot_active = 0U;
    vlSelfRef.test_hmi_gear_feedback__DOT__count = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    VL_SCOPED_RAND_RESET_W(192, vlSelf->test_hmi_gear_feedback__DOT__frame__Vstatic__expected, __VscopeHash, 11965959520148425685ull);
    VL_SCOPED_RAND_RESET_W(216, vlSelf->test_hmi_gear_feedback__DOT__frame__Vstatic__number, __VscopeHash, 18219655617106164169ull);
    VL_SCOPED_RAND_RESET_W(80, vlSelf->test_hmi_gear_feedback__DOT__autopilot_frame__Vstatic__expected, __VscopeHash, 3677631570950449763ull);
    VL_SCOPED_RAND_RESET_W(192, vlSelf->test_hmi_gear_feedback__DOT__text_only__Vstatic__expected, __VscopeHash, 16319182959061128695ull);
}

VL_ATTR_COLD void Vtest_hmi_gear_feedback___024root___eval_final(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_final\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vtest_hmi_gear_feedback___024root___eval_settle(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___eval_settle\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

bool Vtest_hmi_gear_feedback___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_gear_feedback___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_gear_feedback___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_hmi_gear_feedback.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_hmi_gear_feedback.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @( (32'sh18 == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @( (32'sh3d == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @( (32'sh7a == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @( (32'shb7 == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @( (32'shf4 == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 8U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 8 is active: @( (32'sh131 == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 9U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 9 is active: @( (32'sh16e == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000aU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 10 is active: @( (32'sh1ab == test_hmi_gear_feedback.count))\n");
    }
    if ((1U & (IData)((triggers[0U] >> 0x0000000bU)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 11 is active: @( (32'sh1ef == test_hmi_gear_feedback.count))\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_hmi_gear_feedback___024root___ctor_var_reset(Vtest_hmi_gear_feedback___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_gear_feedback___024root___ctor_var_reset\n"); );
    Vtest_hmi_gear_feedback__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_gear_feedback__DOT__txd = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 906071771591741063ull);
    vlSelf->test_hmi_gear_feedback__DOT__valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6054048093845693133ull);
    vlSelf->test_hmi_gear_feedback__DOT__data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3270813036688748474ull);
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->test_hmi_gear_feedback__DOT__captured[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18324533423264432810ull);
    }
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__gear_meta = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10250702495778109457ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__gear_sync = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3817245963799326914ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__refresh_count = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 16718707453464071982ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__byte_gap = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 4055652808891179157ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__state = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4551039865136953535ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__index = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 8423025866187266614ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__gear_character = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 15791109894493923589ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__tx_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6757323962682518995ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__tx_enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7166407328004019290ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__slider_snapshot = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10117819230243583485ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__autopilot_meta = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17895183806235912993ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__autopilot_sync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6387734069138558162ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__autopilot_snapshot = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2646749172353109523ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__tx_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5634121603748420529ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_data_t = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 4944235541995040761ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__tx_cnt = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 9965510262091841147ull);
    vlSelf->test_hmi_gear_feedback__DOT__dut__DOT__tx__DOT__baud_cnt = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6273974557473001289ull);
    vlSelf->test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9405333728460800085ull);
    vlSelf->test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5173730362989058306ull);
    vlSelf->test_hmi_gear_feedback__DOT__receiver__DOT__uart_rxd_d2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6413653190170372116ull);
    vlSelf->test_hmi_gear_feedback__DOT__receiver__DOT__rx_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6324190496560438685ull);
    vlSelf->test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12452807165197840365ull);
    vlSelf->test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14662696401990311061ull);
    vlSelf->test_hmi_gear_feedback__DOT__receiver__DOT__rx_data_t = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2114310834543811036ull);
    vlSelf->__Vdly__test_hmi_gear_feedback__DOT__data = 0;
    vlSelf->__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__baud_cnt = 0;
    vlSelf->__Vdly__test_hmi_gear_feedback__DOT__receiver__DOT__rx_cnt = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_gear_feedback__DOT__rst_n__0 = 0;
    vlSelf->__Vtrigprevexpr_hcbfad22d__1 = 0;
    vlSelf->__Vtrigprevexpr_hcbfae7c2__1 = 0;
    vlSelf->__Vtrigprevexpr_hcc0e817f__1 = 0;
    vlSelf->__Vtrigprevexpr_hcbfcd82a__1 = 0;
    vlSelf->__Vtrigprevexpr_hcc309423__1 = 0;
    vlSelf->__Vtrigprevexpr_hcc347338__1 = 0;
    vlSelf->__Vtrigprevexpr_hcc287291__1 = 0;
    vlSelf->__Vtrigprevexpr_hcc2c9499__1 = 0;
    vlSelf->__Vtrigprevexpr_hcc20226a__1 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
