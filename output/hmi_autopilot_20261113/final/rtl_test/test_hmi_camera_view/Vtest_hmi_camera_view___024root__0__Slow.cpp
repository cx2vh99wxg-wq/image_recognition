// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_camera_view.h for the primary calling header

#include "Vtest_hmi_camera_view__pch.h"

void Vtest_hmi_camera_view___024root___timing_ready(Vtest_hmi_camera_view___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___eval_static(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_static\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vinline__eval_static__TOP_test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c;
    __Vinline__eval_static__TOP_test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c = 0;
    // Body
    vlSelfRef.test_hmi_camera_view__DOT__clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    for (int __Vi0 = 0; __Vi0 < 15; ++__Vi0) {
        vlSelf->test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11798325935399624054ull);
    }
    vlSelf->test_hmi_camera_view__DOT__expect_spi__Vstatic__hi = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 662887750319877932ull);
    vlSelf->test_hmi_camera_view__DOT__expect_spi__Vstatic__lo = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12459660101445587627ull);
    vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg = 0U;
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 768231169478197879ull);
    __Vinline__eval_static__TOP_test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c = 0;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__rst_n__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_clk__0 = 1U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_cs__0 = 1U;
    Vtest_hmi_camera_view___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___eval_static__TOP(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_static__TOP\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c;
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c = 0;
    // Body
    vlSelfRef.test_hmi_camera_view__DOT__clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    for (int __Vi0 = 0; __Vi0 < 15; ++__Vi0) {
        vlSelf->test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11798325935399624054ull);
    }
    vlSelf->test_hmi_camera_view__DOT__expect_spi__Vstatic__hi = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 662887750319877932ull);
    vlSelf->test_hmi_camera_view__DOT__expect_spi__Vstatic__lo = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12459660101445587627ull);
    vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg = 0U;
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 768231169478197879ull);
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c = 0;
}

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___eval_final(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_final\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_camera_view___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_hmi_camera_view___024root___eval_phase__stl(Vtest_hmi_camera_view___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___eval_settle(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_settle\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_camera_view___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 3, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_hmi_camera_view___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___eval_triggers_vec__stl(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_triggers_vec__stl\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_hmi_camera_view___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_camera_view___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_camera_view___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_hmi_camera_view___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___stl_sequent__TOP__0(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___stl_sequent__TOP__0\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT____VlemCall_2__crc_byte;
    CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT____VlemCall_1__crc_byte;
    CData/*7:0*/ test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c;
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c = 0;
    CData/*7:0*/ __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__95__value;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__95__value = 0;
    CData/*7:0*/ __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__Vfuncout;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__Vfuncout = 0;
    CData/*7:0*/ __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__c0;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__c0 = 0;
    CData/*7:0*/ __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__value;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__value = 0;
    CData/*7:0*/ __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__97__value;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__97__value = 0;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_4;
    __VdfgRegularize_h6e95ff9d_0_4 = 0;
    // Body
    vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_complete 
        = ((((IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid) 
             & (3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen))) 
            & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history))) 
           & (0x0007a120U > vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count));
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_gear 
        = ((4U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync))
            ? ((2U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync))
                ? (2U & (- (IData)((1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync))))))
                : (3U & (- (IData)((1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync)))))))
            : ((2U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync))
                ? ((1U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync))
                    ? 3U : 1U) : ((1U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync))
                                   ? 2U : 1U)));
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_status 
        = ((0x000000b0U | (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync)) 
           & (- (IData)(((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync) 
                         == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_previous)))));
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__95__value 
        = (0x000000ffU & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_sync));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x2bU ^ (IData)(__Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__95__value));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT____VlemCall_1__crc_byte 
        = test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__value 
        = (0x000000ffU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_sync) 
                          >> 8U));
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__c0 
        = test_hmi_camera_view__DOT__transport__DOT____VlemCall_1__crc_byte;
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = ((IData)(__Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__c0) 
           ^ (IData)(__Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__value));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__Vfuncout 
        = test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_control_crc 
        = __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__96__Vfuncout;
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
                                                & (2U 
                                                   == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg)));
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_speed 
        = (0x00000080U | ((0xffffffc0U & (((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync) 
                                           << 1U) & 
                                          ((0U != (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_gear)) 
                                           << 6U))) 
                          | ((0x00000030U & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync) 
                                             << 1U)) 
                             | (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_gear))));
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__97__value 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_status;
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = __Vfunc_test_hmi_camera_view__DOT__transport__DOT__crc_byte__97__value;
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c 
        = (0x000000ffU & ((0x00000080U & (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c))
                           ? (7U ^ VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U))
                           : VL_SHIFTL_III(8,8,32, (IData)(test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c), 1U)));
    test_hmi_camera_view__DOT__transport__DOT____VlemCall_2__crc_byte 
        = test_hmi_camera_view__DOT__transport__DOT__crc_byte__Vstatic__c;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_packet 
        = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_status) 
            << 8U) | (IData)(test_hmi_camera_view__DOT__transport__DOT____VlemCall_2__crc_byte));
    __VdfgRegularize_h6e95ff9d_0_4 = ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_active) 
                                      & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2));
    vlSelfRef.test_hmi_camera_view__DOT__bus = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                   ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble)
                                                   : 
                                                  ((IData)(__VdfgRegularize_h6e95ff9d_0_4)
                                                    ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble)
                                                    : 
                                                   ((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                                                    & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)))))) 
                                                 & ((- (IData)((IData)(__VdfgRegularize_h6e95ff9d_0_4))) 
                                                    | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))) 
                                                       | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)))))) 
                                                | ((IData)(vlSelfRef.test_hmi_camera_view__DOT__host_data) 
                                                   & (- (IData)((IData)(vlSelfRef.test_hmi_camera_view__DOT__host_drives)))));
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header 
        = ((0x00fffff0U & (vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_header 
                           << 4U)) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus));
}

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___eval_stl(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_stl\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        Vtest_hmi_camera_view___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD bool Vtest_hmi_camera_view___024root___eval_phase__stl(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_phase__stl\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_hmi_camera_view___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_camera_view___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_hmi_camera_view___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_hmi_camera_view___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_hmi_camera_view___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_camera_view___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_camera_view___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_hmi_camera_view.clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_hmi_camera_view.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(posedge test_hmi_camera_view.spi_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(negedge test_hmi_camera_view.spi_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @(posedge test_hmi_camera_view.spi_cs)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @(negedge test_hmi_camera_view.clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_hmi_camera_view___024root___ctor_var_reset(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___ctor_var_reset\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_camera_view__DOT__view_code = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8966669261899027916ull);
    vlSelf->test_hmi_camera_view__DOT__bus = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2829805732481128039ull);
    vlSelf->test_hmi_camera_view__DOT__decoder__DOT__frame_history = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 14531272481354935133ull);
    vlSelf->test_hmi_camera_view__DOT__decoder__DOT__bytes_seen = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4021129610054759803ull);
    vlSelf->test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 1736865820436563022ull);
    vlSelf->test_hmi_camera_view__DOT__decoder__DOT__frame_complete = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12695154013832553672ull);
    vlSelf->test_hmi_camera_view__DOT__preview__DOT__previous_code = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3738861812049433541ull);
    vlSelf->test_hmi_camera_view__DOT__preview__DOT__cycles_left = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2910570775103096541ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__recv_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12204542050508285790ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__recv_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9004036107642253677ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__cmd_reg = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4171508470419481871ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__cmd_lock = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12740845509889025032ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__auto_enable_meta = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2167655439748978486ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__auto_enable_sync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7591439163182728395ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__status_header = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 3057304016089303513ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__status_nibble_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 10295658458826001979ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__status_read_ready = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5014128762812121459ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__status_read_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18348425562343587451ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__status_tx_started = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2963148345697703589ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3765551866996625470ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__next_status_header = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 8626177106076979477ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__speed_meta = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 10431468050105894194ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__speed_sync = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 12284109723234293084ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__extended_index = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12122432637627755417ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__extended_nibble = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 3610700765653187590ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 12257003222740256287ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__speed_snapshot = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11461055331028365889ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__speed_crc = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6181164829179810653ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__control_meta = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 14155387443384654560ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__control_sync = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1116577912865925343ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__control_snapshot = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11381031815818427522ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__control_crc = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7686511455954228476ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__camera_view_meta = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10898133779826299452ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__camera_view_sync = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2847136033347826988ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__camera_view_previous = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2977258532468714109ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8386127565968738975ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__wheel_meta = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 13623829131798204305ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__wheel_sync = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 7100677856267486669ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__wheel_previous = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14799854111054568992ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14492294568693653744ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__autopilot_meta = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2033392751306126120ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__autopilot_sync = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 7855724998652396386ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__autopilot_previous = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 2654457913944368273ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3447275451586406768ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__extended_active = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 771572782331017499ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__sampled_gear = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 12667446678476248815ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__sampled_speed = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18148457859689432213ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__sampled_control_crc = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16743641813372363923ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__autopilot_status = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18312686183850251312ull);
    vlSelf->test_hmi_camera_view__DOT__transport__DOT__autopilot_packet = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 1465083677378195142ull);
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_2 = 0;
    vlSelf->__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history = 0;
    vlSelf->__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0;
    vlSelf->__Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag = 0;
    vlSelf->__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count = 0;
    vlSelf->__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__rst_n__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_cs__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
