// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_auto_brake_switch.h for the primary calling header

#include "Vtest_hmi_auto_brake_switch__pch.h"

void Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(Vtest_hmi_auto_brake_switch___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h1ff2e788__0(Vtest_hmi_auto_brake_switch___024root* vlSelf, const char* __VeventDescription);
extern const VlWide<30>/*959:0*/ Vtest_hmi_auto_brake_switch__ConstPool__CONST_h06d8a14d_0;
extern const VlWide<30>/*959:0*/ Vtest_hmi_auto_brake_switch__ConstPool__CONST_h4080b00f_0;
extern const VlWide<30>/*959:0*/ Vtest_hmi_auto_brake_switch__ConstPool__CONST_h7d9fc08b_0;
extern const VlWide<30>/*959:0*/ Vtest_hmi_auto_brake_switch__ConstPool__CONST_h911b7cb4_0;
extern const VlWide<30>/*959:0*/ Vtest_hmi_auto_brake_switch__ConstPool__CONST_hf94b97d7_0;
extern const VlWide<30>/*959:0*/ Vtest_hmi_auto_brake_switch__ConstPool__CONST_h3b79abc7_0;

VlCoroutine Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__0__1(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__0__1\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__520__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__520__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__521__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__521__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__522__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__522__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__settle__523__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__523__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__524__condition;
    __Vtask_test_hmi_auto_brake_switch__DOT__check__524__condition = 0;
    VlWide<30>/*959:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__524__description;
    VL_ZERO_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__524__description);
    SData/*15:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__address;
    __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__address = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__expected;
    __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__expected = 0;
    CData/*0:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__change_switch_between_nibbles;
    __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__change_switch_between_nibbles = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__526__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__526__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__527__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__527__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__528__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__528__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__529__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__529__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__530__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__530__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__531__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__531__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__532__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__532__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__533__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__533__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__534__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__534__value = 0;
    CData/*0:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__535__condition;
    __Vtask_test_hmi_auto_brake_switch__DOT__check__535__condition = 0;
    VlWide<30>/*959:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__535__description;
    VL_ZERO_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__535__description);
    CData/*0:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__536__condition;
    __Vtask_test_hmi_auto_brake_switch__DOT__check__536__condition = 0;
    VlWide<30>/*959:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__536__description;
    VL_ZERO_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__536__description);
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__537__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__537__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__function_code;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__function_code = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__539__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__539__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__540__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__540__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__541__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__541__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__542__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__542__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__settle__543__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__543__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__544__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__544__value = 0;
    SData/*15:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__address;
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__address = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__546__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__546__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__547__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__547__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__548__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__548__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__549__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__549__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__550__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__550__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__551__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__551__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__552__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__552__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__553__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__553__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__554__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__554__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__555__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__555__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__556__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__556__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__557__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__557__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__558__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__558__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__559__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__559__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__560__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__560__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__settle__561__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__561__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__562__condition;
    __Vtask_test_hmi_auto_brake_switch__DOT__check__562__condition = 0;
    VlWide<30>/*959:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__562__description;
    VL_ZERO_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__562__description);
    SData/*15:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__address;
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__address = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__564__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__564__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__565__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__565__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__566__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__566__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__567__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__567__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__568__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__568__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__569__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__569__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__570__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__570__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__571__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__571__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__572__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__572__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__573__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__573__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__574__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__574__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__575__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__575__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__576__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__576__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__577__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__577__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__578__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__578__value = 0;
    IData/*31:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__settle__579__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__579__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    CData/*0:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__580__condition;
    __Vtask_test_hmi_auto_brake_switch__DOT__check__580__condition = 0;
    VlWide<30>/*959:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__580__description;
    VL_ZERO_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__580__description);
    CData/*0:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__581__condition;
    __Vtask_test_hmi_auto_brake_switch__DOT__check__581__condition = 0;
    VlWide<30>/*959:0*/ __Vtask_test_hmi_auto_brake_switch__DOT__check__581__description;
    VL_ZERO_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__581__description);
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         103);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__520__value = 0x90U;
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         100);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__520__value;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         103);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__521__value 
        = vlSelfRef.__Vtask_test_hmi_auto_brake_switch__DOT__uart_command__518__function_code;
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         100);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__521__value;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         103);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__522__value 
        = vlSelfRef.__Vtask_test_hmi_auto_brake_switch__DOT__uart_command__518__value;
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         100);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__522__value;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         103);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__523__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__523__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000010U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_auto_brake_switch__DOT__settle__523__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h1ff2e788__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.pad_clk)");
        co_await vlSelfRef.__VtrigSched_h1ff2e788__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.pad_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             94);
        __Vtask_test_hmi_auto_brake_switch__DOT__settle__523__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_auto_brake_switch__DOT__settle__523__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         95);
    VL_ASSIGN_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__524__description, Vtest_hmi_auto_brake_switch__ConstPool__CONST_h06d8a14d_0);
    __Vtask_test_hmi_auto_brake_switch__DOT__check__524__condition 
        = (1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed)) 
                 & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))));
    if (VL_UNLIKELY(((1U & (~ (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__check__524__condition)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:117: Assertion failed in %m: FAIL %0s: auto=%b sync=%b selector=%h directions=%h STOP=%b\n",9, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch.check", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',960,__Vtask_test_hmi_auto_brake_switch__DOT__check__524__description.data()
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg)
                     , '#',1,(1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                                    >> 1U)), '#',4,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg)
                     , '#',4,(((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                 | ((0x0dU != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 3U) | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                           | ((0x0cU 
                                               != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                              | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                          << 2U)) | 
                              ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                 | ((2U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 1U) | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                          | ((1U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                             | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked)))))
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 117, "", false);
    }
    __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__change_switch_between_nibbles = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__expected = 0xa0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__address = 5U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__526__value = 0x81U;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__527__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__526__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__527__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__528__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__526__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__528__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__529__value 
        = (0x000000ffU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__address) 
                          >> 8U));
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__530__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__529__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__530__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__531__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__529__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__531__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__532__value 
        = (0x000000ffU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__address));
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__533__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__532__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__533__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__534__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__532__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__534__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    VL_ASSIGN_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__535__description, Vtest_hmi_auto_brake_switch__ConstPool__CONST_h4080b00f_0);
    __Vtask_test_hmi_auto_brake_switch__DOT__check__535__condition 
        = ((0U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg)) 
           & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid)));
    if (VL_UNLIKELY(((1U & (~ (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__check__535__condition)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:117: Assertion failed in %m: FAIL %0s: auto=%b sync=%b selector=%h directions=%h STOP=%b\n",9, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch.check", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',960,__Vtask_test_hmi_auto_brake_switch__DOT__check__535__description.data()
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg)
                     , '#',1,(1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                                    >> 1U)), '#',4,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg)
                     , '#',4,(((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                 | ((0x0dU != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 3U) | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                           | ((0x0cU 
                                               != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                              | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                          << 2U)) | 
                              ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                 | ((2U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 1U) | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                          | ((1U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                             | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked)))))
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 117, "", false);
    }
    VL_ASSIGN_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__536__description, Vtest_hmi_auto_brake_switch__ConstPool__CONST_h7d9fc08b_0);
    __Vtask_test_hmi_auto_brake_switch__DOT__check__536__condition 
        = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started)));
    if (VL_UNLIKELY(((1U & (~ (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__check__536__condition)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:117: Assertion failed in %m: FAIL %0s: auto=%b sync=%b selector=%h directions=%h STOP=%b\n",9, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch.check", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',960,__Vtask_test_hmi_auto_brake_switch__DOT__check__536__description.data()
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg)
                     , '#',1,(1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                                    >> 1U)), '#',4,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg)
                     , '#',4,(((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                 | ((0x0dU != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 3U) | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                           | ((0x0cU 
                                               != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                              | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                          << 2U)) | 
                              ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                 | ((2U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 1U) | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                          | ((1U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                             | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked)))))
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 117, "", false);
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__537__value = 0;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         136);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         137);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002328ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         138);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         139);
    __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__537__value 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         140);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received 
        = ((0x0fU & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received)) 
           | ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__537__value) 
              << 4U));
    if (__Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__change_switch_between_nibbles) {
        __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__value = 0xeeU;
        __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__function_code = 0x0fU;
        __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__539__value = 0x30U;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             100);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data 
            = __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__539__value;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             103);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
        __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__540__value = 0x90U;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             100);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data 
            = __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__540__value;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             103);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
        __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__541__value 
            = __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__function_code;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             100);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data 
            = __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__541__value;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             103);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
        __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__542__value 
            = __Vtask_test_hmi_auto_brake_switch__DOT__uart_command__538__value;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             100);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data 
            = __Vtask_test_hmi_auto_brake_switch__DOT__uart_byte__542__value;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 1U;
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
        co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             103);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid = 0U;
        __Vtask_test_hmi_auto_brake_switch__DOT__settle__543__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
        __Vtask_test_hmi_auto_brake_switch__DOT__settle__543__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000010U;
        while (VL_LTS_III(32, 0U, __Vtask_test_hmi_auto_brake_switch__DOT__settle__543__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
            Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h1ff2e788__0(vlSelf, 
                                                                              "@(negedge test_hmi_auto_brake_switch.pad_clk)");
            co_await vlSelfRef.__VtrigSched_h1ff2e788__0.trigger(0U, 
                                                                 nullptr, 
                                                                 "@(negedge test_hmi_auto_brake_switch.pad_clk)", 
                                                                 "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                                 94);
            __Vtask_test_hmi_auto_brake_switch__DOT__settle__543__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
                = (__Vtask_test_hmi_auto_brake_switch__DOT__settle__543__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
                   - (IData)(1U));
        }
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                             95);
    }
    __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__544__value = 0;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         136);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         137);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002328ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         138);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         139);
    __Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__544__value 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         140);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received 
        = ((0xf0U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received)) 
           | (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__receive_nibble__544__value));
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received) 
                      != (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__expected))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:160: Assertion failed in %m: FAIL SPI81 address=%h received=%h expected=%h\n",6, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch.read_status", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',16,(IData)(__Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__address)
                     , '#',8,vlSelfRef.test_hmi_auto_brake_switch__DOT__read_status__Vstatic__received
                     , '#',8,(IData)(__Vtask_test_hmi_auto_brake_switch__DOT__read_status__525__expected));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 160, "", false);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         161);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         162);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive = 1U;
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__value = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__address = 4U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         179);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__546__value = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__547__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__546__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__547__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__548__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__546__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__548__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__549__value 
        = (0x000000ffU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__address) 
                          >> 8U));
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__550__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__549__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__550__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__551__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__549__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__551__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__552__value 
        = (0x000000ffU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__address));
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__553__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__552__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__553__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__554__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__552__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__554__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__555__value 
        = __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__545__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__556__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__555__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__556__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__557__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__555__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__557__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__558__value = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__559__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__558__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__559__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__560__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__558__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__560__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         182);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         183);
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__561__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__561__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000010U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_auto_brake_switch__DOT__settle__561__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h1ff2e788__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.pad_clk)");
        co_await vlSelfRef.__VtrigSched_h1ff2e788__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.pad_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             94);
        __Vtask_test_hmi_auto_brake_switch__DOT__settle__561__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_auto_brake_switch__DOT__settle__561__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         95);
    VL_ASSIGN_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__562__description, Vtest_hmi_auto_brake_switch__ConstPool__CONST_h911b7cb4_0);
    __Vtask_test_hmi_auto_brake_switch__DOT__check__562__condition 
        = ((((0U == vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[4U]) 
             & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_sync) 
                   >> 4U))) & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__gated_request))) 
           & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed)));
    if (VL_UNLIKELY(((1U & (~ (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__check__562__condition)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:117: Assertion failed in %m: FAIL %0s: auto=%b sync=%b selector=%h directions=%h STOP=%b\n",9, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch.check", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',960,__Vtask_test_hmi_auto_brake_switch__DOT__check__562__description.data()
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg)
                     , '#',1,(1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                                    >> 1U)), '#',4,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg)
                     , '#',4,(((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                 | ((0x0dU != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 3U) | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                           | ((0x0cU 
                                               != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                              | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                          << 2U)) | 
                              ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                 | ((2U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 1U) | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                          | ((1U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                             | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked)))))
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 117, "", false);
    }
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__value = 1U;
    __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__address = 4U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive = 1U;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         179);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__564__value = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__565__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__564__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__565__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__566__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__564__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__566__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__567__value 
        = (0x000000ffU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__address) 
                          >> 8U));
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__568__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__567__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__568__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__569__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__567__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__569__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__570__value 
        = (0x000000ffU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__address));
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__571__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__570__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__571__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__572__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__570__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__572__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__573__value 
        = __Vtask_test_hmi_auto_brake_switch__DOT__write_spi_register__563__value;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__574__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__573__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__574__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__575__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__573__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__575__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__576__value = 0U;
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__577__value 
        = (0x0000000fU & ((IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__576__value) 
                          >> 4U));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__577__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__578__value 
        = (0x0000000fU & (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__spi_byte__576__value));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data 
        = __Vtask_test_hmi_auto_brake_switch__DOT__spi_nibble__578__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         124);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         125);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         126);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         182);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         183);
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__579__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    __Vtask_test_hmi_auto_brake_switch__DOT__settle__579__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0x00000010U;
    while (VL_LTS_III(32, 0U, __Vtask_test_hmi_auto_brake_switch__DOT__settle__579__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h1ff2e788__0(vlSelf, 
                                                                          "@(negedge test_hmi_auto_brake_switch.pad_clk)");
        co_await vlSelfRef.__VtrigSched_h1ff2e788__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(negedge test_hmi_auto_brake_switch.pad_clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                             94);
        __Vtask_test_hmi_auto_brake_switch__DOT__settle__579__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (__Vtask_test_hmi_auto_brake_switch__DOT__settle__579__test_hmi_auto_brake_switch__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         95);
    VL_ASSIGN_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__580__description, Vtest_hmi_auto_brake_switch__ConstPool__CONST_hf94b97d7_0);
    __Vtask_test_hmi_auto_brake_switch__DOT__check__580__condition 
        = ((((((1U == vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[4U]) 
               & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed))) 
              & (1U == vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[0U])) 
             & (1U == vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[1U])) 
            & (1U == vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[2U])) 
           & (1U == vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[3U]));
    if (VL_UNLIKELY(((1U & (~ (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__check__580__condition)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:117: Assertion failed in %m: FAIL %0s: auto=%b sync=%b selector=%h directions=%h STOP=%b\n",9, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch.check", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',960,__Vtask_test_hmi_auto_brake_switch__DOT__check__580__description.data()
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg)
                     , '#',1,(1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                                    >> 1U)), '#',4,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg)
                     , '#',4,(((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                 | ((0x0dU != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 3U) | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                           | ((0x0cU 
                                               != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                              | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                          << 2U)) | 
                              ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                 | ((2U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 1U) | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                          | ((1U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                             | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked)))))
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 117, "", false);
    }
    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(vlSelf, 
                                                                      "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    co_await vlSelfRef.__VtrigSched_h482006c0__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_auto_brake_switch.uart_clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                                         324);
    vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         325);
    VL_ASSIGN_W(960, __Vtask_test_hmi_auto_brake_switch__DOT__check__581__description, Vtest_hmi_auto_brake_switch__ConstPool__CONST_h3b79abc7_0);
    __Vtask_test_hmi_auto_brake_switch__DOT__check__581__condition 
        = (1U & (((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg)) 
                  & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                        >> 1U))) & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed))));
    if (VL_UNLIKELY(((1U & (~ (IData)(__Vtask_test_hmi_auto_brake_switch__DOT__check__581__condition)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:117: Assertion failed in %m: FAIL %0s: auto=%b sync=%b selector=%h directions=%h STOP=%b\n",9, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch.check", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',960,__Vtask_test_hmi_auto_brake_switch__DOT__check__581__description.data()
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg)
                     , '#',1,(1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                                    >> 1U)), '#',4,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg)
                     , '#',4,(((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                 | ((0x0dU != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 3U) | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                                           | ((0x0cU 
                                               != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
                                              | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                          << 2U)) | 
                              ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                 | ((2U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                    | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked))) 
                                << 1U) | ((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_0) 
                                          | ((1U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched)) 
                                             | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked)))))
                     , '#',1,(IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 117, "", false);
    }
    VL_WRITEF_NX("PASS: UART FF/EE + SPI81 read + SPI00 register4 write + CDC + latched STOP pad + REARM, with legacy controls\n",0);
    VL_FINISH_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 329, "");
    co_return;
}

VlCoroutine Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__1(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x000000003b9aca00ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                         332);
    VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_auto_brake_switch.v:333: Assertion failed in %m: FAIL independent automatic-brake switch test timed out\n",3, 'M',vlSymsp->name(),"test_hmi_auto_brake_switch", 'T',-9
                 , '#',64,VL_TIME_UNITED_Q(1000));
    VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 333, "", false);
    co_return;
}

VlCoroutine Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__2(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__2\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x00000000000004e2ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                             64);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__register_clk 
            = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__register_clk)));
    }
    co_return;
}

VlCoroutine Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__3(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__3\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                             9);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk 
            = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk)));
    }
    co_return;
}

VlCoroutine Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__4(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_initial__TOP__Vtiming__4\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 
                                             8);
        vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk 
            = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk)));
    }
    co_return;
}

void Vtest_hmi_auto_brake_switch___024root___eval_triggers_vec__act(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_triggers_vec__act\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk)) 
                                                        & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0)) 
                                                       << 9U) 
                                                      | (((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0)) 
                                                         << 8U)) 
                                                     | (((((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                            << 3U) 
                                                           | (((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__register_clk)) 
                                                               & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__register_clk__0)) 
                                                              << 2U)) 
                                                          | ((((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk)) 
                                                               & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_clk__0)) 
                                                              << 1U) 
                                                             | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs) 
                                                                & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_cs__0))))) 
                                                         << 4U) 
                                                        | (((((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk) 
                                                              & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_clk__0))) 
                                                             << 3U) 
                                                            | (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk) 
                                                                & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0))) 
                                                               << 2U)) 
                                                           | ((((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
                                                                & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__rst_n__0)) 
                                                               << 1U) 
                                                              | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk) 
                                                                 & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0)))))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__rst_n__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_clk__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__spi_cs__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__register_clk__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__register_clk;
}

bool Vtest_hmi_auto_brake_switch___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___trigger_anySet__act\n"); );
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

void Vtest_hmi_auto_brake_switch___024root___act_comb__TOP__0(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___act_comb__TOP__0\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked));
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
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = (0x0000000fU 
                                                & (- (IData)(
                                                             ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                                              & (2U 
                                                                 == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data 
        = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
              ? (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble)
              : ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg) 
                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
            & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
               | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
           | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data) 
              & (- (IData)((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive)))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header 
        = ((0x00fffff0U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
                           << 4U)) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
}

void Vtest_hmi_auto_brake_switch___024root___eval_act(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_act\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000380ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
               & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked));
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
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = 
            ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = 
            (0x0000000fU & (- (IData)(((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                       & (2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))))));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data 
            = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                  ? (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble)
                  : ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg) 
                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
                & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
                   | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
               | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data) 
                  & (- (IData)((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive)))));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
    }
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__0(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__0\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag;
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__1(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__1\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*19:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count = 0;
    // Body
    __Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count;
    if (vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) {
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen 
                = ((0x00000020U <= vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count)
                    ? 1U : ((3U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen))
                             ? 3U : (3U & ((IData)(1U) 
                                           + (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen)))));
            __Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3 
                = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2 
                = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1;
        } else if ((0x00000020U > vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count)) {
            __Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count 
                = (0x000fffffU & ((IData)(1U) + vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count));
        } else {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen = 0U;
        }
    } else {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen = 0U;
        __Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3 = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2 = 0U;
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count 
        = __Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count;
}

extern const VlUnpacked<CData/*0:0*/, 64> Vtest_hmi_auto_brake_switch__ConstPool__TABLE_hf85a0a8a_0;
extern const VlUnpacked<CData/*3:0*/, 64> Vtest_hmi_auto_brake_switch__ConstPool__TABLE_h69508fcc_0;
extern const VlUnpacked<CData/*0:0*/, 64> Vtest_hmi_auto_brake_switch__ConstPool__TABLE_hd507cf71_0;
extern const VlUnpacked<CData/*0:0*/, 64> Vtest_hmi_auto_brake_switch__ConstPool__TABLE_hb707fd3f_0;

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__2(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__2\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*5:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    CData/*2:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count = 0;
    CData/*0:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed = 0;
    // Body
    vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed;
    __Vtableidx1 = (((((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__rearm_previous)) 
                       & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rearm_command)) 
                      << 5U) | (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__gated_request) 
                                 << 4U) | (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_event_armed) 
                                            & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__gated_request)) 
                                           << 3U))) 
                    | (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_latched) 
                        << 2U) | (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rearm_command) 
                                   << 1U) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__rearm_previous 
        = Vtest_hmi_auto_brake_switch__ConstPool__TABLE_hf85a0a8a_0
        [__Vtableidx1];
    if ((4U & Vtest_hmi_auto_brake_switch__ConstPool__TABLE_h69508fcc_0
         [__Vtableidx1])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_event_armed 
            = Vtest_hmi_auto_brake_switch__ConstPool__TABLE_hd507cf71_0
            [__Vtableidx1];
    }
    if ((8U & Vtest_hmi_auto_brake_switch__ConstPool__TABLE_h69508fcc_0
         [__Vtableidx1])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_stop__DOT__brake_latched 
            = Vtest_hmi_auto_brake_switch__ConstPool__TABLE_hb707fd3f_0
            [__Vtableidx1];
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering_hold 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering) 
               | (0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_hold 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction) 
               | (0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))));
    if (vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe 
            = ((2U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                      << 1U)) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg));
        if ((0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))) {
            if ((7U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count))) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count 
                    = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count)));
            }
        } else {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count = 0U;
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_sync 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_meta;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__previous_hmi_code 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync;
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_code_changed) {
            __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count = 0U;
        } else if ((7U != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count))) {
            __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count)));
        }
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched = 0U;
            __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed = 1U;
        } else if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_code_changed) {
            __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed = 0U;
        } else if (((7U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count)) 
                    & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed)))) {
            __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed = 1U;
            if ((0x0aU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched = 1U;
            } else if ((0x0bU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync))) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched = 2U;
            }
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_meta 
            = ((((4U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[4U] 
                        << 2U)) | ((2U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[3U] 
                                          << 1U)) | 
                                   (1U & vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[2U]))) 
                << 2U) | ((2U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[1U] 
                                 << 1U)) | (1U & vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[0U])));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__code_meta;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__code_meta 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg;
    } else {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_sync = 0x1fU;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__previous_hmi_code = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction_latched = 0U;
        __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count = 0U;
        __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_meta = 0x1fU;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__code_meta = 0U;
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count 
        = __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_stable_count;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed 
        = __Vdly__test_hmi_auto_brake_switch__DOT__u_directions__DOT__traction_press_consumed;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_code_changed 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync) 
           != (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__previous_hmi_code));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_steering 
        = ((0x0cU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
           | (0x0dU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_directions__DOT__hmi_traction 
        = ((0x0aU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
           | (0x0bU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__rearm_command 
        = ((0x0eU == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__hmi_code_sync)) 
           & (7U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi_cdc__DOT__e_stable_count)));
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
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__3(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__3\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock;
    vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count;
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__4(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__4\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg;
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg = 0U;
    } else if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        if ((2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))) {
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg 
                = (1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg)
                          ? (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg))
                          : (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg))));
        }
    }
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__5(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__5\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started = 0;
    // Body
    __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started;
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble = 0x0fU;
    } else if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready) {
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble 
                = (0x0000000fU & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data));
        } else {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble 
                = (0x0000000fU & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data) 
                                  >> 4U));
            __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started = 1U;
        }
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started 
        = __Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started;
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__6(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__6\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*2:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 0;
    CData/*1:0*/ __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 0;
    CData/*7:0*/ __VdlyVal__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0;
    __VdlyVal__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0 = 0;
    CData/*2:0*/ __VdlyDim0__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0;
    __VdlyDim0__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0;
    __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v1;
    __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v1 = 0;
    // Body
    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state;
    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt;
    __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0 = 0U;
    __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v1 = 0U;
    if (vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) {
        if (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en) 
             & (6U > (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr)))) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT____Vlvbound_heb0d0599__0 
                = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg;
            if ((5U >= (7U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr)))) {
                __VdlyVal__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0 
                    = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT____Vlvbound_heb0d0599__0;
                __VdlyDim0__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0 
                    = (7U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr));
                __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0 = 1U;
            }
        }
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__spi_cs_reg) {
            __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en = 0U;
            __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg = 0xffU;
        } else {
            if ((4U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state))) {
                if ((2U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state))) {
                    if ((1U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state))) {
                        __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 0U;
                    } else if (((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d0)) 
                                & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d1))) {
                        __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 0U;
                    }
                } else if ((1U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state))) {
                    if ((2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt))) {
                        __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 6U;
                    }
                } else if (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose) 
                            & (1U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt)))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr 
                        = (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__read_addr_high) 
                            << 8U) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg));
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 2U;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 5U;
                }
            } else if ((2U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state))) {
                if ((1U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en = 0U;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 0U;
                } else if (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose) 
                            & (2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt)))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en = 1U;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 3U;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 3U;
                }
            } else if ((1U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state))) {
                if (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose) 
                     & (1U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt)))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr 
                        = (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__addr_high) 
                            << 8U) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg));
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 2U;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 2U;
                }
            } else {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en = 0U;
                __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 0U;
                if (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose) 
                     & (1U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__cmd_reg)))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__addr_high 
                        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 1U;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 1U;
                } else if (((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose) 
                            & (2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__cmd_reg)))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__read_addr_high 
                        = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 1U;
                    __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 4U;
                }
            }
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg 
                = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data;
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__cmd_reg 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg;
    } else {
        __VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v1 = 1U;
        __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_addr = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__ram_wr_en = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__addr_high = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__read_addr_high = 0U;
        __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__wr_data_reg = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__cmd_reg = 0U;
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d1 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d0));
    if (__VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[__VdlyDim0__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0] 
            = __VdlyVal__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v0;
    }
    if (__VdlySet__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem__v1) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[0U] = 1U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[1U] = 1U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[2U] = 1U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[3U] = 1U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[4U] = 1U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__mem[5U] = 0U;
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d0 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state 
        = __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__state;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt 
        = __Vdly__test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__byte_cnt;
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_pose 
        = ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d1)) 
           & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__rx_valid_d0));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d1 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d0));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__tx_done_d0 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_spi_registers__DOT__spi_cs_reg 
        = ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n))) 
           || (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs));
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__7(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__7\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) {
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete) {
            if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                          >> 7U)))) {
                if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                              >> 6U)))) {
                    if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                  >> 5U)))) {
                        if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                      >> 4U)))) {
                            if ((8U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                if ((4U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                    if ((2U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                        if ((1U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                            if ((0xffU 
                                                 == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data))) {
                                                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 1U;
                                            } else if (
                                                       (0xeeU 
                                                        == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data))) {
                                                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 0U;
                                            }
                                        }
                                    }
                                }
                            }
                            if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                          >> 3U)))) {
                                if ((4U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                    if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                                  >> 1U)))) {
                                        if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1)))) {
                                            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg 
                                                = (0x0000000fU 
                                                   & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1 
                = vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data;
        }
    } else {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1 = 0U;
    }
}

void Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__0(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__0\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked));
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__8(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__8\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header = 0U;
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data = 0xffU;
    } else if ((6U > (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count))) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count 
            = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count)));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header;
        if ((IData)(((5U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count)) 
                     & (0x00810000U == (0x00ff0000U 
                                        & vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header))))) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready = 1U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data 
                = ((5U == (0x0000ffffU & vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header))
                    ? ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_sync)
                        ? 0xa1U : 0xa0U) : 0xffU);
        }
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count 
        = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count;
}

void Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__1(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__1\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started));
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__9(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__9\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid = 0U;
    } else if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag) 
               & (1U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg)));
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_sync 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_meta));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_meta 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
           && (1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                     >> 1U)));
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__10(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__10\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg 
        = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg;
}

void Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__2(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__2\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete 
        = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) 
           & ((3U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen)) 
              & ((0x00000020U > vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count) 
                 & ((0x30U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3)) 
                    & (0x90U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2))))));
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__11(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__11\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg = 0U;
    } else if (((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag))) {
        if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock)))) {
            if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                           | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data)))) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg = 2U;
            } else if ((0U == ((0x000000f0U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data)))) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg = 1U;
            }
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock = 1U;
        }
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock 
        = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock;
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__12(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__12\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe 
        = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe;
}

void Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__13(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__13\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag = 0U;
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data = 0xffU;
    } else if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag) {
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag)));
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data 
                = ((0xf0U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
        } else {
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag)));
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data 
                = ((0x0fU & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                   | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data) 
                      << 4U));
        }
    }
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
        = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag;
}

void Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__3(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__3\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = (0x0000000fU 
                                                & (- (IData)(
                                                             ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                                              & (2U 
                                                                 == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))))));
}

void Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__4(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__4\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__gated_request 
        = (IData)((((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                    >> 1U) & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__use_spi_brake_request)
                               ? (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_sync) 
                                     >> 4U)) : (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__raw_brake_request))));
}

void Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__5(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___nba_comb__TOP__5\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data 
        = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
              ? (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble)
              : ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg) 
                 & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
            & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
               | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
           | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data) 
              & (- (IData)((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive)))));
    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header 
        = ((0x00fffff0U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
                           << 4U)) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
}

void Vtest_hmi_auto_brake_switch___024root___eval_nba(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_nba\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*19:0*/ __Vinline__nba_sequent__TOP__1___Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count;
    __Vinline__nba_sequent__TOP__1___Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count = 0;
    CData/*0:0*/ __Vinline__nba_sequent__TOP__5___Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started;
    __Vinline__nba_sequent__TOP__5___Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started = 0;
    // Body
    if ((0x000000000000000aULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag;
    }
    if ((6ULL & vlSelfRef.__VnbaTriggered[0U])) {
        __Vinline__nba_sequent__TOP__1___Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count;
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) {
            if (vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen 
                    = ((0x00000020U <= vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count)
                        ? 1U : ((3U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen))
                                 ? 3U : (3U & ((IData)(1U) 
                                               + (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen)))));
                __Vinline__nba_sequent__TOP__1___Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count = 0U;
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3 
                    = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2;
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2 
                    = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1;
            } else if ((0x00000020U > vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count)) {
                __Vinline__nba_sequent__TOP__1___Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count 
                    = (0x000fffffU & ((IData)(1U) + vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count));
            } else {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen = 0U;
            }
        } else {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen = 0U;
            __Vinline__nba_sequent__TOP__1___Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3 = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2 = 0U;
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count 
            = __Vinline__nba_sequent__TOP__1___Vdly__test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count;
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__2(vlSelf);
    }
    if ((0x000000000000001aULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock;
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count;
    }
    if ((0x0000000000000022ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg;
        if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg = 0U;
        } else if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            if ((2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))) {
                vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg 
                    = (1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg)
                              ? (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg))
                              : (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg))));
            }
        }
    }
    if ((0x0000000000000032ULL & vlSelfRef.__VnbaTriggered[0U])) {
        __Vinline__nba_sequent__TOP__5___Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started 
            = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started;
        if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            __Vinline__nba_sequent__TOP__5___Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble = 0x0fU;
        } else if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready) {
            if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble 
                    = (0x0000000fU & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data));
            } else {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble 
                    = (0x0000000fU & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data) 
                                      >> 4U));
                __Vinline__nba_sequent__TOP__5___Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started = 1U;
            }
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started 
            = __Vinline__nba_sequent__TOP__5___Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started;
    }
    if ((0x0000000000000042ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_hmi_auto_brake_switch___024root___nba_sequent__TOP__6(vlSelf);
    }
    if ((6ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if (vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) {
            if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete) {
                if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                              >> 7U)))) {
                    if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                  >> 6U)))) {
                        if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                      >> 5U)))) {
                            if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                          >> 4U)))) {
                                if ((8U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                    if ((4U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                        if ((2U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                            if ((1U 
                                                 & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                                if (
                                                    (0xffU 
                                                     == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data))) {
                                                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 1U;
                                                } else if (
                                                           (0xeeU 
                                                            == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data))) {
                                                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 0U;
                                                }
                                            }
                                        }
                                    }
                                }
                                if ((1U & (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                              >> 3U)))) {
                                    if ((4U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1))) {
                                        if ((1U & (~ 
                                                   ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1) 
                                                    >> 1U)))) {
                                            if ((1U 
                                                 & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1)))) {
                                                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg 
                                                    = 
                                                    (0x0000000fU 
                                                     & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1 
                    = vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_data;
            }
        } else {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__auto_brake_enable_reg = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_1 = 0U;
        }
    }
    if ((0x0000000000000383ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__stop_pressed 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
               & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__movement_blocked));
    }
    if ((0x000000000000001aULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header = 0U;
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data = 0xffU;
        } else if ((6U > (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count))) {
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count)));
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
                = vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header;
            if ((IData)(((5U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count)) 
                         & (0x00810000U == (0x00ff0000U 
                                            & vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header))))) {
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_ready = 1U;
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_read_data 
                    = ((5U == (0x0000ffffU & vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header))
                        ? ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_sync)
                            ? 0xa1U : 0xa0U) : 0xffU);
            }
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count 
            = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_nibble_count;
    }
    if ((0x00000000000003b2ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3 = 
            ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_started));
    }
    if ((0x000000000000000aULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid = 0U;
        } else if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_valid 
                = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag) 
                   & (1U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg)));
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_sync 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
               && (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_meta));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__auto_enable_meta 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n) 
               && (1U & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                         >> 1U)));
    }
    if ((0x0000000000000022ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg 
            = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__tx_valid_reg;
    }
    if ((0x0000000000000386ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_complete 
            = ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_valid) 
               & ((3U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__bytes_seen)) 
                  & ((0x00000020U > vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__frame_gap_count) 
                     & ((0x30U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_3)) 
                        & (0x90U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__previous_2))))));
    }
    if ((0x000000000000001aULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg = 0U;
        } else if (((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                    & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag))) {
            if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock)))) {
                if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                               | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data)))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg = 2U;
                } else if ((0U == ((0x000000f0U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data)))) {
                    vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg = 1U;
                }
                vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock = 1U;
            }
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock 
            = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_lock;
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe 
            = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe;
    }
    if ((0x000000000000000aULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag = 0U;
            vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data = 0xffU;
        } else if ((1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)))) {
            if (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag) {
                vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
                    = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag)));
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data 
                    = ((0xf0U & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                       | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
            } else {
                vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
                    = (1U & (~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag)));
                vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data 
                    = ((0x0fU & (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_data)) 
                       | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data) 
                          << 4U));
            }
        }
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag 
            = vlSelfRef.__Vdly__test_hmi_auto_brake_switch__DOT__u_fspi__DOT__recv_flag;
    }
    if ((0x000000000000039aULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4 = 
            (0x0000000fU & (- (IData)(((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_cs)) 
                                       & (2U == (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__cmd_reg))))));
    }
    if ((0x0000000000000383ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__gated_request 
            = (IData)((((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_enable_gate__DOT__enable_pipe) 
                        >> 1U) & ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__use_spi_brake_request)
                                   ? (~ ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__fspi_button_sync) 
                                         >> 4U)) : (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__raw_brake_request))));
    }
    if ((0x00000000000003beULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data 
            = ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3)
                  ? (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_tx_nibble)
                  : ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__u_hmi__DOT__cnn_level_reg) 
                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
                & ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_3))) 
                   | (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_4))) 
               | ((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_data) 
                  & (- (IData)((IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__master_drive)))));
        vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_hmi_auto_brake_switch__DOT__u_fspi__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__spi_data));
    }
}

void Vtest_hmi_auto_brake_switch___024root___timing_ready(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___timing_ready\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000100ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready("@(negedge test_hmi_auto_brake_switch.pad_clk)");
    }
    if ((0x0000000000000200ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h482006c0__0.ready("@(negedge test_hmi_auto_brake_switch.uart_clk)");
    }
}

void Vtest_hmi_auto_brake_switch___024root___timing_resume(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___timing_resume\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h1ff2e788__0.moveToResumeQueue(
                                                          "@(negedge test_hmi_auto_brake_switch.pad_clk)");
    vlSelfRef.__VtrigSched_h482006c0__0.moveToResumeQueue(
                                                          "@(negedge test_hmi_auto_brake_switch.uart_clk)");
    vlSelfRef.__VtrigSched_h1ff2e788__0.resume("@(negedge test_hmi_auto_brake_switch.pad_clk)");
    vlSelfRef.__VtrigSched_h482006c0__0.resume("@(negedge test_hmi_auto_brake_switch.uart_clk)");
    if ((0x0000000000000080ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_hmi_auto_brake_switch___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_hmi_auto_brake_switch___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_hmi_auto_brake_switch___024root___eval_phase__act(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_phase__act\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_hmi_auto_brake_switch___024root___eval_triggers_vec__act(vlSelf);
    Vtest_hmi_auto_brake_switch___024root___timing_ready(vlSelf);
    Vtest_hmi_auto_brake_switch___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_auto_brake_switch___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_hmi_auto_brake_switch___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_hmi_auto_brake_switch___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_hmi_auto_brake_switch___024root___timing_resume(vlSelf);
        Vtest_hmi_auto_brake_switch___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_hmi_auto_brake_switch___024root___eval_phase__inact(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_phase__inact\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 5, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_hmi_auto_brake_switch___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_hmi_auto_brake_switch___024root___eval_phase__nba(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_phase__nba\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_hmi_auto_brake_switch___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_hmi_auto_brake_switch___024root___eval_nba(vlSelf);
        Vtest_hmi_auto_brake_switch___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_hmi_auto_brake_switch___024root___eval(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_auto_brake_switch___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 5, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 5, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_hmi_auto_brake_switch___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_auto_brake_switch.v", 5, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_hmi_auto_brake_switch___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_hmi_auto_brake_switch___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_hmi_auto_brake_switch___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h1ff2e788__0(Vtest_hmi_auto_brake_switch___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h1ff2e788__0\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk)) 
                                   & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0)) 
                                  << 8U)));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__pad_clk__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__pad_clk;
    if ((0x0000000000000100ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h1ff2e788__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0(Vtest_hmi_auto_brake_switch___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root____VbeforeTrig_h482006c0__0\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)((((~ (IData)(vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk)) 
                                   & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0)) 
                                  << 9U)));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_auto_brake_switch__DOT__uart_clk__0 
        = vlSelfRef.test_hmi_auto_brake_switch__DOT__uart_clk;
    if ((0x0000000000000200ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h482006c0__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_hmi_auto_brake_switch___024root___eval_debug_assertions(Vtest_hmi_auto_brake_switch___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_auto_brake_switch___024root___eval_debug_assertions\n"); );
    Vtest_hmi_auto_brake_switch__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
