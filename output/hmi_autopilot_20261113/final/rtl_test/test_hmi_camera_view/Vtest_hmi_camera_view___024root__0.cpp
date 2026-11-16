// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_hmi_camera_view.h for the primary calling header

#include "Vtest_hmi_camera_view__pch.h"

VlCoroutine Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__0(Vtest_hmi_camera_view___024root* vlSelf);
VlCoroutine Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__1(Vtest_hmi_camera_view___024root* vlSelf);

void Vtest_hmi_camera_view___024root___eval_initial(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_initial\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

void Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(Vtest_hmi_camera_view___024root* vlSelf, const char* __VeventDescription);
void Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(Vtest_hmi_camera_view___024root* vlSelf, const char* __VeventDescription);

VlCoroutine Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__0(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ test_hmi_camera_view__DOT__unnamedblk1_1__DOT____Vrepeat0;
    test_hmi_camera_view__DOT__unnamedblk1_1__DOT____Vrepeat0 = 0;
    IData/*31:0*/ test_hmi_camera_view__DOT__unnamedblk2__DOT__i;
    test_hmi_camera_view__DOT__unnamedblk2__DOT__i = 0;
    IData/*31:0*/ test_hmi_camera_view__DOT__unnamedblk1_2__DOT____Vrepeat1;
    test_hmi_camera_view__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0;
    IData/*31:0*/ test_hmi_camera_view__DOT__unnamedblk1_3__DOT____Vrepeat2;
    test_hmi_camera_view__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__expect_view__1__expected;
    __Vtask_test_hmi_camera_view__DOT__expect_view__1__expected = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__2__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__2__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__3__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__3__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__4__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__4__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__5__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__5__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__6__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__6__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__expect_view__8__expected;
    __Vtask_test_hmi_camera_view__DOT__expect_view__8__expected = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__expect_view__10__expected;
    __Vtask_test_hmi_camera_view__DOT__expect_view__10__expected = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__11__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__11__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__12__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__12__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__13__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__13__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__14__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__14__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__15__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__15__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__expect_view__17__expected;
    __Vtask_test_hmi_camera_view__DOT__expect_view__17__expected = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__18__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__18__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__19__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__19__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__20__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__20__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__21__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__21__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__22__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__22__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_hmi;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_hmi = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_view;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_view = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__24____Vlvbound_h19eede4c__0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24____Vlvbound_h19eede4c__0 = 0;
    IData/*31:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__25__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__25__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__26__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__26__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__27__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__27__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__28__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__28__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__29__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__29__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__30__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__30__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__31__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__31__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__32__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__32__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__33__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__33__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_hmi;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_hmi = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_view;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_view = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__35____Vlvbound_h19eede4c__0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35____Vlvbound_h19eede4c__0 = 0;
    IData/*31:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__36__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__36__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__37__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__37__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__38__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__38__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__39__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__39__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__40__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__40__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__41__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__41__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__42__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__42__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__43__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__43__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__44__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__44__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_hmi;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_hmi = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_view;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_view = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__46____Vlvbound_h19eede4c__0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46____Vlvbound_h19eede4c__0 = 0;
    IData/*31:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__47__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__47__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__48__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__48__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__49__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__49__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__50__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__50__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__51__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__51__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__52__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__52__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__53__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__53__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__54__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__54__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__55__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__55__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_hmi;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_hmi = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_view;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_view = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__57____Vlvbound_h19eede4c__0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57____Vlvbound_h19eede4c__0 = 0;
    IData/*31:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__58__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__58__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__59__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__59__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__60__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__60__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__61__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__61__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__62__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__62__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__63__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__63__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__64__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__64__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__65__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__65__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__66__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__66__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__expect_view__69__expected;
    __Vtask_test_hmi_camera_view__DOT__expect_view__69__expected = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__70__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__70__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__71__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__71__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__72__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__72__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__73__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__73__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__74__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__74__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_key__76__value;
    __Vtask_test_hmi_camera_view__DOT__send_key__76__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__77__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__77__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__78__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__78__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__79__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__79__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__send_byte__80__value;
    __Vtask_test_hmi_camera_view__DOT__send_byte__80__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_hmi;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_hmi = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_view;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_view = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__82____Vlvbound_h19eede4c__0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82____Vlvbound_h19eede4c__0 = 0;
    IData/*31:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__83__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__83__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__84__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__84__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__85__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__85__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__86__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__86__value = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_hmi;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_hmi = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_view;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_view = 0;
    CData/*7:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__88____Vlvbound_h19eede4c__0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88____Vlvbound_h19eede4c__0 = 0;
    IData/*31:0*/ __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__89__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__89__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__90__value;
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__90__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__91__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__91__value = 0;
    CData/*3:0*/ __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__92__value;
    __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__92__value = 0;
    // Body
    test_hmi_camera_view__DOT__unnamedblk1_1__DOT____Vrepeat0 = 3U;
    while (VL_LTS_III(32, 0U, test_hmi_camera_view__DOT__unnamedblk1_1__DOT____Vrepeat0)) {
        Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                    "@(posedge test_hmi_camera_view.clk)");
        co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_hmi_camera_view.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                             33);
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             34);
        test_hmi_camera_view__DOT__unnamedblk1_1__DOT____Vrepeat0 
            = (test_hmi_camera_view__DOT__unnamedblk1_1__DOT____Vrepeat0 
               - (IData)(1U));
    }
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         113);
    vlSelfRef.test_hmi_camera_view__DOT__rst_n = 1U;
    __Vtask_test_hmi_camera_view__DOT__expect_view__1__expected = 9U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code) 
                      != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_view__1__expected))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:60: Assertion failed in %m: FAIL view %h, expected %h at %0t\n",6, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_view", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',4,(IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code)
                     , '#',4,__Vtask_test_hmi_camera_view__DOT__expect_view__1__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 60, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__2__value = 0x0cU;
    __Vtask_test_hmi_camera_view__DOT__send_byte__3__value = 0x30U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__3__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__4__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__4__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__5__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__5__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__6__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__2__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__6__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    test_hmi_camera_view__DOT__unnamedblk2__DOT__i = 0U;
    while (VL_GTS_III(32, 0x00000100U, test_hmi_camera_view__DOT__unnamedblk2__DOT__i)) {
        __Vtask_test_hmi_camera_view__DOT__expect_view__8__expected = 2U;
        if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code) 
                          != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_view__8__expected))))) {
            VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:60: Assertion failed in %m: FAIL view %h, expected %h at %0t\n",6, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_view", 'T',-9
                         , '#',64,VL_TIME_UNITED_Q(1000)
                         , '#',4,(IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code)
                         , '#',4,__Vtask_test_hmi_camera_view__DOT__expect_view__8__expected
                         , '#',64,VL_TIME_UNITED_Q(1000));
            VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 60, "", false);
        }
        Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                    "@(posedge test_hmi_camera_view.clk)");
        co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_hmi_camera_view.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                             33);
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             34);
        test_hmi_camera_view__DOT__unnamedblk2__DOT__i 
            = ((IData)(1U) + test_hmi_camera_view__DOT__unnamedblk2__DOT__i);
    }
    __Vtask_test_hmi_camera_view__DOT__expect_view__10__expected = 9U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code) 
                      != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_view__10__expected))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:60: Assertion failed in %m: FAIL view %h, expected %h at %0t\n",6, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_view", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',4,(IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code)
                     , '#',4,__Vtask_test_hmi_camera_view__DOT__expect_view__10__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 60, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__11__value = 0x0cU;
    __Vtask_test_hmi_camera_view__DOT__send_byte__12__value = 0x30U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__12__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__13__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__13__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__14__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__14__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__15__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__11__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__15__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    __Vtask_test_hmi_camera_view__DOT__expect_view__17__expected = 9U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code) 
                      != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_view__17__expected))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:60: Assertion failed in %m: FAIL view %h, expected %h at %0t\n",6, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_view", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',4,(IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code)
                     , '#',4,__Vtask_test_hmi_camera_view__DOT__expect_view__17__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 60, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__18__value = 0x0dU;
    __Vtask_test_hmi_camera_view__DOT__send_byte__19__value = 0x30U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__19__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__20__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__20__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__21__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__21__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__22__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__18__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__22__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_view = 0x33U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_hmi = 0xddU;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24____Vlvbound_h19eede4c__0 = 0;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         88);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__25__value = 8U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__25__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__26__value = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__26__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i = 0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000000fU, __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i)) {
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__27__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__27__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__27__value;
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__28__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__28__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__28__value;
        __Vtask_test_hmi_camera_view__DOT__expect_spi__24____Vlvbound_h19eede4c__0 
            = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi) 
                << 4U) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo));
        if (VL_LIKELY(((0x0eU >= (0x0000000fU & __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i))))) {
            vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[(0x0000000fU 
                                                                              & __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i)] 
                = __Vtask_test_hmi_camera_view__DOT__expect_spi__24____Vlvbound_h19eede4c__0;
        }
        __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i 
            = ((IData)(1U) + __Vtask_test_hmi_camera_view__DOT__expect_spi__24__unnamedblk1__DOT__i);
    }
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         98);
    if (VL_UNLIKELY((((((((vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U] 
                           != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_hmi)) 
                          | (0xa7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[1U])) 
                         | (0xd7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[4U])) 
                        | (0x34U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[5U])) 
                       | (0x12U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[6U])) 
                      | (vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U] 
                         != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_view)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:102: Assertion failed in %m: FAIL FSPI raw %h, view %h, expected %h/%h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U]
                     , '#',8,__Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_hmi
                     , '#',8,(IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__24__expected_view));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 102, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__29__value = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__30__value = 0x30U;
    if (VL_UNLIKELY(((0xb609c478U != (((((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]) 
                                         << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]) 
                                       << 0x00000010U) 
                                      | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]) 
                                          << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U])))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:105: Assertion failed in %m: FAIL wheel tail %h %h %h %h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 105, "", false);
    }
    if (VL_UNLIKELY(((0xbd3aU != (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]) 
                                   << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:107: Assertion failed in %m: FAIL Autopilot status/CRC %h %h\n",5, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 107, "", false);
    }
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__30__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__31__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__31__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__32__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__32__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__33__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__29__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__33__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_view = 0x33U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_hmi = 0U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35____Vlvbound_h19eede4c__0 = 0;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         88);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__36__value = 8U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__36__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__37__value = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__37__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i = 0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000000fU, __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i)) {
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__38__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__38__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__38__value;
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__39__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__39__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__39__value;
        __Vtask_test_hmi_camera_view__DOT__expect_spi__35____Vlvbound_h19eede4c__0 
            = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi) 
                << 4U) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo));
        if (VL_LIKELY(((0x0eU >= (0x0000000fU & __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i))))) {
            vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[(0x0000000fU 
                                                                              & __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i)] 
                = __Vtask_test_hmi_camera_view__DOT__expect_spi__35____Vlvbound_h19eede4c__0;
        }
        __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i 
            = ((IData)(1U) + __Vtask_test_hmi_camera_view__DOT__expect_spi__35__unnamedblk1__DOT__i);
    }
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         98);
    if (VL_UNLIKELY((((((((vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U] 
                           != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_hmi)) 
                          | (0xa7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[1U])) 
                         | (0xd7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[4U])) 
                        | (0x34U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[5U])) 
                       | (0x12U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[6U])) 
                      | (vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U] 
                         != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_view)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:102: Assertion failed in %m: FAIL FSPI raw %h, view %h, expected %h/%h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U]
                     , '#',8,__Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_hmi
                     , '#',8,(IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__35__expected_view));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 102, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__40__value = 0x0cU;
    __Vtask_test_hmi_camera_view__DOT__send_byte__41__value = 0x30U;
    if (VL_UNLIKELY(((0xb609c478U != (((((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]) 
                                         << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]) 
                                       << 0x00000010U) 
                                      | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]) 
                                          << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U])))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:105: Assertion failed in %m: FAIL wheel tail %h %h %h %h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 105, "", false);
    }
    if (VL_UNLIKELY(((0xbd3aU != (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]) 
                                   << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:107: Assertion failed in %m: FAIL Autopilot status/CRC %h %h\n",5, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 107, "", false);
    }
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__41__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__42__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__42__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__43__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__43__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__44__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__40__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__44__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_view = 0x22U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_hmi = 0xccU;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46____Vlvbound_h19eede4c__0 = 0;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         88);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__47__value = 8U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__47__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__48__value = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__48__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i = 0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000000fU, __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i)) {
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__49__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__49__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__49__value;
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__50__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__50__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__50__value;
        __Vtask_test_hmi_camera_view__DOT__expect_spi__46____Vlvbound_h19eede4c__0 
            = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi) 
                << 4U) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo));
        if (VL_LIKELY(((0x0eU >= (0x0000000fU & __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i))))) {
            vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[(0x0000000fU 
                                                                              & __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i)] 
                = __Vtask_test_hmi_camera_view__DOT__expect_spi__46____Vlvbound_h19eede4c__0;
        }
        __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i 
            = ((IData)(1U) + __Vtask_test_hmi_camera_view__DOT__expect_spi__46__unnamedblk1__DOT__i);
    }
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         98);
    if (VL_UNLIKELY((((((((vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U] 
                           != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_hmi)) 
                          | (0xa7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[1U])) 
                         | (0xd7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[4U])) 
                        | (0x34U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[5U])) 
                       | (0x12U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[6U])) 
                      | (vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U] 
                         != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_view)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:102: Assertion failed in %m: FAIL FSPI raw %h, view %h, expected %h/%h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U]
                     , '#',8,__Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_hmi
                     , '#',8,(IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__46__expected_view));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 102, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__51__value = 9U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__52__value = 0x30U;
    if (VL_UNLIKELY(((0xb609c478U != (((((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]) 
                                         << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]) 
                                       << 0x00000010U) 
                                      | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]) 
                                          << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U])))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:105: Assertion failed in %m: FAIL wheel tail %h %h %h %h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 105, "", false);
    }
    if (VL_UNLIKELY(((0xbd3aU != (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]) 
                                   << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:107: Assertion failed in %m: FAIL Autopilot status/CRC %h %h\n",5, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 107, "", false);
    }
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__52__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__53__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__53__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__54__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__54__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__55__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__51__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__55__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_view = 0x99U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_hmi = 0x99U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57____Vlvbound_h19eede4c__0 = 0;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         88);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__58__value = 8U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__58__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__59__value = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__59__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i = 0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000000fU, __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i)) {
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__60__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__60__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__60__value;
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__61__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__61__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__61__value;
        __Vtask_test_hmi_camera_view__DOT__expect_spi__57____Vlvbound_h19eede4c__0 
            = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi) 
                << 4U) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo));
        if (VL_LIKELY(((0x0eU >= (0x0000000fU & __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i))))) {
            vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[(0x0000000fU 
                                                                              & __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i)] 
                = __Vtask_test_hmi_camera_view__DOT__expect_spi__57____Vlvbound_h19eede4c__0;
        }
        __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i 
            = ((IData)(1U) + __Vtask_test_hmi_camera_view__DOT__expect_spi__57__unnamedblk1__DOT__i);
    }
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         98);
    if (VL_UNLIKELY((((((((vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U] 
                           != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_hmi)) 
                          | (0xa7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[1U])) 
                         | (0xd7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[4U])) 
                        | (0x34U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[5U])) 
                       | (0x12U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[6U])) 
                      | (vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U] 
                         != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_view)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:102: Assertion failed in %m: FAIL FSPI raw %h, view %h, expected %h/%h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U]
                     , '#',8,__Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_hmi
                     , '#',8,(IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__57__expected_view));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 102, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__62__value = 2U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__63__value = 0x30U;
    if (VL_UNLIKELY(((0xb609c478U != (((((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]) 
                                         << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]) 
                                       << 0x00000010U) 
                                      | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]) 
                                          << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U])))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:105: Assertion failed in %m: FAIL wheel tail %h %h %h %h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 105, "", false);
    }
    if (VL_UNLIKELY(((0xbd3aU != (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]) 
                                   << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:107: Assertion failed in %m: FAIL Autopilot status/CRC %h %h\n",5, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 107, "", false);
    }
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__63__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__64__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__64__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__65__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__65__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__66__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__62__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__66__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    test_hmi_camera_view__DOT__unnamedblk1_2__DOT____Vrepeat1 = 0x00000101U;
    while (VL_LTS_III(32, 0U, test_hmi_camera_view__DOT__unnamedblk1_2__DOT____Vrepeat1)) {
        Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                    "@(posedge test_hmi_camera_view.clk)");
        co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_hmi_camera_view.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                             33);
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             34);
        test_hmi_camera_view__DOT__unnamedblk1_2__DOT____Vrepeat1 
            = (test_hmi_camera_view__DOT__unnamedblk1_2__DOT____Vrepeat1 
               - (IData)(1U));
    }
    __Vtask_test_hmi_camera_view__DOT__expect_view__69__expected = 2U;
    if (VL_UNLIKELY((((IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code) 
                      != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_view__69__expected))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:60: Assertion failed in %m: FAIL view %h, expected %h at %0t\n",6, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_view", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',4,(IData)(vlSelfRef.test_hmi_camera_view__DOT__view_code)
                     , '#',4,__Vtask_test_hmi_camera_view__DOT__expect_view__69__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 60, "", false);
    }
    __Vtask_test_hmi_camera_view__DOT__send_key__70__value = 0x0dU;
    __Vtask_test_hmi_camera_view__DOT__send_byte__71__value = 0x30U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__71__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__72__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__72__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__73__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__73__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__74__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__70__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__74__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    __Vtask_test_hmi_camera_view__DOT__send_key__76__value = 0x0aU;
    __Vtask_test_hmi_camera_view__DOT__send_byte__77__value = 0x30U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__77__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__78__value = 0x90U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__78__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__79__value = 4U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__79__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    __Vtask_test_hmi_camera_view__DOT__send_byte__80__value 
        = __Vtask_test_hmi_camera_view__DOT__send_key__76__value;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         40);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx = __Vtask_test_hmi_camera_view__DOT__send_byte__80__value;
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 1U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(vlSelf, 
                                                                "@(negedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda7ab__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(negedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         43);
    vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid = 0U;
    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                "@(posedge test_hmi_camera_view.clk)");
    co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                         nullptr, 
                                                         "@(posedge test_hmi_camera_view.clk)", 
                                                         "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                         33);
    co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         34);
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_view = 0x33U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_hmi = 0xaaU;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82____Vlvbound_h19eede4c__0 = 0;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         88);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__83__value = 8U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__83__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__84__value = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__84__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i = 0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000000fU, __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i)) {
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__85__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__85__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__85__value;
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__86__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__86__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__86__value;
        __Vtask_test_hmi_camera_view__DOT__expect_spi__82____Vlvbound_h19eede4c__0 
            = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi) 
                << 4U) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo));
        if (VL_LIKELY(((0x0eU >= (0x0000000fU & __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i))))) {
            vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[(0x0000000fU 
                                                                              & __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i)] 
                = __Vtask_test_hmi_camera_view__DOT__expect_spi__82____Vlvbound_h19eede4c__0;
        }
        __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i 
            = ((IData)(1U) + __Vtask_test_hmi_camera_view__DOT__expect_spi__82__unnamedblk1__DOT__i);
    }
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         98);
    if (VL_UNLIKELY((((((((vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U] 
                           != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_hmi)) 
                          | (0xa7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[1U])) 
                         | (0xd7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[4U])) 
                        | (0x34U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[5U])) 
                       | (0x12U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[6U])) 
                      | (vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U] 
                         != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_view)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:102: Assertion failed in %m: FAIL FSPI raw %h, view %h, expected %h/%h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U]
                     , '#',8,__Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_hmi
                     , '#',8,(IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__82__expected_view));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 102, "", false);
    }
    if (VL_UNLIKELY(((0xb609c478U != (((((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]) 
                                         << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]) 
                                       << 0x00000010U) 
                                      | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]) 
                                          << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U])))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:105: Assertion failed in %m: FAIL wheel tail %h %h %h %h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 105, "", false);
    }
    test_hmi_camera_view__DOT__unnamedblk1_3__DOT____Vrepeat2 = 0x00000100U;
    if (VL_UNLIKELY(((0xbd3aU != (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]) 
                                   << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:107: Assertion failed in %m: FAIL Autopilot status/CRC %h %h\n",5, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 107, "", false);
    }
    while (VL_LTS_III(32, 0U, test_hmi_camera_view__DOT__unnamedblk1_3__DOT____Vrepeat2)) {
        Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(vlSelf, 
                                                                    "@(posedge test_hmi_camera_view.clk)");
        co_await vlSelfRef.__VtrigSched_h4bbda687__0.trigger(0U, 
                                                             nullptr, 
                                                             "@(posedge test_hmi_camera_view.clk)", 
                                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                                             33);
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             34);
        test_hmi_camera_view__DOT__unnamedblk1_3__DOT____Vrepeat2 
            = (test_hmi_camera_view__DOT__unnamedblk1_3__DOT____Vrepeat2 
               - (IData)(1U));
    }
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_view = 0x99U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_hmi = 0xaaU;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88____Vlvbound_h19eede4c__0 = 0;
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 1U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         88);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__89__value = 8U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__89__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__90__value = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
    vlSelfRef.test_hmi_camera_view__DOT__host_data 
        = __Vtask_test_hmi_camera_view__DOT__spi_write_nibble__90__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         68);
    vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         69);
    vlSelfRef.test_hmi_camera_view__DOT__host_drives = 0U;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i = 0;
    __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i = 0U;
    while (VL_GTS_III(32, 0x0000000fU, __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i)) {
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__91__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__91__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__91__value;
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__92__value = 0;
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 0U;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000005161ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             76);
        vlSelfRef.test_hmi_camera_view__DOT__spi_clk = 1U;
        co_await vlSelfRef.__VdlySched.delay(0x00000000000003e8ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             77);
        __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__92__value 
            = vlSelfRef.test_hmi_camera_view__DOT__bus;
        co_await vlSelfRef.__VdlySched.delay(0x0000000000004d79ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             78);
        vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo 
            = __Vtask_test_hmi_camera_view__DOT__spi_read_nibble__92__value;
        __Vtask_test_hmi_camera_view__DOT__expect_spi__88____Vlvbound_h19eede4c__0 
            = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__hi) 
                << 4U) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__lo));
        if (VL_LIKELY(((0x0eU >= (0x0000000fU & __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i))))) {
            vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[(0x0000000fU 
                                                                              & __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i)] 
                = __Vtask_test_hmi_camera_view__DOT__expect_spi__88____Vlvbound_h19eede4c__0;
        }
        __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i 
            = ((IData)(1U) + __Vtask_test_hmi_camera_view__DOT__expect_spi__88__unnamedblk1__DOT__i);
    }
    vlSelfRef.test_hmi_camera_view__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                         98);
    if (VL_UNLIKELY((((((((vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U] 
                           != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_hmi)) 
                          | (0xa7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[1U])) 
                         | (0xd7U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[4U])) 
                        | (0x34U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[5U])) 
                       | (0x12U != vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[6U])) 
                      | (vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U] 
                         != (IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_view)))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:102: Assertion failed in %m: FAIL FSPI raw %h, view %h, expected %h/%h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[0U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[8U]
                     , '#',8,__Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_hmi
                     , '#',8,(IData)(__Vtask_test_hmi_camera_view__DOT__expect_spi__88__expected_view));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 102, "", false);
    }
    if (VL_UNLIKELY(((0xb609c478U != (((((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]) 
                                         << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]) 
                                       << 0x00000010U) 
                                      | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]) 
                                          << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U])))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:105: Assertion failed in %m: FAIL wheel tail %h %h %h %h\n",7, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[9U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[10U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[11U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[12U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 105, "", false);
    }
    if (VL_UNLIKELY(((0xbd3aU != (((IData)(vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]) 
                                   << 8U) | vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]))))) {
        VL_WRITEF_NX("[%0t] %%Fatal: test_hmi_camera_view.v:107: Assertion failed in %m: FAIL Autopilot status/CRC %h %h\n",5, 'M',vlSymsp->name(),"test_hmi_camera_view.expect_spi", 'T',-9
                     , '#',64,VL_TIME_UNITED_Q(1000)
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[13U]
                     , '#',8,vlSelfRef.test_hmi_camera_view__DOT__expect_spi__Vstatic__packet[14U]);
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 107, "", false);
    }
    VL_WRITEF_NX("PASS HMI timed camera view, unchanged raw keys, and S-only FSPI view byte\n",0);
    VL_FINISH_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 143, "");
    co_return;
}

VlCoroutine Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__1(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x0000000000001388ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 
                                             6);
        vlSelfRef.test_hmi_camera_view__DOT__clk = 
            (1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__clk)));
    }
    co_return;
}

void Vtest_hmi_camera_view___024root___eval_triggers_vec__act(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_triggers_vec__act\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__clk)) 
                                                        & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0)) 
                                                       << 6U) 
                                                      | ((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                          << 5U) 
                                                         | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs) 
                                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_cs__0))) 
                                                            << 4U))) 
                                                     | (((((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_clk)) 
                                                           & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_clk__0)) 
                                                          << 3U) 
                                                         | (((IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_clk) 
                                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_clk__0))) 
                                                            << 2U)) 
                                                        | ((((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__rst_n)) 
                                                             & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__rst_n__0)) 
                                                            << 1U) 
                                                           | ((IData)(vlSelfRef.test_hmi_camera_view__DOT__clk) 
                                                              & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0))))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0 
        = vlSelfRef.test_hmi_camera_view__DOT__clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__rst_n__0 
        = vlSelfRef.test_hmi_camera_view__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_clk__0 
        = vlSelfRef.test_hmi_camera_view__DOT__spi_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__spi_cs__0 
        = vlSelfRef.test_hmi_camera_view__DOT__spi_cs;
}

bool Vtest_hmi_camera_view___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___trigger_anySet__act\n"); );
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

void Vtest_hmi_camera_view___024root___act_comb__TOP__0(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___act_comb__TOP__0\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_4;
    __VdfgRegularize_h6e95ff9d_0_4 = 0;
    // Body
    vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_complete 
        = ((((IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid) 
             & (3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen))) 
            & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history))) 
           & (0x0007a120U > vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started));
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
                                                & (2U 
                                                   == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg)));
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

void Vtest_hmi_camera_view___024root___eval_act(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_act\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vinline__act_comb__TOP__0___VdfgRegularize_h6e95ff9d_0_4;
    __Vinline__act_comb__TOP__0___VdfgRegularize_h6e95ff9d_0_4 = 0;
    // Body
    if ((0x0000000000000061ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_complete 
            = ((((IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid) 
                 & (3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen))) 
                & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history))) 
               & (0x0007a120U > vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started));
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = 
            ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
             & (2U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg)));
        __Vinline__act_comb__TOP__0___VdfgRegularize_h6e95ff9d_0_4 
            = ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_active) 
               & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2));
        vlSelfRef.test_hmi_camera_view__DOT__bus = 
            ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble)
                : (__Vinline__act_comb__TOP__0___VdfgRegularize_h6e95ff9d_0_4
                    ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble)
                    : ((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                       & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)))))) 
              & ((- (IData)(__Vinline__act_comb__TOP__0___VdfgRegularize_h6e95ff9d_0_4)) 
                 | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))) 
                    | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)))))) 
             | ((IData)(vlSelfRef.test_hmi_camera_view__DOT__host_data) 
                & (- (IData)((IData)(vlSelfRef.test_hmi_camera_view__DOT__host_drives)))));
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus));
    }
}

void Vtest_hmi_camera_view___024root___nba_sequent__TOP__0(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_sequent__TOP__0\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*19:0*/ __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count;
    __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count = 0;
    // Body
    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history 
        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history;
    __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count 
        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count;
    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left 
        = vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left;
    if (vlSelfRef.test_hmi_camera_view__DOT__rst_n) {
        if (vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid) {
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history 
                = ((0x00ffff00U & (vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history 
                                   << 8U)) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx));
            vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen 
                = ((0x0007a120U <= vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count)
                    ? 1U : ((3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen))
                             ? 3U : (3U & ((IData)(1U) 
                                           + (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen)))));
            __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count = 0U;
        } else if ((0x0007a120U > vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count)) {
            __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count 
                = (0x000fffffU & ((IData)(1U) + vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count));
        } else {
            vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen = 0U;
        }
    } else {
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen = 0U;
        __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count = 0U;
    }
    vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count 
        = __Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count;
}

void Vtest_hmi_camera_view___024root___nba_sequent__TOP__1(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_sequent__TOP__1\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag;
    if (vlSelfRef.test_hmi_camera_view__DOT__rst_n) {
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_meta;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_previous 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_meta;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_meta = 0x0dU;
    } else {
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_previous = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_meta = 0U;
    }
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_meta = 0U;
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
}

void Vtest_hmi_camera_view___024root___nba_sequent__TOP__2(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_sequent__TOP__2\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count;
    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_lock;
}

void Vtest_hmi_camera_view___024root___nba_sequent__TOP__3(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_sequent__TOP__3\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__Vfuncout;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__Vfuncout = 0;
    CData/*7:0*/ __Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__value;
    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__value = 0;
    CData/*4:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__extended_index;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__extended_index = 0;
    CData/*3:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot = 0;
    CData/*7:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_snapshot = 0;
    CData/*7:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_crc;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_crc = 0;
    SData/*15:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_snapshot = 0;
    CData/*7:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_crc;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_crc = 0;
    CData/*3:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot = 0;
    IData/*31:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot = 0;
    SData/*15:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot = 0;
    CData/*0:0*/ __Vdly__test_hmi_camera_view__DOT__transport__DOT__status_tx_started;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__status_tx_started = 0;
    // Body
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__status_tx_started 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__extended_index 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_snapshot 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_crc 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_snapshot 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_crc 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_crc;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot;
    __Vdly__test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot 
        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot;
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)))) {
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_active = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__status_tx_started = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble = 0x0fU;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__extended_index = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble = 0x0fU;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_snapshot = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_crc = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_snapshot = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_crc = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot = 0U;
        __Vdly__test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot = 0U;
    } else {
        if ((2U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg))) {
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_active = 1U;
            if ((0x1eU > (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                __Vdly__test_hmi_camera_view__DOT__transport__DOT__extended_index 
                    = (0x0000001fU & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index)));
            }
            if (((((((((0U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index)) 
                       | (1U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                      | (2U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                     | (3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                    | (4U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                   | (5U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                  | (6U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                 | (7U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index)))) {
                if ((0U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__value 
                        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_speed;
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x7cU ^ (IData)(__Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__value));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot 
                        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_snapshot 
                        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_speed;
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c 
                        = (0x000000ffU & ((0x00000080U 
                                           & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c))
                                           ? (7U ^ 
                                              VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U))
                                           : VL_SHIFTL_III(8,8,32, (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c), 1U)));
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_snapshot 
                        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_sync;
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_crc 
                        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__sampled_control_crc;
                    __Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__Vfuncout 
                        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc8__Vstatic__c;
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_crc 
                        = __Vfunc_test_hmi_camera_view__DOT__transport__DOT__speed_crc8__98__Vfuncout;
                } else {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = (0x0000000fU & ((1U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                           ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot)
                                           : ((2U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                               ? 0x0aU
                                               : ((3U 
                                                   == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                                   ? 7U
                                                   : 
                                                  ((4U 
                                                    == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                                    ? 
                                                   ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_snapshot) 
                                                    >> 4U)
                                                    : 
                                                   ((5U 
                                                     == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                                     ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_snapshot)
                                                     : 
                                                    ((6U 
                                                      == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                                      ? 
                                                     ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc) 
                                                      >> 4U)
                                                      : (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc))))))));
                }
            } else if (((((((((8U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index)) 
                              | (9U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                             | (0x0aU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                            | (0x0bU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                           | (0x0cU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                          | (0x0dU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                         | (0x0eU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) 
                        | (0x0fU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index)))) {
                if ((8U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble = 0x0dU;
                } else if ((9U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble = 7U;
                } else if ((0x0aU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_snapshot) 
                                          >> 4U));
                } else if ((0x0bU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = (0x0000000fU & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_snapshot));
                } else if ((0x0cU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_snapshot) 
                                          >> 0x0cU));
                } else if ((0x0dU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_snapshot) 
                                          >> 8U));
                } else if ((0x0eU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = (0x0000000fU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_crc) 
                                          >> 4U));
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot 
                        = (((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_sync) 
                            == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_previous))
                            ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_sync)
                            : 0U);
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot 
                        = ((vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_sync 
                            == vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_previous)
                            ? vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_sync
                            : 0U);
                    __Vdly__test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot 
                        = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_packet;
                } else {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                        = (0x0000000fU & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_crc));
                }
            } else {
                vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble 
                    = (0x0000000fU & ((0x12U > (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                       ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot)
                                       : ((0x1aU > (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                           ? (vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot 
                                              >> (0x0000001fU 
                                                  & (((IData)(0x1fU) 
                                                      - 
                                                      (0x0000001cU 
                                                       & (((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index) 
                                                           - (IData)(2U)) 
                                                          << 2U))) 
                                                     - (IData)(3U))))
                                           : (((2U 
                                                & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                                ? (
                                                   (1U 
                                                    & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                                    ? 
                                                   ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot) 
                                                    >> 8U)
                                                    : 
                                                   ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot) 
                                                    >> 0x0000000cU))
                                                : (
                                                   (1U 
                                                    & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))
                                                    ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot)
                                                    : 
                                                   ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot) 
                                                    >> 4U))) 
                                              | (- (IData)(
                                                           (0x1eU 
                                                            <= (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index))))))));
            }
        }
        if (vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_ready) {
            if (vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started) {
                vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble 
                    = (0x0000000fU & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_data));
            } else {
                vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble 
                    = (0x0000000fU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_data) 
                                      >> 4U));
                __Vdly__test_hmi_camera_view__DOT__transport__DOT__status_tx_started = 1U;
            }
        }
    }
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__status_tx_started;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_index 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__extended_index;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__hmi_snapshot;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_snapshot 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_snapshot;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_crc 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__speed_crc;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_snapshot 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_snapshot;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_crc 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__control_crc;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__camera_view_snapshot;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__wheel_snapshot;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot 
        = __Vdly__test_hmi_camera_view__DOT__transport__DOT__autopilot_snapshot;
}

void Vtest_hmi_camera_view___024root___nba_sequent__TOP__4(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_sequent__TOP__4\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)))) {
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_header = 0U;
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_ready = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_data = 0xffU;
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg = 0U;
    } else {
        if ((6U > (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count))) {
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count)));
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_header 
                = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header;
            if ((IData)(((5U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count)) 
                         & (0x00810000U == (0x00ff0000U 
                                            & vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header))))) {
                vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_ready = 1U;
                vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_data 
                    = ((5U == (0x0000ffffU & vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header))
                        ? ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__auto_enable_sync)
                            ? 0xa1U : 0xa0U) : 0xffU);
            }
        }
        if (((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag))) {
            if ((1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_lock)))) {
                if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data)) 
                               | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus)))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg = 2U;
                } else if ((0U == ((0x000000f0U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data)) 
                                   | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus)))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg = 1U;
                }
                vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock = 1U;
            }
        }
    }
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count 
        = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count;
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_lock 
        = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock;
}

void Vtest_hmi_camera_view___024root___nba_sequent__TOP__5(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_sequent__TOP__5\n"); );
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
    // Body
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
    if (vlSelfRef.test_hmi_camera_view__DOT__rst_n) {
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_sync 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_meta;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_previous 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_sync;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_previous 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_sync;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_meta = 0x1234U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_sync 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_meta;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_sync 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_meta;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_meta = 0xb609c478U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_meta 
            = vlSelfRef.test_hmi_camera_view__DOT__view_code;
    } else {
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_sync = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_previous = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_previous = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__control_meta = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_sync = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_sync = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__wheel_meta = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__camera_view_meta = 0U;
    }
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__auto_enable_sync 
        = ((IData)(vlSelfRef.test_hmi_camera_view__DOT__rst_n) 
           && (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__auto_enable_meta));
    if ((1U & ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data = 0xffU;
    } else if ((1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)))) {
        if (vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag) {
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag)));
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data 
                = ((0xf0U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data)) 
                   | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus));
        } else {
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag)));
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data 
                = ((0x0fU & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data)) 
                   | ((IData)(vlSelfRef.test_hmi_camera_view__DOT__bus) 
                      << 4U));
        }
    }
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag 
        = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag;
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
    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__auto_enable_meta = 0U;
}

void Vtest_hmi_camera_view___024root___nba_comb__TOP__0(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_comb__TOP__0\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started));
}

void Vtest_hmi_camera_view___024root___nba_comb__TOP__1(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_comb__TOP__1\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
                                                & (2U 
                                                   == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg)));
}

void Vtest_hmi_camera_view___024root___nba_sequent__TOP__6(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_sequent__TOP__6\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.test_hmi_camera_view__DOT__rst_n) {
        if ((0U != (IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left))) {
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left 
                = (0x000001ffU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left) 
                                  - (IData)(1U)));
            if ((1U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left))) {
                vlSelfRef.test_hmi_camera_view__DOT__view_code = 9U;
            }
        }
        if (((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
             != (IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__previous_code))) {
            if ((8U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                if ((4U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                    if ((1U & (~ ((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                                  >> 1U)))) {
                        vlSelfRef.test_hmi_camera_view__DOT__view_code 
                            = ((0x0cU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))
                                ? 2U : 3U);
                        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0x0100U;
                    }
                } else if ((1U & (~ ((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                                     >> 1U)))) {
                    vlSelfRef.test_hmi_camera_view__DOT__view_code 
                        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
                }
            } else if ((4U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                vlSelfRef.test_hmi_camera_view__DOT__view_code 
                    = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
            } else if ((2U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                vlSelfRef.test_hmi_camera_view__DOT__view_code 
                    = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
            } else if ((1U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                vlSelfRef.test_hmi_camera_view__DOT__view_code 
                    = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
            }
        }
        vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__previous_code 
            = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
        if (((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_complete) 
             & (4U == (0x000000ffU & vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history)))) {
            vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg 
                = (0x0000000fU & (IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx));
        }
    } else {
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__view_code = 9U;
        vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__previous_code = 0U;
        vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg = 0U;
    }
    vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left 
        = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left;
    vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history 
        = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history;
}

void Vtest_hmi_camera_view___024root___nba_comb__TOP__2(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_comb__TOP__2\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_4;
    __VdfgRegularize_h6e95ff9d_0_4 = 0;
    // Body
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

void Vtest_hmi_camera_view___024root___nba_comb__TOP__3(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___nba_comb__TOP__3\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_complete 
        = ((((IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid) 
             & (3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen))) 
            & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history))) 
           & (0x0007a120U > vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count));
}

void Vtest_hmi_camera_view___024root___eval_nba(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_nba\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*19:0*/ __Vinline__nba_sequent__TOP__0___Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count;
    __Vinline__nba_sequent__TOP__0___Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count = 0;
    CData/*0:0*/ __Vinline__nba_comb__TOP__2___VdfgRegularize_h6e95ff9d_0_4;
    __Vinline__nba_comb__TOP__2___VdfgRegularize_h6e95ff9d_0_4 = 0;
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history 
            = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history;
        __Vinline__nba_sequent__TOP__0___Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count 
            = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count;
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left 
            = vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left;
        if (vlSelfRef.test_hmi_camera_view__DOT__rst_n) {
            if (vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid) {
                vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history 
                    = ((0x00ffff00U & (vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history 
                                       << 8U)) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx));
                vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen 
                    = ((0x0007a120U <= vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count)
                        ? 1U : ((3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen))
                                 ? 3U : (3U & ((IData)(1U) 
                                               + (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen)))));
                __Vinline__nba_sequent__TOP__0___Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count = 0U;
            } else if ((0x0007a120U > vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count)) {
                __Vinline__nba_sequent__TOP__0___Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count 
                    = (0x000fffffU & ((IData)(1U) + vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count));
            } else {
                vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen = 0U;
            }
        } else {
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen = 0U;
            __Vinline__nba_sequent__TOP__0___Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count = 0U;
        }
        vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count 
            = __Vinline__nba_sequent__TOP__0___Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count;
    }
    if ((6ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__recv_flag 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag;
        if (vlSelfRef.test_hmi_camera_view__DOT__rst_n) {
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync 
                = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_meta;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_previous 
                = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync 
                = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_meta;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_meta = 0x0dU;
        } else {
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_sync = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_previous = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_sync = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__autopilot_meta = 0U;
        }
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__speed_meta = 0U;
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
    }
    if ((0x0000000000000016ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count;
        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock 
            = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_lock;
    }
    if ((0x000000000000001aULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_hmi_camera_view___024root___nba_sequent__TOP__3(vlSelf);
    }
    if ((0x0000000000000016ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)))) {
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_header = 0U;
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_ready = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_data = 0xffU;
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg = 0U;
        } else {
            if ((6U > (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count))) {
                vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count 
                    = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count)));
                vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_header 
                    = vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header;
                if ((IData)(((5U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count)) 
                             & (0x00810000U == (0x00ff0000U 
                                                & vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header))))) {
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_ready = 1U;
                    vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_read_data 
                        = ((5U == (0x0000ffffU & vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header))
                            ? ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__auto_enable_sync)
                                ? 0xa1U : 0xa0U) : 0xffU);
                }
            }
            if (((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
                 & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_flag))) {
                if ((1U & (~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_lock)))) {
                    if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data)) 
                                   | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus)))) {
                        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg = 2U;
                    } else if ((0U == ((0x000000f0U 
                                        & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__recv_data)) 
                                       | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus)))) {
                        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg = 1U;
                    }
                    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock = 1U;
                }
            }
        }
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_nibble_count 
            = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__status_nibble_count;
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_lock 
            = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__transport__DOT__cmd_lock;
    }
    if ((6ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_hmi_camera_view___024root___nba_sequent__TOP__5(vlSelf);
    }
    if ((0x000000000000007bULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_started));
    }
    if ((0x0000000000000077ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = 
            ((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__spi_cs)) 
             & (2U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__cmd_reg)));
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if (vlSelfRef.test_hmi_camera_view__DOT__rst_n) {
            if ((0U != (IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left))) {
                vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left 
                    = (0x000001ffU & ((IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left) 
                                      - (IData)(1U)));
                if ((1U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left))) {
                    vlSelfRef.test_hmi_camera_view__DOT__view_code = 9U;
                }
            }
            if (((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                 != (IData)(vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__previous_code))) {
                if ((8U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                    if ((4U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                        if ((1U & (~ ((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                                      >> 1U)))) {
                            vlSelfRef.test_hmi_camera_view__DOT__view_code 
                                = ((0x0cU == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))
                                    ? 2U : 3U);
                            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0x0100U;
                        }
                    } else if ((1U & (~ ((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                                         >> 1U)))) {
                        vlSelfRef.test_hmi_camera_view__DOT__view_code 
                            = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                        vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
                    }
                } else if ((4U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                    vlSelfRef.test_hmi_camera_view__DOT__view_code 
                        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
                } else if ((2U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                    vlSelfRef.test_hmi_camera_view__DOT__view_code 
                        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
                } else if ((1U & (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg))) {
                    vlSelfRef.test_hmi_camera_view__DOT__view_code 
                        = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
                    vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
                }
            }
            vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__previous_code 
                = vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg;
            if (((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_complete) 
                 & (4U == (0x000000ffU & vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history)))) {
                vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg 
                    = (0x0000000fU & (IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx));
            }
        } else {
            vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__view_code = 9U;
            vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__previous_code = 0U;
            vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg = 0U;
        }
        vlSelfRef.test_hmi_camera_view__DOT__preview__DOT__cycles_left 
            = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__preview__DOT__cycles_left;
        vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history 
            = vlSelfRef.__Vdly__test_hmi_camera_view__DOT__decoder__DOT__frame_history;
    }
    if ((0x000000000000007fULL & vlSelfRef.__VnbaTriggered[0U])) {
        __Vinline__nba_comb__TOP__2___VdfgRegularize_h6e95ff9d_0_4 
            = ((IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_active) 
               & (IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2));
        vlSelfRef.test_hmi_camera_view__DOT__bus = 
            ((((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1)
                ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_tx_nibble)
                : (__Vinline__nba_comb__TOP__2___VdfgRegularize_h6e95ff9d_0_4
                    ? (IData)(vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__extended_nibble)
                    : ((IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__cnn_level_reg) 
                       & (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)))))) 
              & ((- (IData)(__Vinline__nba_comb__TOP__2___VdfgRegularize_h6e95ff9d_0_4)) 
                 | ((- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1))) 
                    | (- (IData)((IData)(vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2)))))) 
             | ((IData)(vlSelfRef.test_hmi_camera_view__DOT__host_data) 
                & (- (IData)((IData)(vlSelfRef.test_hmi_camera_view__DOT__host_drives)))));
        vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__next_status_header 
            = ((0x00fffff0U & (vlSelfRef.test_hmi_camera_view__DOT__transport__DOT__status_header 
                               << 4U)) | (IData)(vlSelfRef.test_hmi_camera_view__DOT__bus));
    }
    if ((0x0000000000000063ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_complete 
            = ((((IData)(vlSelfRef.test_hmi_camera_view__DOT__uart_rx_valid) 
                 & (3U == (IData)(vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__bytes_seen))) 
                & (0x00309000U == (0x00ffff00U & vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_history))) 
               & (0x0007a120U > vlSelfRef.test_hmi_camera_view__DOT__decoder__DOT__frame_gap_count));
    }
}

void Vtest_hmi_camera_view___024root___timing_ready(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___timing_ready\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h4bbda687__0.ready("@(posedge test_hmi_camera_view.clk)");
    }
    if ((0x0000000000000040ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready("@(negedge test_hmi_camera_view.clk)");
    }
}

void Vtest_hmi_camera_view___024root___timing_resume(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___timing_resume\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VtrigSched_h4bbda687__0.moveToResumeQueue(
                                                          "@(posedge test_hmi_camera_view.clk)");
    vlSelfRef.__VtrigSched_h4bbda7ab__0.moveToResumeQueue(
                                                          "@(negedge test_hmi_camera_view.clk)");
    vlSelfRef.__VtrigSched_h4bbda687__0.resume("@(posedge test_hmi_camera_view.clk)");
    vlSelfRef.__VtrigSched_h4bbda7ab__0.resume("@(negedge test_hmi_camera_view.clk)");
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_hmi_camera_view___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_hmi_camera_view___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_hmi_camera_view___024root___eval_phase__act(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_phase__act\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_hmi_camera_view___024root___eval_triggers_vec__act(vlSelf);
    Vtest_hmi_camera_view___024root___timing_ready(vlSelf);
    Vtest_hmi_camera_view___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_hmi_camera_view___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_hmi_camera_view___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_hmi_camera_view___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_hmi_camera_view___024root___timing_resume(vlSelf);
        Vtest_hmi_camera_view___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_hmi_camera_view___024root___eval_phase__inact(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_phase__inact\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 3, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_hmi_camera_view___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_hmi_camera_view___024root___eval_phase__nba(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_phase__nba\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_hmi_camera_view___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_hmi_camera_view___024root___eval_nba(vlSelf);
        Vtest_hmi_camera_view___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_hmi_camera_view___024root___eval(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_hmi_camera_view___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 3, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 3, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_hmi_camera_view___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_hmi_camera_view.v", 3, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_hmi_camera_view___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_hmi_camera_view___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_hmi_camera_view___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

void Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0(Vtest_hmi_camera_view___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda687__0\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__clk)) 
                                    & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0)) 
                                   << 6U) | ((IData)(vlSelfRef.test_hmi_camera_view__DOT__clk) 
                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0 
        = vlSelfRef.test_hmi_camera_view__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
    }
    if ((0x0000000000000040ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

void Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0(Vtest_hmi_camera_view___024root* vlSelf, const char* __VeventDescription) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root____VbeforeTrig_h4bbda7ab__0\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    VlUnpacked<QData/*63:0*/, 1> __VTmp;
    // Body
    __VTmp[0U] = (QData)((IData)(((((~ (IData)(vlSelfRef.test_hmi_camera_view__DOT__clk)) 
                                    & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0)) 
                                   << 6U) | ((IData)(vlSelfRef.test_hmi_camera_view__DOT__clk) 
                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_hmi_camera_view__DOT__clk__0 
        = vlSelfRef.test_hmi_camera_view__DOT__clk;
    if ((1ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda687__0.ready(__VeventDescription);
    }
    if ((0x0000000000000040ULL & __VTmp[0U])) {
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
        vlSelfRef.__VtrigSched_h4bbda7ab__0.ready(__VeventDescription);
    }
    vlSelfRef.__VactTriggeredAcc[0U] = (vlSelfRef.__VactTriggeredAcc[0U] 
                                        | __VTmp[0U]);
}

#ifdef VL_DEBUG
void Vtest_hmi_camera_view___024root___eval_debug_assertions(Vtest_hmi_camera_view___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_hmi_camera_view___024root___eval_debug_assertions\n"); );
    Vtest_hmi_camera_view__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
