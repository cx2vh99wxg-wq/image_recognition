// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_serial_release.h for the primary calling header

#include "Vtest_hmi_serial_release__pch.h"

void Vtest_hmi_serial_release___024root___timing_ready(Vtest_hmi_serial_release___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___eval_static(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___eval_static\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_serial_release__DOT__clk_50m = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__clk_25m = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__uart_rxd = 1U;
    vlSelfRef.test_hmi_serial_release__DOT__brake_request = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__fspi_released = 0x0fU;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_serial_release__DOT__send_byte__Vstatic__bit_index = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14337750635297060711ull);
    vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__cnn_level_reg = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__clk_50m__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__rst_n__0 = 0U;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__clk_25m__0 = 0U;
    Vtest_hmi_serial_release___024root___timing_ready(vlSelf);
    do {
        vlSelfRef.__VactTriggeredAcc[vlSelfRef.__Vi] 
            = vlSelfRef.__VactTriggered[vlSelfRef.__Vi];
        vlSelfRef.__Vi = ((IData)(1U) + vlSelfRef.__Vi);
    } while ((0U >= vlSelfRef.__Vi));
}

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___eval_static__TOP(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___eval_static__TOP\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_serial_release__DOT__clk_50m = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__clk_25m = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__rst_n = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__uart_rxd = 1U;
    vlSelfRef.test_hmi_serial_release__DOT__brake_request = 0U;
    vlSelfRef.test_hmi_serial_release__DOT__fspi_released = 0x0fU;
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_serial_release__DOT__send_byte__Vstatic__bit_index = VL_SCOPED_RAND_RESET_I(32, __VscopeHash, 14337750635297060711ull);
    vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__cnn_level_reg = 0U;
}

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___eval_final(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___eval_final\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_serial_release___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtest_hmi_serial_release___024root___eval_phase__stl(Vtest_hmi_serial_release___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___eval_settle(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___eval_settle\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VstlIterCount;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_serial_release___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_serial_release.v", 5, "", "DIDNOTCONVERGE: Settle region did not converge after '--converge-limit' of 10000 tries");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        vlSelfRef.__VstlPhaseResult = Vtest_hmi_serial_release___024root___eval_phase__stl(vlSelf);
        vlSelfRef.__VstlFirstIteration = 0U;
    } while (vlSelfRef.__VstlPhaseResult);
}

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___eval_triggers_vec__stl(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___eval_triggers_vec__stl\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered[0U] = ((0xfffffffffffffffeULL 
                                      & vlSelfRef.__VstlTriggered[0U]) 
                                     | (IData)((IData)(vlSelfRef.__VstlFirstIteration)));
}

VL_ATTR_COLD bool Vtest_hmi_serial_release___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_serial_release___024root___dump_triggers__stl(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_serial_release___024root___trigger_anySet__stl(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD bool Vtest_hmi_serial_release___024root___trigger_anySet__stl(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___trigger_anySet__stl\n"); );
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

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___stl_sequent__TOP__0(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___stl_sequent__TOP__0\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_code_changed 
        = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync) 
           != (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__previous_hmi_code));
    vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_complete 
        = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__uart_rx_valid) 
           & ((3U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__bytes_seen)) 
              & ((0x00004e20U > vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_gap_count) 
                 & ((0x30U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_3)) 
                    & (0x90U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_2))))));
    vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering 
        = ((0x0cU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)) 
           | (0x0dU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)));
    vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction 
        = ((0x0aU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)) 
           | (0x0bU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)));
    vlSelfRef.test_hmi_serial_release__DOT__rearm_command 
        = ((0x0eU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)) 
           & (7U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_hmi_control_synchronizer__DOT__e_stable_count)));
    vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering 
        = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering) 
           | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering_hold) 
              | (0x0eU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))));
    vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction 
        = ((1U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched)) 
           | ((2U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched)) 
              | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction) 
                 | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_hold) 
                    | (0x0eU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))))));
    vlSelfRef.test_hmi_serial_release__DOT__movement_blocked 
        = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__brake_latched) 
           | (IData)(vlSelfRef.test_hmi_serial_release__DOT__rearm_command));
    vlSelfRef.test_hmi_serial_release__DOT__stop_pad 
        = (1U & (~ ((IData)(vlSelfRef.test_hmi_serial_release__DOT__rst_n) 
                    & (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked))));
    vlSelfRef.test_hmi_serial_release__DOT__direction_pad 
        = ((((2U & ((((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering)
                       ? (0x0dU != (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))
                       : ((~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                          | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released) 
                             >> 3U))) | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)) 
                    << 1U)) | (1U & (((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering)
                                       ? (0x0cU != (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))
                                       : ((~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                                          | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released) 
                                             >> 2U))) 
                                     | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)))) 
            << 2U) | ((2U & ((((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction)
                                ? (2U != (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched))
                                : ((~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                                   | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released) 
                                      >> 1U))) | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)) 
                             << 1U)) | (1U & (((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction)
                                                ? (1U 
                                                   != (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched))
                                                : (
                                                   (~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                                                   | (IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released))) 
                                              | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)))));
}

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___eval_stl(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___eval_stl\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered[0U])) {
        vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_code_changed 
            = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync) 
               != (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__previous_hmi_code));
        vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_complete 
            = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__uart_rx_valid) 
               & ((3U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__bytes_seen)) 
                  & ((0x00004e20U > vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_gap_count) 
                     & ((0x30U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_3)) 
                        & (0x90U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_2))))));
        vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering 
            = ((0x0cU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)) 
               | (0x0dU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)));
        vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction 
            = ((0x0aU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)) 
               | (0x0bU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)));
        vlSelfRef.test_hmi_serial_release__DOT__rearm_command 
            = ((0x0eU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync)) 
               & (7U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_hmi_control_synchronizer__DOT__e_stable_count)));
        vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering 
            = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering) 
               | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering_hold) 
                  | (0x0eU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))));
        vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction 
            = ((1U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched)) 
               | ((2U == (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched)) 
                  | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction) 
                     | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_hold) 
                        | (0x0eU == (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))))));
        vlSelfRef.test_hmi_serial_release__DOT__movement_blocked 
            = ((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__brake_latched) 
               | (IData)(vlSelfRef.test_hmi_serial_release__DOT__rearm_command));
        vlSelfRef.test_hmi_serial_release__DOT__stop_pad 
            = (1U & (~ ((IData)(vlSelfRef.test_hmi_serial_release__DOT__rst_n) 
                        & (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked))));
        vlSelfRef.test_hmi_serial_release__DOT__direction_pad 
            = ((((2U & ((((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering)
                           ? (0x0dU != (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))
                           : ((~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                              | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released) 
                                 >> 3U))) | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)) 
                        << 1U)) | (1U & (((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering)
                                           ? (0x0cU 
                                              != (IData)(vlSelfRef.test_hmi_serial_release__DOT__hmi_code_sync))
                                           : ((~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                                              | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released) 
                                                 >> 2U))) 
                                         | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)))) 
                << 2U) | ((2U & ((((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction)
                                    ? (2U != (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched))
                                    : ((~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                                       | ((IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released) 
                                          >> 1U))) 
                                  | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)) 
                                 << 1U)) | (1U & (((IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction)
                                                    ? 
                                                   (1U 
                                                    != (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched))
                                                    : 
                                                   ((~ (IData)(vlSelfRef.test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed)) 
                                                    | (IData)(vlSelfRef.test_hmi_serial_release__DOT__fspi_released))) 
                                                  | (IData)(vlSelfRef.test_hmi_serial_release__DOT__movement_blocked)))));
    }
}

VL_ATTR_COLD bool Vtest_hmi_serial_release___024root___eval_phase__stl(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___eval_phase__stl\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtest_hmi_serial_release___024root___eval_triggers_vec__stl(vlSelf);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_serial_release___024root___dump_triggers__stl(vlSelfRef.__VstlTriggered, "stl"s);
    }
#endif
    __VstlExecute = Vtest_hmi_serial_release___024root___trigger_anySet__stl(vlSelfRef.__VstlTriggered);
    if (__VstlExecute) {
        Vtest_hmi_serial_release___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

bool Vtest_hmi_serial_release___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in);

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtest_hmi_serial_release___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(Vtest_hmi_serial_release___024root___trigger_anySet__act(triggers))))) {
        VL_DBG_MSGS("         No '" + tag + "' region triggers active\n");
    }
    if ((1U & (IData)(triggers[0U]))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 0 is active: @(posedge test_hmi_serial_release.clk_50m)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 1U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 1 is active: @(negedge test_hmi_serial_release.rst_n)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 2U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 2 is active: @(posedge test_hmi_serial_release.clk_25m)\n");
    }
    if ((1U & (IData)((triggers[0U] >> 3U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 3 is active: @([true] __VdlySched.awaitingCurrentTime())\n");
    }
    if ((1U & (IData)((triggers[0U] >> 4U)))) {
        VL_DBG_MSGS("         '" + tag + "' region trigger index 4 is active: @(negedge test_hmi_serial_release.clk_25m)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtest_hmi_serial_release___024root___ctor_var_reset(Vtest_hmi_serial_release___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_serial_release___024root___ctor_var_reset\n"); );
    Vtest_hmi_serial_release__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->vlNamep);
    vlSelf->test_hmi_serial_release__DOT__uart_rx_valid = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11353131682311790684ull);
    vlSelf->test_hmi_serial_release__DOT__uart_rx_data = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 16071137850196682570ull);
    vlSelf->test_hmi_serial_release__DOT__hmi_code_sync = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 4733242810550006990ull);
    vlSelf->test_hmi_serial_release__DOT__rearm_command = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12800352016366352893ull);
    vlSelf->test_hmi_serial_release__DOT__movement_blocked = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16542680100363756134ull);
    vlSelf->test_hmi_serial_release__DOT__stop_pad = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 520211381684531786ull);
    vlSelf->test_hmi_serial_release__DOT__direction_pad = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17715161280377272933ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_rx__DOT__uart_rxd_d0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6707796030206913422ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_rx__DOT__uart_rxd_d1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7562126168919499299ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_rx__DOT__uart_rxd_d2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9212845741972952185ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_rx__DOT__rx_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16812398625899632138ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_rx__DOT__rx_cnt = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5706919625388986802ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_rx__DOT__baud_cnt = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 15034860288752172212ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_rx__DOT__rx_data_t = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5759722483527941662ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_3 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 14764290827606367260ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_2 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 17769250305063883408ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_lcd__DOT__previous_1 = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 925957338420860237ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_lcd__DOT__bytes_seen = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4709753415684296105ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_gap_count = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 6972085863438675795ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_lcd__DOT__frame_complete = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3348771080180885577ull);
    vlSelf->test_hmi_serial_release__DOT__u_uart_lcd__DOT__traction_lease_count = VL_SCOPED_RAND_RESET_I(27, __VscopeHash, 12932393791665448128ull);
    vlSelf->test_hmi_serial_release__DOT__u_hmi_control_synchronizer__DOT__code_meta = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 8124091943049490507ull);
    vlSelf->test_hmi_serial_release__DOT__u_hmi_control_synchronizer__DOT__e_stable_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 4099387270918411244ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18129764739722926234ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9607566878116571982ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_latched = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 9629244039890696753ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__previous_hmi_code = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 10167354582719296875ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__traction_stable_count = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1793553546143791374ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__traction_press_consumed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14669997212326031849ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_code_changed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4171820678924725006ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_traction_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12710051716735156383ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_steering_hold = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3473906458944812515ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_traction = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11338479286675537431ull);
    vlSelf->test_hmi_serial_release__DOT__u_remote_direction_arbiter__DOT__hmi_owns_steering = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8732817956919389666ull);
    vlSelf->test_hmi_serial_release__DOT__u_stop__DOT__brake_latched = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15108932753186456010ull);
    vlSelf->test_hmi_serial_release__DOT__u_stop__DOT__brake_event_armed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14155138077379375449ull);
    vlSelf->test_hmi_serial_release__DOT__u_stop__DOT__rearm_previous = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14674961317668969264ull);
    vlSelf->test_hmi_serial_release__DOT__u_stop__DOT__fspi_armed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17057876258716106191ull);
    vlSelf->__Vdly__test_hmi_serial_release__DOT__u_uart_lcd__DOT__cnn_level_reg = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VstlTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggered[__Vi0] = 0;
    }
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VactTriggeredAcc[__Vi0] = 0;
    }
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__clk_50m__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__rst_n__0 = 0;
    vlSelf->__Vtrigprevexpr___TOP__test_hmi_serial_release__DOT__clk_25m__0 = 0;
    for (int __Vi0 = 0; __Vi0 < 1; ++__Vi0) {
        vlSelf->__VnbaTriggered[__Vi0] = 0;
    }
    vlSelf->__Vi = 0;
}
