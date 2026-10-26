// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_smdt_transport.h for the primary calling header

#include "Vtest_smdt_transport__pch.h"

VlCoroutine Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__0(Vtest_smdt_transport___024root* vlSelf);
VlCoroutine Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__1(Vtest_smdt_transport___024root* vlSelf);

void Vtest_smdt_transport___024root___eval_initial(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_initial\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__0(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__0__addr;
    __Vtask_test_smdt_transport__DOT__host_write__0__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__0__data;
    __Vtask_test_smdt_transport__DOT__host_write__0__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__0__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__0__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__1__b;
    __Vtask_test_smdt_transport__DOT__byte_out__1__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__2__b;
    __Vtask_test_smdt_transport__DOT__byte_out__2__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__3__b;
    __Vtask_test_smdt_transport__DOT__byte_out__3__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__4__b;
    __Vtask_test_smdt_transport__DOT__byte_out__4__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__5__b;
    __Vtask_test_smdt_transport__DOT__byte_out__5__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__6__b;
    __Vtask_test_smdt_transport__DOT__byte_out__6__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__7__addr;
    __Vtask_test_smdt_transport__DOT__host_write__7__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__7__data;
    __Vtask_test_smdt_transport__DOT__host_write__7__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__7__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__7__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__8__b;
    __Vtask_test_smdt_transport__DOT__byte_out__8__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__9__b;
    __Vtask_test_smdt_transport__DOT__byte_out__9__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__10__b;
    __Vtask_test_smdt_transport__DOT__byte_out__10__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__11__b;
    __Vtask_test_smdt_transport__DOT__byte_out__11__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__12__b;
    __Vtask_test_smdt_transport__DOT__byte_out__12__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__13__b;
    __Vtask_test_smdt_transport__DOT__byte_out__13__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__14__addr;
    __Vtask_test_smdt_transport__DOT__host_write__14__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__14__data;
    __Vtask_test_smdt_transport__DOT__host_write__14__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__14__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__14__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__15__b;
    __Vtask_test_smdt_transport__DOT__byte_out__15__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__16__b;
    __Vtask_test_smdt_transport__DOT__byte_out__16__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__17__b;
    __Vtask_test_smdt_transport__DOT__byte_out__17__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__18__b;
    __Vtask_test_smdt_transport__DOT__byte_out__18__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__19__b;
    __Vtask_test_smdt_transport__DOT__byte_out__19__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__20__b;
    __Vtask_test_smdt_transport__DOT__byte_out__20__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__21__addr;
    __Vtask_test_smdt_transport__DOT__host_write__21__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__21__data;
    __Vtask_test_smdt_transport__DOT__host_write__21__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__21__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__21__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__22__b;
    __Vtask_test_smdt_transport__DOT__byte_out__22__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__23__b;
    __Vtask_test_smdt_transport__DOT__byte_out__23__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__24__b;
    __Vtask_test_smdt_transport__DOT__byte_out__24__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__25__b;
    __Vtask_test_smdt_transport__DOT__byte_out__25__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__26__b;
    __Vtask_test_smdt_transport__DOT__byte_out__26__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__27__b;
    __Vtask_test_smdt_transport__DOT__byte_out__27__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__28__addr;
    __Vtask_test_smdt_transport__DOT__host_write__28__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__28__data;
    __Vtask_test_smdt_transport__DOT__host_write__28__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__28__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__28__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__29__b;
    __Vtask_test_smdt_transport__DOT__byte_out__29__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__30__b;
    __Vtask_test_smdt_transport__DOT__byte_out__30__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__31__b;
    __Vtask_test_smdt_transport__DOT__byte_out__31__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__32__b;
    __Vtask_test_smdt_transport__DOT__byte_out__32__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__33__b;
    __Vtask_test_smdt_transport__DOT__byte_out__33__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__34__b;
    __Vtask_test_smdt_transport__DOT__byte_out__34__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__35__addr;
    __Vtask_test_smdt_transport__DOT__host_write__35__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__35__data;
    __Vtask_test_smdt_transport__DOT__host_write__35__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__35__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__35__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__36__b;
    __Vtask_test_smdt_transport__DOT__byte_out__36__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__37__b;
    __Vtask_test_smdt_transport__DOT__byte_out__37__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__38__b;
    __Vtask_test_smdt_transport__DOT__byte_out__38__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__39__b;
    __Vtask_test_smdt_transport__DOT__byte_out__39__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__40__b;
    __Vtask_test_smdt_transport__DOT__byte_out__40__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__41__b;
    __Vtask_test_smdt_transport__DOT__byte_out__41__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__42__addr;
    __Vtask_test_smdt_transport__DOT__host_write__42__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__42__data;
    __Vtask_test_smdt_transport__DOT__host_write__42__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__42__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__42__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__43__b;
    __Vtask_test_smdt_transport__DOT__byte_out__43__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__44__b;
    __Vtask_test_smdt_transport__DOT__byte_out__44__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__45__b;
    __Vtask_test_smdt_transport__DOT__byte_out__45__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__46__b;
    __Vtask_test_smdt_transport__DOT__byte_out__46__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__47__b;
    __Vtask_test_smdt_transport__DOT__byte_out__47__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__48__b;
    __Vtask_test_smdt_transport__DOT__byte_out__48__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__49__addr;
    __Vtask_test_smdt_transport__DOT__host_write__49__addr = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__host_write__49__data;
    __Vtask_test_smdt_transport__DOT__host_write__49__data = 0;
    CData/*0:0*/ __Vtask_test_smdt_transport__DOT__host_write__49__redundant_opcode;
    __Vtask_test_smdt_transport__DOT__host_write__49__redundant_opcode = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__50__b;
    __Vtask_test_smdt_transport__DOT__byte_out__50__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__51__b;
    __Vtask_test_smdt_transport__DOT__byte_out__51__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__52__b;
    __Vtask_test_smdt_transport__DOT__byte_out__52__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__53__b;
    __Vtask_test_smdt_transport__DOT__byte_out__53__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__54__b;
    __Vtask_test_smdt_transport__DOT__byte_out__54__b = 0;
    CData/*7:0*/ __Vtask_test_smdt_transport__DOT__byte_out__55__b;
    __Vtask_test_smdt_transport__DOT__byte_out__55__b = 0;
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         38);
    vlSelfRef.test_smdt_transport__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x000000000000c350ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         38);
    __Vtask_test_smdt_transport__DOT__host_write__0__redundant_opcode = 1U;
    __Vtask_test_smdt_transport__DOT__host_write__0__data = 1U;
    __Vtask_test_smdt_transport__DOT__host_write__0__addr = 4U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__1__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__1__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__1__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__0__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__2__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__2__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__2__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__3__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__3__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__3__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__4__b 
        = __Vtask_test_smdt_transport__DOT__host_write__0__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__4__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__4__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__5__b 
        = __Vtask_test_smdt_transport__DOT__host_write__0__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__5__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__5__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__0__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__6__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__6__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__6__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    if (VL_UNLIKELY((((4U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[0U]) 
                      | (1U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[4U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_smdt_transport.v:40: Assertion failed in %m: redundant opcode reproduction mismatch\n",3, 'M',vlSymsp->name(),"test_smdt_transport", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 40, "", false);
    }
    VL_WRITEF_NX("REPRO: host reg4=1 becomes reg0=4, GO active-low bit becomes 0\n",0);
    __Vtask_test_smdt_transport__DOT__host_write__7__redundant_opcode = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__7__data = 1U;
    __Vtask_test_smdt_transport__DOT__host_write__7__addr = 0U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__8__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__8__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__8__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__7__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__9__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__9__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__9__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__10__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__10__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__10__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__11__b 
        = __Vtask_test_smdt_transport__DOT__host_write__7__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__11__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__11__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__12__b 
        = __Vtask_test_smdt_transport__DOT__host_write__7__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__12__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__12__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__7__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__13__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__13__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__13__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    if (VL_UNLIKELY(((1U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[0U])))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_smdt_transport.v:43: Assertion failed in %m: SDK release GO failed\n",3, 'M',vlSymsp->name(),"test_smdt_transport", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 43, "", false);
    }
    __Vtask_test_smdt_transport__DOT__host_write__14__redundant_opcode = 1U;
    __Vtask_test_smdt_transport__DOT__host_write__14__data = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__14__addr = 2U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__15__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__15__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__15__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__14__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__16__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__16__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__16__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__17__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__17__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__17__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__18__b 
        = __Vtask_test_smdt_transport__DOT__host_write__14__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__18__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__18__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__19__b 
        = __Vtask_test_smdt_transport__DOT__host_write__14__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__19__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__19__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__14__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__20__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__20__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__20__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    if (VL_UNLIKELY((((2U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[0U]) 
                      | (1U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_smdt_transport.v:45: Assertion failed in %m: redundant LEFT reproduction mismatch\n",3, 'M',vlSymsp->name(),"test_smdt_transport", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 45, "", false);
    }
    VL_WRITEF_NX("REPRO: host LEFT=0 becomes reg0=2; LEFT remains released\n",0);
    __Vtask_test_smdt_transport__DOT__host_write__21__redundant_opcode = 1U;
    __Vtask_test_smdt_transport__DOT__host_write__21__data = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__21__addr = 3U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__22__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__22__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__22__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__21__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__23__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__23__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__23__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__24__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__24__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__24__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__25__b 
        = __Vtask_test_smdt_transport__DOT__host_write__21__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__25__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__25__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__26__b 
        = __Vtask_test_smdt_transport__DOT__host_write__21__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__26__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__26__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__21__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__27__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__27__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__27__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    if (VL_UNLIKELY((((3U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[0U]) 
                      | (1U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_smdt_transport.v:48: Assertion failed in %m: redundant RIGHT reproduction mismatch\n",3, 'M',vlSymsp->name(),"test_smdt_transport", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 48, "", false);
    }
    VL_WRITEF_NX("REPRO: host RIGHT=0 becomes reg0=3; GO released and RIGHT remains released\n",0);
    __Vtask_test_smdt_transport__DOT__host_write__28__redundant_opcode = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__28__data = 1U;
    __Vtask_test_smdt_transport__DOT__host_write__28__addr = 0U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__29__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__29__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__29__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__28__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__30__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__30__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__30__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__31__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__31__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__31__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__32__b 
        = __Vtask_test_smdt_transport__DOT__host_write__28__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__32__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__32__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__33__b 
        = __Vtask_test_smdt_transport__DOT__host_write__28__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__33__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__33__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__28__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__34__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__34__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__34__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    __Vtask_test_smdt_transport__DOT__host_write__35__redundant_opcode = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__35__data = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__35__addr = 2U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__36__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__36__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__36__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__35__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__37__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__37__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__37__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__38__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__38__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__38__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__39__b 
        = __Vtask_test_smdt_transport__DOT__host_write__35__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__39__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__39__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__40__b 
        = __Vtask_test_smdt_transport__DOT__host_write__35__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__40__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__40__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__35__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__41__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__41__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__41__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    if (VL_UNLIKELY((((1U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[0U]) 
                      | (0U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[2U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_smdt_transport.v:51: Assertion failed in %m: SDK LEFT failed\n",3, 'M',vlSymsp->name(),"test_smdt_transport", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 51, "", false);
    }
    __Vtask_test_smdt_transport__DOT__host_write__42__redundant_opcode = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__42__data = 1U;
    __Vtask_test_smdt_transport__DOT__host_write__42__addr = 2U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__43__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__43__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__43__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__42__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__44__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__44__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__44__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__45__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__45__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__45__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__46__b 
        = __Vtask_test_smdt_transport__DOT__host_write__42__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__46__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__46__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__47__b 
        = __Vtask_test_smdt_transport__DOT__host_write__42__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__47__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__47__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__42__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__48__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__48__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__48__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    __Vtask_test_smdt_transport__DOT__host_write__49__redundant_opcode = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__49__data = 0U;
    __Vtask_test_smdt_transport__DOT__host_write__49__addr = 3U;
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         29);
    __Vtask_test_smdt_transport__DOT__byte_out__50__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__50__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__50__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__49__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__51__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__51__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__51__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    __Vtask_test_smdt_transport__DOT__byte_out__52__b = 0U;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__52__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__52__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__53__b 
        = __Vtask_test_smdt_transport__DOT__host_write__49__addr;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__53__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__53__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    __Vtask_test_smdt_transport__DOT__byte_out__54__b 
        = __Vtask_test_smdt_transport__DOT__host_write__49__data;
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__54__b) 
                          >> 4U));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         23);
    vlSelfRef.test_smdt_transport__DOT__master_data 
        = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__54__b));
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         24);
    if (__Vtask_test_smdt_transport__DOT__host_write__49__redundant_opcode) {
        __Vtask_test_smdt_transport__DOT__byte_out__55__b = 0U;
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & ((IData)(__Vtask_test_smdt_transport__DOT__byte_out__55__b) 
                              >> 4U));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             23);
        vlSelfRef.test_smdt_transport__DOT__master_data 
            = (0x0000000fU & (IData)(__Vtask_test_smdt_transport__DOT__byte_out__55__b));
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
        vlSelfRef.test_smdt_transport__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             24);
    }
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    vlSelfRef.test_smdt_transport__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                         34);
    if (VL_UNLIKELY(((((1U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[0U]) 
                       | (1U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[2U])) 
                      | (0U != vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[3U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_smdt_transport.v:53: Assertion failed in %m: SDK RIGHT failed\n",3, 'M',vlSymsp->name(),"test_smdt_transport", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 53, "", false);
    }
    VL_WRITEF_NX("PASS: prefixed-controller model reproduces unintended GO and absent steering; SDK payload selects correct registers\n",0);
    VL_FINISH_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 55, "");
    co_return;
}

VlCoroutine Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__1(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x00000000000004e2ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 
                                             6);
        vlSelfRef.test_smdt_transport__DOT__wr_clk 
            = (1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__wr_clk)));
    }
    co_return;
}

void Vtest_smdt_transport___024root___eval_triggers_vec__act(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_triggers_vec__act\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                       << 5U) 
                                                      | (((~ (IData)(vlSelfRef.test_smdt_transport__DOT__wr_clk)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__wr_clk__0)) 
                                                         << 4U)) 
                                                     | (((((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_clk)) 
                                                           & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__spi_clk__0)) 
                                                          << 3U) 
                                                         | (((IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs) 
                                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__spi_cs__0))) 
                                                            << 2U)) 
                                                        | ((((IData)(vlSelfRef.test_smdt_transport__DOT__spi_clk) 
                                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__spi_clk__0))) 
                                                            << 1U) 
                                                           | ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
                                                              & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__rst_n__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__rst_n__0 
        = vlSelfRef.test_smdt_transport__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__spi_clk__0 
        = vlSelfRef.test_smdt_transport__DOT__spi_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__spi_cs__0 
        = vlSelfRef.test_smdt_transport__DOT__spi_cs;
    vlSelfRef.__Vtrigprevexpr___TOP__test_smdt_transport__DOT__wr_clk__0 
        = vlSelfRef.test_smdt_transport__DOT__wr_clk;
}

bool Vtest_smdt_transport___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___trigger_anySet__act\n"); );
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

void Vtest_smdt_transport___024root___act_sequent__TOP__0(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___act_sequent__TOP__0\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (0x0000000fU 
                                                & (- (IData)(
                                                             ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                                                              & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started)))));
    vlSelfRef.test_smdt_transport__DOT__spi_data = 
        ((IData)(vlSelfRef.test_smdt_transport__DOT__master_data) 
         | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
             | (- (IData)(((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                           & (2U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg)))))) 
            & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble) 
               & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))));
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header 
        = ((0x00fffff0U & (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header 
                           << 4U)) | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data));
}

void Vtest_smdt_transport___024root___eval_act(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_act\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            (0x0000000fU & (- (IData)(((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                                       & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started)))));
        vlSelfRef.test_smdt_transport__DOT__spi_data 
            = ((IData)(vlSelfRef.test_smdt_transport__DOT__master_data) 
               | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                   | (- (IData)(((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                                 & (2U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg)))))) 
                  & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble) 
                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))));
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data));
    }
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__0(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__0\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag 
        = vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag;
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__1(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__1\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count 
        = vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count;
    vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock 
        = vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_lock;
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__2(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__2\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg 
        = vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg;
    if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg = 0U;
    } else if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        if ((2U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg))) {
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg 
                = (1U & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg)
                          ? (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg))
                          : (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg))));
        }
    }
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__3(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__3\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started;
    __Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started = 0;
    // Body
    __Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started 
        = vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started;
    if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        __Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started = 0U;
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble = 0x0fU;
    } else if (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_ready) {
        if (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started) {
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble 
                = (0x0000000fU & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data));
        } else {
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble 
                = (0x0000000fU & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data) 
                                  >> 4U));
            __Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started = 1U;
        }
    }
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started 
        = __Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started;
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__4(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__4\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*2:0*/ __Vdly__test_smdt_transport__DOT__regs__DOT__state;
    __Vdly__test_smdt_transport__DOT__regs__DOT__state = 0;
    CData/*1:0*/ __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt;
    __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 0;
    CData/*7:0*/ __VdlyVal__test_smdt_transport__DOT__regs__DOT__mem__v0;
    __VdlyVal__test_smdt_transport__DOT__regs__DOT__mem__v0 = 0;
    CData/*2:0*/ __VdlyDim0__test_smdt_transport__DOT__regs__DOT__mem__v0;
    __VdlyDim0__test_smdt_transport__DOT__regs__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v0;
    __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v1;
    __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v1 = 0;
    // Body
    __Vdly__test_smdt_transport__DOT__regs__DOT__state 
        = vlSelfRef.test_smdt_transport__DOT__regs__DOT__state;
    __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt 
        = vlSelfRef.test_smdt_transport__DOT__regs__DOT__byte_cnt;
    __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v0 = 0U;
    __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v1 = 0U;
    if (vlSelfRef.test_smdt_transport__DOT__rst_n) {
        if (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_wr_en) 
             & (5U > (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_addr)))) {
            vlSelfRef.test_smdt_transport__DOT__regs__DOT____Vlvbound_h11419250__0 
                = vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg;
            if ((4U >= (7U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_addr)))) {
                __VdlyVal__test_smdt_transport__DOT__regs__DOT__mem__v0 
                    = vlSelfRef.test_smdt_transport__DOT__regs__DOT____Vlvbound_h11419250__0;
                __VdlyDim0__test_smdt_transport__DOT__regs__DOT__mem__v0 
                    = (7U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_addr));
                __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v0 = 1U;
            }
        }
        if (vlSelfRef.test_smdt_transport__DOT__regs__DOT__spi_cs_reg) {
            __Vdly__test_smdt_transport__DOT__regs__DOT__state = 0U;
            vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_wr_en = 0U;
            __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 0U;
            vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg = 0xffU;
        } else {
            if ((4U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__state))) {
                if ((2U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__state))) {
                    if ((1U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__state))) {
                        __Vdly__test_smdt_transport__DOT__regs__DOT__state = 0U;
                    } else if (((~ (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__tx_done_d0)) 
                                & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__tx_done_d1))) {
                        __Vdly__test_smdt_transport__DOT__regs__DOT__state = 0U;
                    }
                } else if ((1U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__state))) {
                    if ((2U == (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__byte_cnt))) {
                        __Vdly__test_smdt_transport__DOT__regs__DOT__state = 6U;
                    }
                } else if (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_pose) 
                            & (1U == (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__byte_cnt)))) {
                    vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_addr 
                        = (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__read_addr_high) 
                            << 8U) | (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg));
                    __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 2U;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__state = 5U;
                }
            } else if ((2U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__state))) {
                if ((1U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__state))) {
                    vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_wr_en = 0U;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__state = 0U;
                } else if (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_pose) 
                            & (2U == (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__byte_cnt)))) {
                    vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_wr_en = 1U;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 3U;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__state = 3U;
                }
            } else if ((1U & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__state))) {
                if (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_pose) 
                     & (1U == (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__byte_cnt)))) {
                    vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_addr 
                        = (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__addr_high) 
                            << 8U) | (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg));
                    __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 2U;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__state = 2U;
                }
            } else {
                vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_wr_en = 0U;
                __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 0U;
                if (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_pose) 
                     & (1U == (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__cmd_reg)))) {
                    vlSelfRef.test_smdt_transport__DOT__regs__DOT__addr_high 
                        = vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 1U;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__state = 1U;
                } else if (((IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_pose) 
                            & (2U == (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__cmd_reg)))) {
                    vlSelfRef.test_smdt_transport__DOT__regs__DOT__read_addr_high 
                        = vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 1U;
                    __Vdly__test_smdt_transport__DOT__regs__DOT__state = 4U;
                }
            }
            vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg 
                = vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data;
        }
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__cmd_reg 
            = vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg;
    } else {
        __VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v1 = 1U;
        __Vdly__test_smdt_transport__DOT__regs__DOT__state = 0U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_addr = 0U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__ram_wr_en = 0U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__addr_high = 0U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__read_addr_high = 0U;
        __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt = 0U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__wr_data_reg = 0U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__cmd_reg = 0U;
    }
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_valid_d1 
        = ((IData)(vlSelfRef.test_smdt_transport__DOT__rst_n) 
           && (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_valid_d0));
    if (__VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v0) {
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[__VdlyDim0__test_smdt_transport__DOT__regs__DOT__mem__v0] 
            = __VdlyVal__test_smdt_transport__DOT__regs__DOT__mem__v0;
    }
    if (__VdlySet__test_smdt_transport__DOT__regs__DOT__mem__v1) {
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[0U] = 1U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[1U] = 1U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[2U] = 1U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[3U] = 1U;
        vlSelfRef.test_smdt_transport__DOT__regs__DOT__mem[4U] = 1U;
    }
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_valid_d0 
        = ((IData)(vlSelfRef.test_smdt_transport__DOT__rst_n) 
           && (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_valid));
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__state 
        = __Vdly__test_smdt_transport__DOT__regs__DOT__state;
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__byte_cnt 
        = __Vdly__test_smdt_transport__DOT__regs__DOT__byte_cnt;
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_pose 
        = ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_valid_d1)) 
           & (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__rx_valid_d0));
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__tx_done_d1 
        = ((IData)(vlSelfRef.test_smdt_transport__DOT__rst_n) 
           && (IData)(vlSelfRef.test_smdt_transport__DOT__regs__DOT__tx_done_d0));
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__tx_done_d0 
        = ((IData)(vlSelfRef.test_smdt_transport__DOT__rst_n) 
           && (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg));
    vlSelfRef.test_smdt_transport__DOT__regs__DOT__spi_cs_reg 
        = ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n))) 
           || (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs));
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__5(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__5\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header = 0U;
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count = 0U;
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_ready = 0U;
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data = 0xffU;
    } else if ((6U > (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count))) {
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count 
            = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count)));
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header 
            = vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header;
        if ((IData)(((5U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count)) 
                     & (0x00810000U == (0x00ff0000U 
                                        & vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header))))) {
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_ready = 1U;
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data 
                = ((5U == (0x0000ffffU & vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header))
                    ? ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_sync)
                        ? 0xa1U : 0xa0U) : 0xffU);
        }
    }
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count 
        = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count;
}

void Vtest_smdt_transport___024root___nba_comb__TOP__0(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_comb__TOP__0\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = (0x0000000fU 
                                                & (- (IData)(
                                                             ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                                                              & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started)))));
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__6(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__6\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_valid = 0U;
    } else if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_valid 
            = ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag) 
               & (1U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg)));
    }
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_sync 
        = ((IData)(vlSelfRef.test_smdt_transport__DOT__rst_n) 
           && (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_meta));
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_meta = 0U;
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__7(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__7\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg 
        = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg;
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__8(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__8\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock = 0U;
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg = 0U;
    } else if (((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag))) {
        if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_lock)))) {
            if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                           | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data)))) {
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg = 2U;
            } else if ((0U == ((0x000000f0U & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data)))) {
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg = 1U;
            }
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock = 1U;
        }
    }
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_lock 
        = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock;
}

void Vtest_smdt_transport___024root___nba_sequent__TOP__9(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_sequent__TOP__9\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag = 0U;
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data = 0xffU;
    } else if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
        if (vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag) {
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag)));
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data 
                = ((0xf0U & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data));
        } else {
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag)));
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data 
                = ((0x0fU & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                   | ((IData)(vlSelfRef.test_smdt_transport__DOT__spi_data) 
                      << 4U));
        }
    }
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag 
        = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag;
}

void Vtest_smdt_transport___024root___nba_comb__TOP__1(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___nba_comb__TOP__1\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_smdt_transport__DOT__spi_data = 
        ((IData)(vlSelfRef.test_smdt_transport__DOT__master_data) 
         | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
             | (- (IData)(((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                           & (2U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg)))))) 
            & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble) 
               & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))));
    vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header 
        = ((0x00fffff0U & (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header 
                           << 4U)) | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data));
}

void Vtest_smdt_transport___024root___eval_nba(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_nba\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vinline__nba_sequent__TOP__3___Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started;
    __Vinline__nba_sequent__TOP__3___Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started = 0;
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag 
            = vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag;
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count 
            = vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count;
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock 
            = vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_lock;
    }
    if ((9ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg 
            = vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg;
        if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg = 0U;
        } else if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            if ((2U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg))) {
                vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg 
                    = (1U & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg)
                              ? (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg))
                              : (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg))));
            }
        }
    }
    if ((0x000000000000000dULL & vlSelfRef.__VnbaTriggered[0U])) {
        __Vinline__nba_sequent__TOP__3___Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started 
            = vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started;
        if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            __Vinline__nba_sequent__TOP__3___Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started = 0U;
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble = 0x0fU;
        } else if (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_ready) {
            if (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started) {
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble 
                    = (0x0000000fU & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data));
            } else {
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble 
                    = (0x0000000fU & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data) 
                                      >> 4U));
                __Vinline__nba_sequent__TOP__3___Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started = 1U;
            }
        }
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started 
            = __Vinline__nba_sequent__TOP__3___Vdly__test_smdt_transport__DOT__slave__DOT__status_tx_started;
    }
    if ((0x0000000000000011ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_smdt_transport___024root___nba_sequent__TOP__4(vlSelf);
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header = 0U;
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count = 0U;
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_ready = 0U;
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data = 0xffU;
        } else if ((6U > (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count))) {
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count)));
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header 
                = vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header;
            if ((IData)(((5U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count)) 
                         & (0x00810000U == (0x00ff0000U 
                                            & vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header))))) {
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_ready = 1U;
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_read_data 
                    = ((5U == (0x0000ffffU & vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header))
                        ? ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_sync)
                            ? 0xa1U : 0xa0U) : 0xffU);
            }
        }
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_nibble_count 
            = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__status_nibble_count;
    }
    if ((0x000000000000002dULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            (0x0000000fU & (- (IData)(((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                                       & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_started)))));
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_valid = 0U;
        } else if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_valid 
                = ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag) 
                   & (1U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg)));
        }
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_sync 
            = ((IData)(vlSelfRef.test_smdt_transport__DOT__rst_n) 
               && (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_meta));
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__auto_enable_meta = 0U;
    }
    if ((9ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__tx_valid_reg 
            = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__tx_valid_reg;
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock = 0U;
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg = 0U;
        } else if (((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                    & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag))) {
            if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_lock)))) {
                if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                               | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data)))) {
                    vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg = 2U;
                } else if ((0U == ((0x000000f0U & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data)))) {
                    vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg = 1U;
                }
                vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock = 1U;
            }
        }
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_lock 
            = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__cmd_lock;
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_smdt_transport__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag = 0U;
            vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data = 0xffU;
        } else if ((1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)))) {
            if (vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag) {
                vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag 
                    = (1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag)));
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data 
                    = ((0xf0U & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                       | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data));
            } else {
                vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag 
                    = (1U & (~ (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag)));
                vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data 
                    = ((0x0fU & (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_data)) 
                       | ((IData)(vlSelfRef.test_smdt_transport__DOT__spi_data) 
                          << 4U));
            }
        }
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__recv_flag 
            = vlSelfRef.__Vdly__test_smdt_transport__DOT__slave__DOT__recv_flag;
    }
    if ((0x000000000000002fULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_smdt_transport__DOT__spi_data 
            = ((IData)(vlSelfRef.test_smdt_transport__DOT__master_data) 
               | (((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1) 
                   | (- (IData)(((~ (IData)(vlSelfRef.test_smdt_transport__DOT__spi_cs)) 
                                 & (2U == (IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__cmd_reg)))))) 
                  & ((IData)(vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_tx_nibble) 
                     & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))));
        vlSelfRef.test_smdt_transport__DOT__slave__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_smdt_transport__DOT__slave__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_smdt_transport__DOT__spi_data));
    }
}

void Vtest_smdt_transport___024root___timing_resume(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___timing_resume\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_smdt_transport___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_smdt_transport___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_smdt_transport___024root___eval_phase__act(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_phase__act\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_smdt_transport___024root___eval_triggers_vec__act(vlSelf);
    Vtest_smdt_transport___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_smdt_transport___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_smdt_transport___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_smdt_transport___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_smdt_transport___024root___timing_resume(vlSelf);
        Vtest_smdt_transport___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_smdt_transport___024root___eval_phase__inact(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_phase__inact\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 5, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_smdt_transport___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_smdt_transport___024root___eval_phase__nba(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_phase__nba\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_smdt_transport___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_smdt_transport___024root___eval_nba(vlSelf);
        Vtest_smdt_transport___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_smdt_transport___024root___eval(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_smdt_transport___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 5, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 5, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_smdt_transport___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_smdt_transport.v", 5, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_smdt_transport___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_smdt_transport___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_smdt_transport___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

#ifdef VL_DEBUG
void Vtest_smdt_transport___024root___eval_debug_assertions(Vtest_smdt_transport___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_smdt_transport___024root___eval_debug_assertions\n"); );
    Vtest_smdt_transport__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
