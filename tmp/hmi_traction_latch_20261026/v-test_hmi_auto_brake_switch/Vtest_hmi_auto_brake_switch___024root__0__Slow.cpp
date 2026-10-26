// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_auto_brake_switch.h for the primary calling header

#include "Vtest_hmi_auto_brake_switch__pch.h"

void Vtest_hmi_auto_brake_switch___024root___timing_ready(Vtest_hmi_auto_brake_switch___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___eval_static(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_static\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__raw_brake_request = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__use_spi_brake_request = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data = 0x0fU;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__register_clk = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10357544498265783680ull);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__rst_n__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_clk__0 = 1U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_cs__0 = 1U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__register_clk__0 = 0U;
    Vtest_hmi_auto_brake_switch___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___eval_static__TOP(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_static__TOP\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__raw_brake_request = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__use_spi_brake_request = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data = 0x0fU;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__register_clk = 0U;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10357544498265783680ull);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg = 0U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 0U;
}

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___eval_final(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_final\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_hmi_auto_brake_switch___024root___eval_phase__stl(Vtest_hmi_auto_brake_switch___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___eval_settle(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_settle\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_auto_brake_switch___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 5, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_hmi_auto_brake_switch___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___eval_triggers_vec__stl(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_triggers_vec__stl\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_hmi_auto_brake_switch___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_auto_brake_switch___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_hmi_auto_brake_switch___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___stl_sequent__TOP__0(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___stl_sequent__TOP__0\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_code_changed 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync) 
           != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__previous_hmi_code));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose 
        = ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d1)) 
           & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d0));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__gated_request 
        = (IData)((((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                    >> 1U) & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__use_spi_brake_request)
                               ? (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_sync) 
                                     >> 4U)) : (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__raw_brake_request))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) 
           & ((3U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen)) 
              & ((0x00000020U > vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count) 
                 & ((0x30U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3)) 
                    & (0x90U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2))))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering 
        = ((0x0cU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
           | (0x0dU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction 
        = ((0x0aU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
           | (0x0bU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__rearm_command 
        = ((0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
           & (7U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count)));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = (0x0000000fU 
                                                & (- (IData)(
                                                             ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                                              & (2U 
                                                                 == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (1U 
                                                & (~ 
                                                   ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering) 
                                                    | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering_hold) 
                                                       | (0x0eU 
                                                          == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))))));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0 = (1U 
                                                & (~ 
                                                   ((1U 
                                                     == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                                    | ((2U 
                                                        == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                                       | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction) 
                                                          | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_hold) 
                                                             | (0x0eU 
                                                                == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))))))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_latched) 
           | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rearm_command));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data 
        = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
              ? (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble)
              : ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg) 
                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
            & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
               | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
           | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data) 
              & (- (IData)((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive)))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header 
        = ((0x00fffff0U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
                           << 4U)) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
}

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___eval_stl(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_stl\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_code_changed 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync) 
               != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__previous_hmi_code));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose 
            = ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d1)) 
               & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d0));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__gated_request 
            = (IData)((((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                        >> 1U) & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__use_spi_brake_request)
                                   ? (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_sync) 
                                         >> 4U)) : (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__raw_brake_request))));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) 
               & ((3U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen)) 
                  & ((0x00000020U > vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count) 
                     & ((0x30U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3)) 
                        & (0x90U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2))))));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering 
            = ((0x0cU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
               | (0x0dU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction 
            = ((0x0aU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
               | (0x0bU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__rearm_command 
            = ((0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
               & (7U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count)));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = 
            ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = 
            (0x0000000fU & (- (IData)(((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                       & (2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))))));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            (1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering) 
                      | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering_hold) 
                         | (0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))))));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0 = 
            (1U & (~ ((1U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                      | ((2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                         | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction) 
                            | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_hold) 
                               | (0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))))))));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_latched) 
               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rearm_command));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data 
            = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                  ? (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble)
                  : ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg) 
                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
                & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
                   | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
               | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data) 
                  & (- (IData)((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive)))));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
               & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
    }
}

VL_ATTR_COLD bool Vtest_hmi_auto_brake_switch___024root___eval_phase__stl(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_phase__stl\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_hmi_auto_brake_switch___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_auto_brake_switch___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_hmi_auto_brake_switch___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_hmi_auto_brake_switch___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_hmi_auto_brake_switch___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_auto_brake_switch___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_hmi_auto_brake_switch.pad_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_hmi_auto_brake_switch.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(posedge test_hmi_auto_brake_switch.uart_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @(posedge test_hmi_auto_brake_switch.spi_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @(posedge test_hmi_auto_brake_switch.spi_cs)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 5U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 5 is active: @(negedge test_hmi_auto_brake_switch.spi_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 6U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 6 is active: @(negedge test_hmi_auto_brake_switch.register_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 7U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 7 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 8U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 8 is active: @(negedge test_hmi_auto_brake_switch.pad_clk)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 9U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 9 is active: @(negedge test_hmi_auto_brake_switch.uart_clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___ctor_var_reset(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___ctor_var_reset\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_auto_brake_switch__DOT__hmi_code_sync = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13045153746361207562ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__gated_request = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 806391759704436872ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__rearm_command = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9357753977980472197ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__stop_pressed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18403927494349036876ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__movement_blocked = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14885292147454446979ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__spi_data = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 611937321882722315ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__fspi_button_meta = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 18028056010421662388ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__fspi_button_sync = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 5249438150732065183ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__legacy_read = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7747411341882949023ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 1807243843971833055ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6708296055842497641ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14724787176086696077ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 12125346916446660723ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 15169531739777160534ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12715505565084247751ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 1759664238977268786ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__code_meta = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 400157427693157827ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1317375156985017932ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_latched = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6993258188953051256ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_event_armed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17387435042493510342ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_stop__DOT__rearm_previous = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2696614308540584931ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6176480743775458022ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14533399035356156118ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13177435182616648146ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__previous_hmi_code = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10373442532107912874ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 13234808780200365582ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6563976716628335077ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_code_changed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12008949139401251479ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13766175694127864190ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1737289088508550110ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8618126450789124406ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16119534763192391006ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6013750541521559048ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 10578734440196884724ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12703302905980683284ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9749343871120674926ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_meta = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5432805883005044339ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_sync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6697325872093497955ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 1067810489525124872ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 16709918765442641762ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9977849961527509952ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17636468381958309070ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1918044487018802078ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 11796166593423900206ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header = VL_SCOPED_RAND_RESET_I(24, __VscopeHash, 10867893344642041851ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT____Vlvbound_h11419250__0 = 0;
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16963316733809289436ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 2618943899912328600ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__cmd_reg = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 16259746472562002078ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__spi_cs_reg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8823951632407960003ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 937254723438860672ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14251195149460889164ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8512426017245410000ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5367875315692412125ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 18254557787043283009ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__addr_high = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14788558843295143227ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18082287427580205935ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5305319560287756938ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 3321629560556315509ull);
    vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__read_addr_high = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3146640234629836608ull);
    for (int __Vi0 = 0; __Vi0 < 5; ++__Vi0) {
        vlSelf->test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 6337873647679135430ull);
    }
    vlSelf->__Vtask_test_hmi_auto_brake_switch__DOT__uart_command__518__function_code = 0;
    vlSelf->__Vtask_test_hmi_auto_brake_switch__DOT__uart_command__518__value = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_0 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_1 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_3 = 0;
    vlSelf->__VdfgRegularize_h6e95ff9d_0_4 = 0;
    vlSelf->__Vdly__test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe = 0;
    vlSelf->__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag = 0;
    vlSelf->__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count = 0;
    vlSelf->__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock = 0;
    vlSelf->__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__rst_n__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_clk__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_cs__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__register_clk__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
