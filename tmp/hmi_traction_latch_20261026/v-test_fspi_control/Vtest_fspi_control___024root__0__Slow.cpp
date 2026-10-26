// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_fspi_control.h for the primary calling header

#include "Vtest_fspi_control__pch.h"

VL_ATTR_COLD void Vtest_fspi_control___024root___eval_static(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_static\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_fspi_control__DOT__wr_clk = 0U;
    vlSelfRef.test_fspi_control__DOT__rst_n = 0U;
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    vlSelfRef.test_fspi_control__DOT__master_data = 0x0fU;
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__rst_n__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_clk__0 = 1U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_cs__0 = 1U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__wr_clk__0 = 0U;
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_fspi_control___024root___eval_static__TOP(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_static__TOP\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_fspi_control__DOT__wr_clk = 0U;
    vlSelfRef.test_fspi_control__DOT__rst_n = 0U;
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    vlSelfRef.test_fspi_control__DOT__master_data = 0x0fU;
}

VL_ATTR_COLD void Vtest_fspi_control___024root___eval_final(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_final\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_fspi_control___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_fspi_control___024root___eval_phase__stl(Vtest_fspi_control___024root* vlSelf);

VL_ATTR_COLD void Vtest_fspi_control___024root___eval_settle(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_settle\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_fspi_control___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 2, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_fspi_control___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_fspi_control___024root___eval_triggers_vec__stl(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_triggers_vec__stl\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_fspi_control___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_fspi_control___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_fspi_control___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_fspi_control___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_fspi_control___024root___stl_sequent__TOP__0(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___stl_sequent__TOP__0\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose 
        = ((~ (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d1)) 
           & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d0));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = ((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = (0x0000000fU 
                                                & (- (IData)(
                                                             ((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                                                              & (2U 
                                                                 == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg))))));
    vlSelfRef.test_fspi_control__DOT__spi_data = ((
                                                   ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                                                     ? (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble)
                                                     : 
                                                    (0x0aU 
                                                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))) 
                                                   & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))) 
                                                      | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))) 
                                                  | (IData)(vlSelfRef.test_fspi_control__DOT__master_data));
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header 
        = ((0x00fffff0U & (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_header 
                           << 4U)) | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data));
}

VL_ATTR_COLD void Vtest_fspi_control___024root___eval_stl(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_stl\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose 
            = ((~ (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d1)) 
               & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d0));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            ((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = 
            (0x0000000fU & (- (IData)(((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                                       & (2U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg))))));
        vlSelfRef.test_fspi_control__DOT__spi_data 
            = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                  ? (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble)
                  : (0x0aU & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))) 
                & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))) 
                   | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2))) 
               | (IData)(vlSelfRef.test_fspi_control__DOT__master_data));
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data));
    }
}

VL_ATTR_COLD bool Vtest_fspi_control___024root___eval_phase__stl(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_phase__stl\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_fspi_control___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_fspi_control___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_fspi_control___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_fspi_control___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_fspi_control___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_fspi_control___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_fspi_control___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(negedge test_fspi_control.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(posedge test_fspi_control.spi_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(posedge test_fspi_control.spi_cs)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(negedge test_fspi_control.spi_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @(negedge test_fspi_control.wr_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_fspi_control___024root___ctor_var_reset(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___ctor_var_reset\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_fspi_control__DOT__spi_data = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 18141321876248587200ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__recv_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11190211847577169403ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__recv_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 2568849059675016967ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__recv_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18271859832135086710ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__cmd_reg = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14277050163293723566ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__cmd_lock = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2362108482544953372ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__tx_valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4587064398534413055ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__auto_enable_meta = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6315211342399421456ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__auto_enable_sync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5560519202909334451ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__status_header = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 17614514723167972756ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__status_nibble_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6461263359373599544ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__status_read_ready = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10234019674499363453ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__status_read_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14858362157803411710ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__status_tx_started = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5744181360067115596ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__status_tx_nibble = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13648747035789187056ull);
    vlSelf->test_fspi_control__DOT__u_slave__DOT__next_status_header = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 7466630617164384599ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT____Vlvbound_h11419250__0 = 0;
    vlSelf->test_fspi_control__DOT__u_regs__DOT__rx_pose = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14551414061113964726ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6102277406688599779ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__cmd_reg = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5692766297533640337ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__spi_cs_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10492937304954337823ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__rx_valid_d0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15088719756983859523ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__rx_valid_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14711452014138316707ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__tx_done_d0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13035083607607466855ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__tx_done_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17162470639945622707ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__wr_data_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8330697426262902977ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__addr_high = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14228398547506262295ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__ram_wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9972495665272553744ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__ram_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12488136724368804919ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__byte_cnt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14992092515527493635ull);
    vlSelf->test_fspi_control__DOT__u_regs__DOT__read_addr_high = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14383031417839534822ull);
    for (int __Vi0 = 0; __Vi0 < 5; ++__Vi0) {
        vlSelf->test_fspi_control__DOT__u_regs__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12410185770941226216ull);
    }
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_2 = 0;
    vlSelf->__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag = 0;
    vlSelf->__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count = 0;
    vlSelf->__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock = 0;
    vlSelf->__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_fspi_control__DOT__rst_n__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_cs__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_fspi_control__DOT__wr_clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
