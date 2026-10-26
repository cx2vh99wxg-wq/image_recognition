// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtest_fspi_control.h for the primary calling header

#include "Vtest_fspi_control__pch.h"

VlCoroutine Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__0(Vtest_fspi_control___024root* vlSelf);
VlCoroutine Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__1(Vtest_fspi_control___024root* vlSelf);

void Vtest_fspi_control___024root___eval_initial(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_initial\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__0(vlSelf);
    Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__1(vlSelf);
}

VlCoroutine Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__0(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__0\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__0__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__0__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__0__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__0__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__0__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__0__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__1__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__1__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__1__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__1__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__1__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__1__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__2__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__2__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__2__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__2__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__2__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__2__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__3__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__3__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__3__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__3__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__3__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__3__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__4__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__4__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__4__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__4__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__4__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__4__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__5__addr;
    __Vtask_test_fspi_control__DOT__write_reg__5__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__5__value;
    __Vtask_test_fspi_control__DOT__write_reg__5__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__6__value;
    __Vtask_test_fspi_control__DOT__send_byte__6__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__7__value;
    __Vtask_test_fspi_control__DOT__send_nibble__7__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__8__value;
    __Vtask_test_fspi_control__DOT__send_nibble__8__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__9__value;
    __Vtask_test_fspi_control__DOT__send_byte__9__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__10__value;
    __Vtask_test_fspi_control__DOT__send_nibble__10__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__11__value;
    __Vtask_test_fspi_control__DOT__send_nibble__11__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__12__value;
    __Vtask_test_fspi_control__DOT__send_byte__12__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__13__value;
    __Vtask_test_fspi_control__DOT__send_nibble__13__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__14__value;
    __Vtask_test_fspi_control__DOT__send_nibble__14__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__15__value;
    __Vtask_test_fspi_control__DOT__send_byte__15__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__16__value;
    __Vtask_test_fspi_control__DOT__send_nibble__16__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__17__value;
    __Vtask_test_fspi_control__DOT__send_nibble__17__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__18__value;
    __Vtask_test_fspi_control__DOT__send_byte__18__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__19__value;
    __Vtask_test_fspi_control__DOT__send_nibble__19__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__20__value;
    __Vtask_test_fspi_control__DOT__send_nibble__20__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__21__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__21__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__21__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__21__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__21__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__21__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__22__addr;
    __Vtask_test_fspi_control__DOT__write_reg__22__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__22__value;
    __Vtask_test_fspi_control__DOT__write_reg__22__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__23__value;
    __Vtask_test_fspi_control__DOT__send_byte__23__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__24__value;
    __Vtask_test_fspi_control__DOT__send_nibble__24__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__25__value;
    __Vtask_test_fspi_control__DOT__send_nibble__25__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__26__value;
    __Vtask_test_fspi_control__DOT__send_byte__26__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__27__value;
    __Vtask_test_fspi_control__DOT__send_nibble__27__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__28__value;
    __Vtask_test_fspi_control__DOT__send_nibble__28__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__29__value;
    __Vtask_test_fspi_control__DOT__send_byte__29__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__30__value;
    __Vtask_test_fspi_control__DOT__send_nibble__30__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__31__value;
    __Vtask_test_fspi_control__DOT__send_nibble__31__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__32__value;
    __Vtask_test_fspi_control__DOT__send_byte__32__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__33__value;
    __Vtask_test_fspi_control__DOT__send_nibble__33__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__34__value;
    __Vtask_test_fspi_control__DOT__send_nibble__34__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__35__value;
    __Vtask_test_fspi_control__DOT__send_byte__35__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__36__value;
    __Vtask_test_fspi_control__DOT__send_nibble__36__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__37__value;
    __Vtask_test_fspi_control__DOT__send_nibble__37__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__38__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__38__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__38__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__38__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__38__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__38__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__39__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__39__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__39__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__39__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__39__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__39__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__40__addr;
    __Vtask_test_fspi_control__DOT__write_reg__40__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__40__value;
    __Vtask_test_fspi_control__DOT__write_reg__40__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__41__value;
    __Vtask_test_fspi_control__DOT__send_byte__41__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__42__value;
    __Vtask_test_fspi_control__DOT__send_nibble__42__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__43__value;
    __Vtask_test_fspi_control__DOT__send_nibble__43__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__44__value;
    __Vtask_test_fspi_control__DOT__send_byte__44__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__45__value;
    __Vtask_test_fspi_control__DOT__send_nibble__45__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__46__value;
    __Vtask_test_fspi_control__DOT__send_nibble__46__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__47__value;
    __Vtask_test_fspi_control__DOT__send_byte__47__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__48__value;
    __Vtask_test_fspi_control__DOT__send_nibble__48__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__49__value;
    __Vtask_test_fspi_control__DOT__send_nibble__49__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__50__value;
    __Vtask_test_fspi_control__DOT__send_byte__50__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__51__value;
    __Vtask_test_fspi_control__DOT__send_nibble__51__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__52__value;
    __Vtask_test_fspi_control__DOT__send_nibble__52__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__53__value;
    __Vtask_test_fspi_control__DOT__send_byte__53__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__54__value;
    __Vtask_test_fspi_control__DOT__send_nibble__54__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__55__value;
    __Vtask_test_fspi_control__DOT__send_nibble__55__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__56__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__56__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__56__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__56__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__56__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__56__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__57__addr;
    __Vtask_test_fspi_control__DOT__write_reg__57__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__57__value;
    __Vtask_test_fspi_control__DOT__write_reg__57__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__58__value;
    __Vtask_test_fspi_control__DOT__send_byte__58__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__59__value;
    __Vtask_test_fspi_control__DOT__send_nibble__59__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__60__value;
    __Vtask_test_fspi_control__DOT__send_nibble__60__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__61__value;
    __Vtask_test_fspi_control__DOT__send_byte__61__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__62__value;
    __Vtask_test_fspi_control__DOT__send_nibble__62__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__63__value;
    __Vtask_test_fspi_control__DOT__send_nibble__63__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__64__value;
    __Vtask_test_fspi_control__DOT__send_byte__64__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__65__value;
    __Vtask_test_fspi_control__DOT__send_nibble__65__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__66__value;
    __Vtask_test_fspi_control__DOT__send_nibble__66__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__67__value;
    __Vtask_test_fspi_control__DOT__send_byte__67__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__68__value;
    __Vtask_test_fspi_control__DOT__send_nibble__68__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__69__value;
    __Vtask_test_fspi_control__DOT__send_nibble__69__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__70__value;
    __Vtask_test_fspi_control__DOT__send_byte__70__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__71__value;
    __Vtask_test_fspi_control__DOT__send_nibble__71__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__72__value;
    __Vtask_test_fspi_control__DOT__send_nibble__72__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__73__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__73__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__73__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__73__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__73__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__73__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__74__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__74__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__74__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__74__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__74__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__74__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__75__addr;
    __Vtask_test_fspi_control__DOT__write_reg__75__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__75__value;
    __Vtask_test_fspi_control__DOT__write_reg__75__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__76__value;
    __Vtask_test_fspi_control__DOT__send_byte__76__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__77__value;
    __Vtask_test_fspi_control__DOT__send_nibble__77__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__78__value;
    __Vtask_test_fspi_control__DOT__send_nibble__78__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__79__value;
    __Vtask_test_fspi_control__DOT__send_byte__79__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__80__value;
    __Vtask_test_fspi_control__DOT__send_nibble__80__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__81__value;
    __Vtask_test_fspi_control__DOT__send_nibble__81__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__82__value;
    __Vtask_test_fspi_control__DOT__send_byte__82__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__83__value;
    __Vtask_test_fspi_control__DOT__send_nibble__83__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__84__value;
    __Vtask_test_fspi_control__DOT__send_nibble__84__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__85__value;
    __Vtask_test_fspi_control__DOT__send_byte__85__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__86__value;
    __Vtask_test_fspi_control__DOT__send_nibble__86__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__87__value;
    __Vtask_test_fspi_control__DOT__send_nibble__87__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__88__value;
    __Vtask_test_fspi_control__DOT__send_byte__88__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__89__value;
    __Vtask_test_fspi_control__DOT__send_nibble__89__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__90__value;
    __Vtask_test_fspi_control__DOT__send_nibble__90__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__91__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__91__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__91__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__91__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__91__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__91__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__92__addr;
    __Vtask_test_fspi_control__DOT__write_reg__92__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__92__value;
    __Vtask_test_fspi_control__DOT__write_reg__92__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__93__value;
    __Vtask_test_fspi_control__DOT__send_byte__93__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__94__value;
    __Vtask_test_fspi_control__DOT__send_nibble__94__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__95__value;
    __Vtask_test_fspi_control__DOT__send_nibble__95__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__96__value;
    __Vtask_test_fspi_control__DOT__send_byte__96__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__97__value;
    __Vtask_test_fspi_control__DOT__send_nibble__97__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__98__value;
    __Vtask_test_fspi_control__DOT__send_nibble__98__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__99__value;
    __Vtask_test_fspi_control__DOT__send_byte__99__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__100__value;
    __Vtask_test_fspi_control__DOT__send_nibble__100__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__101__value;
    __Vtask_test_fspi_control__DOT__send_nibble__101__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__102__value;
    __Vtask_test_fspi_control__DOT__send_byte__102__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__103__value;
    __Vtask_test_fspi_control__DOT__send_nibble__103__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__104__value;
    __Vtask_test_fspi_control__DOT__send_nibble__104__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__105__value;
    __Vtask_test_fspi_control__DOT__send_byte__105__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__106__value;
    __Vtask_test_fspi_control__DOT__send_nibble__106__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__107__value;
    __Vtask_test_fspi_control__DOT__send_nibble__107__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__108__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__108__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__108__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__108__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__108__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__108__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__109__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__109__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__109__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__109__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__109__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__109__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__110__addr;
    __Vtask_test_fspi_control__DOT__write_reg__110__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__110__value;
    __Vtask_test_fspi_control__DOT__write_reg__110__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__111__value;
    __Vtask_test_fspi_control__DOT__send_byte__111__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__112__value;
    __Vtask_test_fspi_control__DOT__send_nibble__112__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__113__value;
    __Vtask_test_fspi_control__DOT__send_nibble__113__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__114__value;
    __Vtask_test_fspi_control__DOT__send_byte__114__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__115__value;
    __Vtask_test_fspi_control__DOT__send_nibble__115__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__116__value;
    __Vtask_test_fspi_control__DOT__send_nibble__116__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__117__value;
    __Vtask_test_fspi_control__DOT__send_byte__117__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__118__value;
    __Vtask_test_fspi_control__DOT__send_nibble__118__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__119__value;
    __Vtask_test_fspi_control__DOT__send_nibble__119__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__120__value;
    __Vtask_test_fspi_control__DOT__send_byte__120__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__121__value;
    __Vtask_test_fspi_control__DOT__send_nibble__121__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__122__value;
    __Vtask_test_fspi_control__DOT__send_nibble__122__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__123__value;
    __Vtask_test_fspi_control__DOT__send_byte__123__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__124__value;
    __Vtask_test_fspi_control__DOT__send_nibble__124__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__125__value;
    __Vtask_test_fspi_control__DOT__send_nibble__125__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__126__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__126__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__126__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__126__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__126__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__126__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__127__addr;
    __Vtask_test_fspi_control__DOT__write_reg__127__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__127__value;
    __Vtask_test_fspi_control__DOT__write_reg__127__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__128__value;
    __Vtask_test_fspi_control__DOT__send_byte__128__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__129__value;
    __Vtask_test_fspi_control__DOT__send_nibble__129__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__130__value;
    __Vtask_test_fspi_control__DOT__send_nibble__130__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__131__value;
    __Vtask_test_fspi_control__DOT__send_byte__131__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__132__value;
    __Vtask_test_fspi_control__DOT__send_nibble__132__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__133__value;
    __Vtask_test_fspi_control__DOT__send_nibble__133__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__134__value;
    __Vtask_test_fspi_control__DOT__send_byte__134__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__135__value;
    __Vtask_test_fspi_control__DOT__send_nibble__135__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__136__value;
    __Vtask_test_fspi_control__DOT__send_nibble__136__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__137__value;
    __Vtask_test_fspi_control__DOT__send_byte__137__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__138__value;
    __Vtask_test_fspi_control__DOT__send_nibble__138__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__139__value;
    __Vtask_test_fspi_control__DOT__send_nibble__139__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__140__value;
    __Vtask_test_fspi_control__DOT__send_byte__140__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__141__value;
    __Vtask_test_fspi_control__DOT__send_nibble__141__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__142__value;
    __Vtask_test_fspi_control__DOT__send_nibble__142__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__143__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__143__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__143__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__143__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__143__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__143__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__144__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__144__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__144__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__144__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__144__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__144__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__145__addr;
    __Vtask_test_fspi_control__DOT__write_reg__145__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__145__value;
    __Vtask_test_fspi_control__DOT__write_reg__145__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__146__value;
    __Vtask_test_fspi_control__DOT__send_byte__146__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__147__value;
    __Vtask_test_fspi_control__DOT__send_nibble__147__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__148__value;
    __Vtask_test_fspi_control__DOT__send_nibble__148__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__149__value;
    __Vtask_test_fspi_control__DOT__send_byte__149__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__150__value;
    __Vtask_test_fspi_control__DOT__send_nibble__150__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__151__value;
    __Vtask_test_fspi_control__DOT__send_nibble__151__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__152__value;
    __Vtask_test_fspi_control__DOT__send_byte__152__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__153__value;
    __Vtask_test_fspi_control__DOT__send_nibble__153__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__154__value;
    __Vtask_test_fspi_control__DOT__send_nibble__154__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__155__value;
    __Vtask_test_fspi_control__DOT__send_byte__155__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__156__value;
    __Vtask_test_fspi_control__DOT__send_nibble__156__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__157__value;
    __Vtask_test_fspi_control__DOT__send_nibble__157__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__158__value;
    __Vtask_test_fspi_control__DOT__send_byte__158__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__159__value;
    __Vtask_test_fspi_control__DOT__send_nibble__159__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__160__value;
    __Vtask_test_fspi_control__DOT__send_nibble__160__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__161__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__161__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__161__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__161__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__161__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__161__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__162__addr;
    __Vtask_test_fspi_control__DOT__write_reg__162__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__162__value;
    __Vtask_test_fspi_control__DOT__write_reg__162__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__163__value;
    __Vtask_test_fspi_control__DOT__send_byte__163__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__164__value;
    __Vtask_test_fspi_control__DOT__send_nibble__164__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__165__value;
    __Vtask_test_fspi_control__DOT__send_nibble__165__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__166__value;
    __Vtask_test_fspi_control__DOT__send_byte__166__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__167__value;
    __Vtask_test_fspi_control__DOT__send_nibble__167__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__168__value;
    __Vtask_test_fspi_control__DOT__send_nibble__168__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__169__value;
    __Vtask_test_fspi_control__DOT__send_byte__169__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__170__value;
    __Vtask_test_fspi_control__DOT__send_nibble__170__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__171__value;
    __Vtask_test_fspi_control__DOT__send_nibble__171__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__172__value;
    __Vtask_test_fspi_control__DOT__send_byte__172__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__173__value;
    __Vtask_test_fspi_control__DOT__send_nibble__173__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__174__value;
    __Vtask_test_fspi_control__DOT__send_nibble__174__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__175__value;
    __Vtask_test_fspi_control__DOT__send_byte__175__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__176__value;
    __Vtask_test_fspi_control__DOT__send_nibble__176__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__177__value;
    __Vtask_test_fspi_control__DOT__send_nibble__177__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__178__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__178__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__178__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__178__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__178__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__178__label);
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__179__addr;
    __Vtask_test_fspi_control__DOT__write_reg__179__addr = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__write_reg__179__value;
    __Vtask_test_fspi_control__DOT__write_reg__179__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__180__value;
    __Vtask_test_fspi_control__DOT__send_byte__180__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__181__value;
    __Vtask_test_fspi_control__DOT__send_nibble__181__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__182__value;
    __Vtask_test_fspi_control__DOT__send_nibble__182__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__183__value;
    __Vtask_test_fspi_control__DOT__send_byte__183__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__184__value;
    __Vtask_test_fspi_control__DOT__send_nibble__184__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__185__value;
    __Vtask_test_fspi_control__DOT__send_nibble__185__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__186__value;
    __Vtask_test_fspi_control__DOT__send_byte__186__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__187__value;
    __Vtask_test_fspi_control__DOT__send_nibble__187__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__188__value;
    __Vtask_test_fspi_control__DOT__send_nibble__188__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__189__value;
    __Vtask_test_fspi_control__DOT__send_byte__189__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__190__value;
    __Vtask_test_fspi_control__DOT__send_nibble__190__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__191__value;
    __Vtask_test_fspi_control__DOT__send_nibble__191__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__send_byte__192__value;
    __Vtask_test_fspi_control__DOT__send_byte__192__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__193__value;
    __Vtask_test_fspi_control__DOT__send_nibble__193__value = 0;
    CData/*3:0*/ __Vtask_test_fspi_control__DOT__send_nibble__194__value;
    __Vtask_test_fspi_control__DOT__send_nibble__194__value = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__195__actual;
    __Vtask_test_fspi_control__DOT__expect_byte__195__actual = 0;
    CData/*7:0*/ __Vtask_test_fspi_control__DOT__expect_byte__195__expected;
    __Vtask_test_fspi_control__DOT__expect_byte__195__expected = 0;
    VlWide<4>/*127:0*/ __Vtask_test_fspi_control__DOT__expect_byte__195__label;
    VL_ZERO_W(128, __Vtask_test_fspi_control__DOT__expect_byte__195__label);
    // Body
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         76);
    vlSelfRef.test_fspi_control__DOT__rst_n = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x000000000000c350ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         77);
    __Vtask_test_fspi_control__DOT__expect_byte__0__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__0__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__0__label[2U] = 0x20676f20U;
    __Vtask_test_fspi_control__DOT__expect_byte__0__label[3U] = 0x65736574U;
    __Vtask_test_fspi_control__DOT__expect_byte__0__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__0__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[0U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__0__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__0__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__0__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__0__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__0__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__1__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__1__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__1__label[2U] = 0x61636b20U;
    __Vtask_test_fspi_control__DOT__expect_byte__1__label[3U] = 0x65742062U;
    __Vtask_test_fspi_control__DOT__expect_byte__1__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__1__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[1U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__1__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__1__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__1__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__1__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__1__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__2__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__2__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__2__label[2U] = 0x65667420U;
    __Vtask_test_fspi_control__DOT__expect_byte__2__label[3U] = 0x6574206cU;
    __Vtask_test_fspi_control__DOT__expect_byte__2__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__2__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[2U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__2__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__2__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__2__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__2__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__2__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__3__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__3__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__3__label[2U] = 0x67687420U;
    __Vtask_test_fspi_control__DOT__expect_byte__3__label[3U] = 0x74207269U;
    __Vtask_test_fspi_control__DOT__expect_byte__3__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__3__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[3U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__3__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__3__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__3__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__3__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__3__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__4__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__4__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__4__label[2U] = 0x746f7020U;
    __Vtask_test_fspi_control__DOT__expect_byte__4__label[3U] = 0x65742073U;
    __Vtask_test_fspi_control__DOT__expect_byte__4__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__4__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[4U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__4__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__4__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__4__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__4__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__4__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__5__value = 1U;
    __Vtask_test_fspi_control__DOT__write_reg__5__addr = 4U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__6__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__7__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__6__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__7__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__8__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__6__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__8__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__9__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__10__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__9__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__10__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__11__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__9__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__11__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__12__value 
        = __Vtask_test_fspi_control__DOT__write_reg__5__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__13__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__12__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__13__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__14__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__12__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__14__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__15__value 
        = __Vtask_test_fspi_control__DOT__write_reg__5__value;
    __Vtask_test_fspi_control__DOT__send_nibble__16__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__15__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__16__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__17__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__15__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__17__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__18__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__19__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__18__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__19__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__20__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__18__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__20__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__21__label[0U] = 0x73746f70U;
    __Vtask_test_fspi_control__DOT__expect_byte__21__label[1U] = 0x61736520U;
    __Vtask_test_fspi_control__DOT__expect_byte__21__label[2U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__21__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__21__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__21__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[4U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__21__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__21__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__21__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__21__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__21__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__22__value = 0U;
    __Vtask_test_fspi_control__DOT__write_reg__22__addr = 0U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__23__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__24__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__23__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__24__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__25__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__23__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__25__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__26__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__27__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__26__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__27__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__28__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__26__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__28__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__29__value 
        = __Vtask_test_fspi_control__DOT__write_reg__22__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__30__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__29__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__30__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__31__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__29__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__31__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__32__value 
        = __Vtask_test_fspi_control__DOT__write_reg__22__value;
    __Vtask_test_fspi_control__DOT__send_nibble__33__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__32__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__33__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__34__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__32__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__34__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__35__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__36__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__35__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__36__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__37__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__35__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__37__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__38__label[0U] = 0x7320676fU;
    __Vtask_test_fspi_control__DOT__expect_byte__38__label[1U] = 0x70726573U;
    __Vtask_test_fspi_control__DOT__expect_byte__38__label[2U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__38__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__38__expected = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__38__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[0U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__38__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__38__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__38__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__38__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__38__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__39__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__39__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__39__label[2U] = 0x696c6c20U;
    __Vtask_test_fspi_control__DOT__expect_byte__39__label[3U] = 0x6b207374U;
    __Vtask_test_fspi_control__DOT__expect_byte__39__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__39__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[1U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__39__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__39__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__39__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__39__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__39__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__40__value = 1U;
    __Vtask_test_fspi_control__DOT__write_reg__40__addr = 0U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__41__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__42__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__41__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__42__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__43__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__41__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__43__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__44__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__45__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__44__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__45__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__46__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__44__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__46__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__47__value 
        = __Vtask_test_fspi_control__DOT__write_reg__40__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__48__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__47__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__48__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__49__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__47__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__49__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__50__value 
        = __Vtask_test_fspi_control__DOT__write_reg__40__value;
    __Vtask_test_fspi_control__DOT__send_nibble__51__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__50__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__51__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__52__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__50__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__52__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__53__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__54__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__53__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__54__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__55__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__53__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__55__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__56__label[0U] = 0x6520676fU;
    __Vtask_test_fspi_control__DOT__expect_byte__56__label[1U] = 0x6c656173U;
    __Vtask_test_fspi_control__DOT__expect_byte__56__label[2U] = 0x00007265U;
    __Vtask_test_fspi_control__DOT__expect_byte__56__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__56__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__56__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[0U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__56__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__56__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__56__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__56__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__56__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__57__value = 0U;
    __Vtask_test_fspi_control__DOT__write_reg__57__addr = 1U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__58__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__59__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__58__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__59__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__60__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__58__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__60__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__61__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__62__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__61__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__62__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__63__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__61__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__63__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__64__value 
        = __Vtask_test_fspi_control__DOT__write_reg__57__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__65__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__64__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__65__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__66__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__64__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__66__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__67__value 
        = __Vtask_test_fspi_control__DOT__write_reg__57__value;
    __Vtask_test_fspi_control__DOT__send_nibble__68__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__67__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__68__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__69__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__67__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__69__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__70__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__71__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__70__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__71__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__72__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__70__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__72__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__73__label[0U] = 0x6261636bU;
    __Vtask_test_fspi_control__DOT__expect_byte__73__label[1U] = 0x65737320U;
    __Vtask_test_fspi_control__DOT__expect_byte__73__label[2U] = 0x00007072U;
    __Vtask_test_fspi_control__DOT__expect_byte__73__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__73__expected = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__73__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[1U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__73__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__73__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__73__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__73__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__73__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__74__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__74__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__74__label[2U] = 0x696c6c20U;
    __Vtask_test_fspi_control__DOT__expect_byte__74__label[3U] = 0x6f207374U;
    __Vtask_test_fspi_control__DOT__expect_byte__74__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__74__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[0U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__74__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__74__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__74__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__74__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__74__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__75__value = 1U;
    __Vtask_test_fspi_control__DOT__write_reg__75__addr = 1U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__76__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__77__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__76__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__77__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__78__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__76__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__78__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__79__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__80__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__79__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__80__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__81__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__79__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__81__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__82__value 
        = __Vtask_test_fspi_control__DOT__write_reg__75__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__83__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__82__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__83__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__84__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__82__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__84__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__85__value 
        = __Vtask_test_fspi_control__DOT__write_reg__75__value;
    __Vtask_test_fspi_control__DOT__send_nibble__86__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__85__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__86__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__87__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__85__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__87__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__88__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__89__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__88__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__89__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__90__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__88__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__90__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__91__label[0U] = 0x6261636bU;
    __Vtask_test_fspi_control__DOT__expect_byte__91__label[1U] = 0x61736520U;
    __Vtask_test_fspi_control__DOT__expect_byte__91__label[2U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__91__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__91__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__91__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[1U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__91__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__91__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__91__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__91__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__91__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__92__value = 0U;
    __Vtask_test_fspi_control__DOT__write_reg__92__addr = 2U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__93__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__94__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__93__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__94__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__95__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__93__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__95__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__96__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__97__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__96__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__97__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__98__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__96__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__98__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__99__value 
        = __Vtask_test_fspi_control__DOT__write_reg__92__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__100__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__99__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__100__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__101__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__99__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__101__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__102__value 
        = __Vtask_test_fspi_control__DOT__write_reg__92__value;
    __Vtask_test_fspi_control__DOT__send_nibble__103__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__102__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__103__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__104__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__102__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__104__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__105__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__106__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__105__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__106__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__107__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__105__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__107__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__108__label[0U] = 0x6c656674U;
    __Vtask_test_fspi_control__DOT__expect_byte__108__label[1U] = 0x65737320U;
    __Vtask_test_fspi_control__DOT__expect_byte__108__label[2U] = 0x00007072U;
    __Vtask_test_fspi_control__DOT__expect_byte__108__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__108__expected = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__108__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[2U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__108__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__108__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__108__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__108__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__108__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__109__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__109__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__109__label[2U] = 0x696c6c20U;
    __Vtask_test_fspi_control__DOT__expect_byte__109__label[3U] = 0x74207374U;
    __Vtask_test_fspi_control__DOT__expect_byte__109__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__109__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[3U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__109__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__109__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__109__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__109__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__109__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__110__value = 1U;
    __Vtask_test_fspi_control__DOT__write_reg__110__addr = 2U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__111__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__112__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__111__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__112__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__113__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__111__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__113__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__114__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__115__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__114__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__115__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__116__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__114__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__116__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__117__value 
        = __Vtask_test_fspi_control__DOT__write_reg__110__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__118__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__117__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__118__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__119__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__117__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__119__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__120__value 
        = __Vtask_test_fspi_control__DOT__write_reg__110__value;
    __Vtask_test_fspi_control__DOT__send_nibble__121__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__120__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__121__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__122__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__120__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__122__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__123__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__124__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__123__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__124__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__125__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__123__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__125__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__126__label[0U] = 0x6c656674U;
    __Vtask_test_fspi_control__DOT__expect_byte__126__label[1U] = 0x61736520U;
    __Vtask_test_fspi_control__DOT__expect_byte__126__label[2U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__126__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__126__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__126__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[2U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__126__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__126__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__126__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__126__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__126__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__127__value = 0U;
    __Vtask_test_fspi_control__DOT__write_reg__127__addr = 3U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__128__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__129__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__128__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__129__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__130__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__128__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__130__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__131__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__132__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__131__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__132__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__133__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__131__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__133__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__134__value 
        = __Vtask_test_fspi_control__DOT__write_reg__127__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__135__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__134__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__135__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__136__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__134__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__136__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__137__value 
        = __Vtask_test_fspi_control__DOT__write_reg__127__value;
    __Vtask_test_fspi_control__DOT__send_nibble__138__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__137__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__138__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__139__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__137__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__139__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__140__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__141__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__140__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__141__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__142__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__140__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__142__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__143__label[0U] = 0x69676874U;
    __Vtask_test_fspi_control__DOT__expect_byte__143__label[1U] = 0x73732072U;
    __Vtask_test_fspi_control__DOT__expect_byte__143__label[2U] = 0x00707265U;
    __Vtask_test_fspi_control__DOT__expect_byte__143__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__143__expected = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__143__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[3U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__143__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__143__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__143__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__143__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__143__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__expect_byte__144__label[0U] = 0x61736564U;
    __Vtask_test_fspi_control__DOT__expect_byte__144__label[1U] = 0x72656c65U;
    __Vtask_test_fspi_control__DOT__expect_byte__144__label[2U] = 0x696c6c20U;
    __Vtask_test_fspi_control__DOT__expect_byte__144__label[3U] = 0x74207374U;
    __Vtask_test_fspi_control__DOT__expect_byte__144__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__144__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[2U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__144__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__144__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__144__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__144__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__144__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__145__value = 1U;
    __Vtask_test_fspi_control__DOT__write_reg__145__addr = 3U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__146__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__147__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__146__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__147__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__148__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__146__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__148__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__149__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__150__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__149__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__150__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__151__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__149__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__151__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__152__value 
        = __Vtask_test_fspi_control__DOT__write_reg__145__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__153__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__152__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__153__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__154__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__152__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__154__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__155__value 
        = __Vtask_test_fspi_control__DOT__write_reg__145__value;
    __Vtask_test_fspi_control__DOT__send_nibble__156__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__155__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__156__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__157__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__155__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__157__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__158__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__159__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__158__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__159__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__160__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__158__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__160__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__161__label[0U] = 0x69676874U;
    __Vtask_test_fspi_control__DOT__expect_byte__161__label[1U] = 0x73652072U;
    __Vtask_test_fspi_control__DOT__expect_byte__161__label[2U] = 0x656c6561U;
    __Vtask_test_fspi_control__DOT__expect_byte__161__label[3U] = 0x00000072U;
    __Vtask_test_fspi_control__DOT__expect_byte__161__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__161__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[3U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__161__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__161__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__161__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__161__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__161__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__162__value = 0U;
    __Vtask_test_fspi_control__DOT__write_reg__162__addr = 4U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__163__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__164__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__163__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__164__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__165__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__163__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__165__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__166__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__167__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__166__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__167__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__168__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__166__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__168__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__169__value 
        = __Vtask_test_fspi_control__DOT__write_reg__162__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__170__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__169__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__170__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__171__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__169__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__171__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__172__value 
        = __Vtask_test_fspi_control__DOT__write_reg__162__value;
    __Vtask_test_fspi_control__DOT__send_nibble__173__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__172__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__173__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__174__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__172__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__174__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__175__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__176__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__175__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__176__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__177__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__175__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__177__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__178__label[0U] = 0x73746f70U;
    __Vtask_test_fspi_control__DOT__expect_byte__178__label[1U] = 0x65727420U;
    __Vtask_test_fspi_control__DOT__expect_byte__178__label[2U] = 0x00617373U;
    __Vtask_test_fspi_control__DOT__expect_byte__178__label[3U] = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__178__expected = 0U;
    __Vtask_test_fspi_control__DOT__expect_byte__178__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[4U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__178__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__178__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__178__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__178__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__178__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    __Vtask_test_fspi_control__DOT__write_reg__179__value = 1U;
    __Vtask_test_fspi_control__DOT__write_reg__179__addr = 4U;
    vlSelfRef.test_fspi_control__DOT__spi_cs = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         55);
    __Vtask_test_fspi_control__DOT__send_byte__180__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__181__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__180__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__181__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__182__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__180__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__182__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__183__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__184__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__183__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__184__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__185__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__183__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__185__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__186__value 
        = __Vtask_test_fspi_control__DOT__write_reg__179__addr;
    __Vtask_test_fspi_control__DOT__send_nibble__187__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__186__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__187__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__188__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__186__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__188__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__189__value 
        = __Vtask_test_fspi_control__DOT__write_reg__179__value;
    __Vtask_test_fspi_control__DOT__send_nibble__190__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__189__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__190__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__191__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__189__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__191__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_byte__192__value = 0U;
    __Vtask_test_fspi_control__DOT__send_nibble__193__value 
        = (0x0000000fU & ((IData)(__Vtask_test_fspi_control__DOT__send_byte__192__value) 
                          >> 4U));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__193__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    __Vtask_test_fspi_control__DOT__send_nibble__194__value 
        = (0x0000000fU & (IData)(__Vtask_test_fspi_control__DOT__send_byte__192__value));
    vlSelfRef.test_fspi_control__DOT__master_data = __Vtask_test_fspi_control__DOT__send_nibble__194__value;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         39);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 0U;
    co_await vlSelfRef.__VdlySched.delay(0x0000000000002710ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         40);
    vlSelfRef.test_fspi_control__DOT__spi_clk = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000007d0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         41);
    co_await vlSelfRef.__VdlySched.delay(0x0000000000004e20ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         61);
    vlSelfRef.test_fspi_control__DOT__spi_cs = 1U;
    co_await vlSelfRef.__VdlySched.delay(0x00000000000186a0ULL, 
                                         nullptr, "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                         62);
    __Vtask_test_fspi_control__DOT__expect_byte__195__label[0U] = 0x6761696eU;
    __Vtask_test_fspi_control__DOT__expect_byte__195__label[1U] = 0x73652061U;
    __Vtask_test_fspi_control__DOT__expect_byte__195__label[2U] = 0x656c6561U;
    __Vtask_test_fspi_control__DOT__expect_byte__195__label[3U] = 0x00000072U;
    __Vtask_test_fspi_control__DOT__expect_byte__195__expected = 1U;
    __Vtask_test_fspi_control__DOT__expect_byte__195__actual 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[4U];
    if (VL_UNLIKELY((((IData)(__Vtask_test_fspi_control__DOT__expect_byte__195__actual) 
                      != (IData)(__Vtask_test_fspi_control__DOT__expect_byte__195__expected))))) {
        VL_WRITEF_NX("FAIL %s: actual=%02h expected=%02h\n[%0t] %%Fatal: test_fspi_control.v:70: Assertion failed in %m\n",6, 'M',vlSymsp->name(),"test_fspi_control.expect_byte", 'T',-9
                     , '#',128,__Vtask_test_fspi_control__DOT__expect_byte__195__label.data()
                     , '#',8,(IData)(__Vtask_test_fspi_control__DOT__expect_byte__195__actual)
                     , '#',8,__Vtask_test_fspi_control__DOT__expect_byte__195__expected
                     , '#',64,VL_TIME_UNITED_Q(1000));
        VL_STOP_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 70, "", false);
    }
    VL_WRITEF_NX("PASS: quad SPI writes update all five control registers\n",0);
    VL_FINISH_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 110, "");
    co_return;
}

VlCoroutine Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__1(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_initial__TOP__Vtiming__1\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    while (VL_LIKELY(!vlSymsp->_vm_contextp__->gotFinish())) {
        co_await vlSelfRef.__VdlySched.delay(0x00000000000004e2ULL, 
                                             nullptr, 
                                             "fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 
                                             4);
        vlSelfRef.test_fspi_control__DOT__wr_clk = 
            (1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__wr_clk)));
    }
    co_return;
}

void Vtest_fspi_control___024root___eval_triggers_vec__act(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_triggers_vec__act\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered[0U] = (QData)((IData)(
                                                    (((vlSelfRef.__VdlySched.awaitingCurrentTime() 
                                                       << 5U) 
                                                      | (((~ (IData)(vlSelfRef.test_fspi_control__DOT__wr_clk)) 
                                                          & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__wr_clk__0)) 
                                                         << 4U)) 
                                                     | (((((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_clk)) 
                                                           & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_clk__0)) 
                                                          << 3U) 
                                                         | (((IData)(vlSelfRef.test_fspi_control__DOT__spi_cs) 
                                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_cs__0))) 
                                                            << 2U)) 
                                                        | ((((IData)(vlSelfRef.test_fspi_control__DOT__spi_clk) 
                                                             & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_clk__0))) 
                                                            << 1U) 
                                                           | ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
                                                              & (IData)(vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__rst_n__0)))))));
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__rst_n__0 
        = vlSelfRef.test_fspi_control__DOT__rst_n;
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_clk__0 
        = vlSelfRef.test_fspi_control__DOT__spi_clk;
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__spi_cs__0 
        = vlSelfRef.test_fspi_control__DOT__spi_cs;
    vlSelfRef.__Vtrigprevexpr___TOP__test_fspi_control__DOT__wr_clk__0 
        = vlSelfRef.test_fspi_control__DOT__wr_clk;
}

bool Vtest_fspi_control___024root___trigger_anySet__act(const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___trigger_anySet__act\n"); );
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

void Vtest_fspi_control___024root___act_sequent__TOP__0(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___act_sequent__TOP__0\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
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

void Vtest_fspi_control___024root___eval_act(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_act\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
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

void Vtest_fspi_control___024root___nba_sequent__TOP__0(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__0\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag 
        = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag;
}

void Vtest_fspi_control___024root___nba_sequent__TOP__1(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__1\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count 
        = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count;
    vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock 
        = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_lock;
}

void Vtest_fspi_control___024root___nba_sequent__TOP__2(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__2\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg 
        = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg;
    if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg = 0U;
    } else if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        if ((2U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg))) {
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg 
                = (1U & ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg)
                          ? (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg))
                          : (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg))));
        }
    }
}

void Vtest_fspi_control___024root___nba_sequent__TOP__3(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__3\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started;
    __Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started = 0;
    // Body
    __Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started 
        = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started;
    if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        __Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started = 0U;
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble = 0x0fU;
    } else if (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_ready) {
        if (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started) {
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble 
                = (0x0000000fU & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data));
        } else {
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble 
                = (0x0000000fU & ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data) 
                                  >> 4U));
            __Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started = 1U;
        }
    }
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started 
        = __Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started;
}

void Vtest_fspi_control___024root___nba_sequent__TOP__4(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__4\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*2:0*/ __Vdly__test_fspi_control__DOT__u_regs__DOT__state;
    __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 0;
    CData/*1:0*/ __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt;
    __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 0;
    CData/*7:0*/ __VdlyVal__test_fspi_control__DOT__u_regs__DOT__mem__v0;
    __VdlyVal__test_fspi_control__DOT__u_regs__DOT__mem__v0 = 0;
    CData/*2:0*/ __VdlyDim0__test_fspi_control__DOT__u_regs__DOT__mem__v0;
    __VdlyDim0__test_fspi_control__DOT__u_regs__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v0;
    __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v0 = 0;
    CData/*0:0*/ __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v1;
    __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v1 = 0;
    // Body
    __Vdly__test_fspi_control__DOT__u_regs__DOT__state 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state;
    __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt 
        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__byte_cnt;
    __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v0 = 0U;
    __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v1 = 0U;
    if (vlSelfRef.test_fspi_control__DOT__rst_n) {
        if (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_wr_en) 
             & (5U > (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_addr)))) {
            vlSelfRef.test_fspi_control__DOT__u_regs__DOT____Vlvbound_h11419250__0 
                = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg;
            if ((4U >= (7U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_addr)))) {
                __VdlyVal__test_fspi_control__DOT__u_regs__DOT__mem__v0 
                    = vlSelfRef.test_fspi_control__DOT__u_regs__DOT____Vlvbound_h11419250__0;
                __VdlyDim0__test_fspi_control__DOT__u_regs__DOT__mem__v0 
                    = (7U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_addr));
                __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v0 = 1U;
            }
        }
        if (vlSelfRef.test_fspi_control__DOT__u_regs__DOT__spi_cs_reg) {
            __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 0U;
            vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_wr_en = 0U;
            __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 0U;
            vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg = 0xffU;
        } else {
            if ((4U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state))) {
                if ((2U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state))) {
                    if ((1U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state))) {
                        __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 0U;
                    } else if (((~ (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__tx_done_d0)) 
                                & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__tx_done_d1))) {
                        __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 0U;
                    }
                } else if ((1U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state))) {
                    if ((2U == (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__byte_cnt))) {
                        __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 6U;
                    }
                } else if (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose) 
                            & (1U == (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__byte_cnt)))) {
                    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_addr 
                        = (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__read_addr_high) 
                            << 8U) | (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg));
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 2U;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 5U;
                }
            } else if ((2U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state))) {
                if ((1U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state))) {
                    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_wr_en = 0U;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 0U;
                } else if (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose) 
                            & (2U == (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__byte_cnt)))) {
                    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_wr_en = 1U;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 3U;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 3U;
                }
            } else if ((1U & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state))) {
                if (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose) 
                     & (1U == (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__byte_cnt)))) {
                    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_addr 
                        = (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__addr_high) 
                            << 8U) | (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg));
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 2U;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 2U;
                }
            } else {
                vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_wr_en = 0U;
                __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 0U;
                if (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose) 
                     & (1U == (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__cmd_reg)))) {
                    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__addr_high 
                        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 1U;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 1U;
                } else if (((IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose) 
                            & (2U == (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__cmd_reg)))) {
                    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__read_addr_high 
                        = vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 1U;
                    __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 4U;
                }
            }
            vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg 
                = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data;
        }
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__cmd_reg 
            = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg;
    } else {
        __VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v1 = 1U;
        __Vdly__test_fspi_control__DOT__u_regs__DOT__state = 0U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_addr = 0U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__ram_wr_en = 0U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__addr_high = 0U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__read_addr_high = 0U;
        __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt = 0U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__wr_data_reg = 0U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__cmd_reg = 0U;
    }
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d1 
        = ((IData)(vlSelfRef.test_fspi_control__DOT__rst_n) 
           && (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d0));
    if (__VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v0) {
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[__VdlyDim0__test_fspi_control__DOT__u_regs__DOT__mem__v0] 
            = __VdlyVal__test_fspi_control__DOT__u_regs__DOT__mem__v0;
    }
    if (__VdlySet__test_fspi_control__DOT__u_regs__DOT__mem__v1) {
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[0U] = 1U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[1U] = 1U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[2U] = 1U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[3U] = 1U;
        vlSelfRef.test_fspi_control__DOT__u_regs__DOT__mem[4U] = 1U;
    }
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d0 
        = ((IData)(vlSelfRef.test_fspi_control__DOT__rst_n) 
           && (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_valid));
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__state 
        = __Vdly__test_fspi_control__DOT__u_regs__DOT__state;
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__byte_cnt 
        = __Vdly__test_fspi_control__DOT__u_regs__DOT__byte_cnt;
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_pose 
        = ((~ (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d1)) 
           & (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__rx_valid_d0));
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__tx_done_d1 
        = ((IData)(vlSelfRef.test_fspi_control__DOT__rst_n) 
           && (IData)(vlSelfRef.test_fspi_control__DOT__u_regs__DOT__tx_done_d0));
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__tx_done_d0 
        = ((IData)(vlSelfRef.test_fspi_control__DOT__rst_n) 
           && (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg));
    vlSelfRef.test_fspi_control__DOT__u_regs__DOT__spi_cs_reg 
        = ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n))) 
           || (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs));
}

void Vtest_fspi_control___024root___nba_sequent__TOP__5(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__5\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_header = 0U;
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count = 0U;
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_ready = 0U;
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data = 0xffU;
    } else if ((6U > (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count))) {
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count 
            = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count)));
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_header 
            = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header;
        if ((IData)(((5U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count)) 
                     & (0x00810000U == (0x00ff0000U 
                                        & vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header))))) {
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_ready = 1U;
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data 
                = ((5U == (0x0000ffffU & vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header))
                    ? ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_sync)
                        ? 0xa1U : 0xa0U) : 0xffU);
        }
    }
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count 
        = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count;
}

void Vtest_fspi_control___024root___nba_comb__TOP__0(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_comb__TOP__0\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = ((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                                                & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started));
}

void Vtest_fspi_control___024root___nba_sequent__TOP__6(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__6\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_valid = 0U;
    } else if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_valid 
            = ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag) 
               & (1U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg)));
    }
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_sync 
        = ((IData)(vlSelfRef.test_fspi_control__DOT__rst_n) 
           && (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_meta));
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_meta = 0U;
}

void Vtest_fspi_control___024root___nba_sequent__TOP__7(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__7\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg 
        = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg;
}

void Vtest_fspi_control___024root___nba_sequent__TOP__8(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__8\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock = 0U;
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg = 0U;
    } else if (((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag))) {
        if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_lock)))) {
            if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                           | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data)))) {
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg = 2U;
            } else if ((0U == ((0x000000f0U & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data)))) {
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg = 1U;
            }
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock = 1U;
        }
    }
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_lock 
        = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock;
}

void Vtest_fspi_control___024root___nba_sequent__TOP__9(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_sequent__TOP__9\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag = 0U;
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data = 0xffU;
    } else if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
        if (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag) {
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag)));
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data 
                = ((0xf0U & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data));
        } else {
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag 
                = (1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag)));
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data 
                = ((0x0fU & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                   | ((IData)(vlSelfRef.test_fspi_control__DOT__spi_data) 
                      << 4U));
        }
    }
    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag 
        = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag;
}

void Vtest_fspi_control___024root___nba_comb__TOP__1(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_comb__TOP__1\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = (0x0000000fU 
                                                & (- (IData)(
                                                             ((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                                                              & (2U 
                                                                 == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg))))));
}

void Vtest_fspi_control___024root___nba_comb__TOP__2(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___nba_comb__TOP__2\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
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

void Vtest_fspi_control___024root___eval_nba(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_nba\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __Vinline__nba_sequent__TOP__3___Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started;
    __Vinline__nba_sequent__TOP__3___Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started = 0;
    // Body
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag 
            = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag;
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count 
            = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count;
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock 
            = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_lock;
    }
    if ((9ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg 
            = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg;
        if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg = 0U;
        } else if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            if ((2U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg))) {
                vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg 
                    = (1U & ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg)
                              ? (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg))
                              : (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg))));
            }
        }
    }
    if ((0x000000000000000dULL & vlSelfRef.__VnbaTriggered[0U])) {
        __Vinline__nba_sequent__TOP__3___Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started 
            = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started;
        if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            __Vinline__nba_sequent__TOP__3___Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started = 0U;
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble = 0x0fU;
        } else if (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_ready) {
            if (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started) {
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble 
                    = (0x0000000fU & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data));
            } else {
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_nibble 
                    = (0x0000000fU & ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data) 
                                      >> 4U));
                __Vinline__nba_sequent__TOP__3___Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started = 1U;
            }
        }
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started 
            = __Vinline__nba_sequent__TOP__3___Vdly__test_fspi_control__DOT__u_slave__DOT__status_tx_started;
    }
    if ((0x0000000000000011ULL & vlSelfRef.__VnbaTriggered[0U])) {
        Vtest_fspi_control___024root___nba_sequent__TOP__4(vlSelf);
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_header = 0U;
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count = 0U;
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_ready = 0U;
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data = 0xffU;
        } else if ((6U > (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count))) {
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count 
                = (7U & ((IData)(1U) + (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count)));
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_header 
                = vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header;
            if ((IData)(((5U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count)) 
                         & (0x00810000U == (0x00ff0000U 
                                            & vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header))))) {
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_ready = 1U;
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_read_data 
                    = ((5U == (0x0000ffffU & vlSelfRef.test_fspi_control__DOT__u_slave__DOT__next_status_header))
                        ? ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_sync)
                            ? 0xa1U : 0xa0U) : 0xffU);
            }
        }
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_nibble_count 
            = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__status_nibble_count;
    }
    if ((0x000000000000002dULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_1 = 
            ((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
             & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__status_tx_started));
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_valid = 0U;
        } else if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_valid 
                = ((IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag) 
                   & (1U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg)));
        }
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_sync 
            = ((IData)(vlSelfRef.test_fspi_control__DOT__rst_n) 
               && (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_meta));
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__auto_enable_meta = 0U;
    }
    if ((9ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__tx_valid_reg 
            = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__tx_valid_reg;
    }
    if ((7ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock = 0U;
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg = 0U;
        } else if (((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                    & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag))) {
            if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_lock)))) {
                if ((0x80U == ((0x000000f0U & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                               | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data)))) {
                    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg = 2U;
                } else if ((0U == ((0x000000f0U & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data)))) {
                    vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg = 1U;
                }
                vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock = 1U;
            }
        }
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_lock 
            = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__cmd_lock;
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered[0U])) {
        if ((1U & ((~ (IData)(vlSelfRef.test_fspi_control__DOT__rst_n)) 
                   | (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag = 0U;
            vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data = 0xffU;
        } else if ((1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)))) {
            if (vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag) {
                vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag 
                    = (1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag)));
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data 
                    = ((0xf0U & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                       | (IData)(vlSelfRef.test_fspi_control__DOT__spi_data));
            } else {
                vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag 
                    = (1U & (~ (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag)));
                vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data 
                    = ((0x0fU & (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_data)) 
                       | ((IData)(vlSelfRef.test_fspi_control__DOT__spi_data) 
                          << 4U));
            }
        }
        vlSelfRef.test_fspi_control__DOT__u_slave__DOT__recv_flag 
            = vlSelfRef.__Vdly__test_fspi_control__DOT__u_slave__DOT__recv_flag;
    }
    if ((0x0000000000000027ULL & vlSelfRef.__VnbaTriggered[0U])) {
        vlSelfRef.__VdfgRegularize_h6e95ff9d_0_2 = 
            (0x0000000fU & (- (IData)(((~ (IData)(vlSelfRef.test_fspi_control__DOT__spi_cs)) 
                                       & (2U == (IData)(vlSelfRef.test_fspi_control__DOT__u_slave__DOT__cmd_reg))))));
    }
    if ((0x000000000000002fULL & vlSelfRef.__VnbaTriggered[0U])) {
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

void Vtest_fspi_control___024root___timing_resume(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___timing_resume\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((0x0000000000000020ULL & vlSelfRef.__VactTriggered[0U])) {
        vlSelfRef.__VdlySched.resume();
    }
}

void Vtest_fspi_control___024root___trigger_orInto__act_vec_vec(VlUnpacked<QData/*63:0*/, 1> &out, const VlUnpacked<QData/*63:0*/, 1> &in) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___trigger_orInto__act_vec_vec\n"); );
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
VL_ATTR_COLD void Vtest_fspi_control___024root___dump_triggers__act(const VlUnpacked<QData/*63:0*/, 1> &triggers, const std::string &tag);
#endif  // VL_DEBUG

bool Vtest_fspi_control___024root___eval_phase__act(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_phase__act\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VactExecute;
    // Body
    Vtest_fspi_control___024root___eval_triggers_vec__act(vlSelf);
    Vtest_fspi_control___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VactTriggered, vlSelfRef.__VactTriggeredAcc);
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vtest_fspi_control___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
    }
#endif
    Vtest_fspi_control___024root___trigger_orInto__act_vec_vec(vlSelfRef.__VnbaTriggered, vlSelfRef.__VactTriggered);
    __VactExecute = Vtest_fspi_control___024root___trigger_anySet__act(vlSelfRef.__VactTriggered);
    if (__VactExecute) {
        vlSelfRef.__VactTriggeredAcc.fill(0ULL);
        Vtest_fspi_control___024root___timing_resume(vlSelf);
        Vtest_fspi_control___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtest_fspi_control___024root___eval_phase__inact(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_phase__inact\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VinactExecute;
    // Body
    __VinactExecute = vlSelfRef.__VdlySched.awaitingZeroDelay();
    if (__VinactExecute) {
        VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 2, "", "ZERODLY: Design Verilated with '--no-sched-zero-delay', but #0 delay executed at runtime");
    }
    return (__VinactExecute);
}

void Vtest_fspi_control___024root___trigger_clear__act(VlUnpacked<QData/*63:0*/, 1> &out) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___trigger_clear__act\n"); );
    // Locals
    IData/*31:0*/ n;
    // Body
    n = 0U;
    do {
        out[n] = 0ULL;
        n = ((IData)(1U) + n);
    } while ((1U > n));
}

bool Vtest_fspi_control___024root___eval_phase__nba(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_phase__nba\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = Vtest_fspi_control___024root___trigger_anySet__act(vlSelfRef.__VnbaTriggered);
    if (__VnbaExecute) {
        Vtest_fspi_control___024root___eval_nba(vlSelf);
        Vtest_fspi_control___024root___trigger_clear__act(vlSelfRef.__VnbaTriggered);
    }
    return (__VnbaExecute);
}

void Vtest_fspi_control___024root___eval(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Locals
    IData/*31:0*/ __VnbaIterCount;
    // Body
    __VnbaIterCount = 0U;
    do {
        if (VL_UNLIKELY(((0x00002710U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vtest_fspi_control___024root___dump_triggers__act(vlSelfRef.__VnbaTriggered, "nba"s);
#endif
            VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 2, "", "DIDNOTCONVERGE: NBA region did not converge after '--converge-limit' of 10000 tries");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        vlSelfRef.__VinactIterCount = 0U;
        do {
            if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VinactIterCount)))) {
                VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 2, "", "DIDNOTCONVERGE: Inactive region did not converge after '--converge-limit' of 10000 tries");
            }
            vlSelfRef.__VinactIterCount = ((IData)(1U) 
                                           + vlSelfRef.__VinactIterCount);
            vlSelfRef.__VactIterCount = 0U;
            do {
                if (VL_UNLIKELY(((0x00002710U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                    Vtest_fspi_control___024root___dump_triggers__act(vlSelfRef.__VactTriggered, "act"s);
#endif
                    VL_FATAL_MT("fpga/fpga_pcie_ov5640/sim/test_fspi_control.v", 2, "", "DIDNOTCONVERGE: Active region did not converge after '--converge-limit' of 10000 tries");
                }
                vlSelfRef.__VactIterCount = ((IData)(1U) 
                                             + vlSelfRef.__VactIterCount);
                vlSelfRef.__VactPhaseResult = Vtest_fspi_control___024root___eval_phase__act(vlSelf);
            } while (vlSelfRef.__VactPhaseResult);
            vlSelfRef.__VinactPhaseResult = Vtest_fspi_control___024root___eval_phase__inact(vlSelf);
        } while (vlSelfRef.__VinactPhaseResult);
        vlSelfRef.__VnbaPhaseResult = Vtest_fspi_control___024root___eval_phase__nba(vlSelf);
    } while (vlSelfRef.__VnbaPhaseResult);
}

#ifdef VL_DEBUG
void Vtest_fspi_control___024root___eval_debug_assertions(Vtest_fspi_control___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtest_fspi_control___024root___eval_debug_assertions\n"); );
    Vtest_fspi_control__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}
#endif  // VL_DEBUG
