// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtest_steering_equivalence.h for the primary calling header

#ifndef VERILATED_VTEST_STEERING_EQUIVALENCE___024ROOT_H_
#define VERILATED_VTEST_STEERING_EQUIVALENCE___024ROOT_H_  // guard

#include "verilated.h"
#include "verilated_timing.h"


class Vtest_steering_equivalence__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtest_steering_equivalence___024root final {
  public:

    // DESIGN SPECIFIC STATE
    CData/*0:0*/ test_steering_equivalence__DOT__clk;
    CData/*0:0*/ test_steering_equivalence__DOT__rst_n;
    CData/*3:0*/ test_steering_equivalence__DOT__hmi_code;
    CData/*3:0*/ test_steering_equivalence__DOT__fspi_released;
    CData/*0:0*/ test_steering_equivalence__DOT__armed;
    CData/*0:0*/ test_steering_equivalence__DOT__blocked;
    CData/*0:0*/ test_steering_equivalence__DOT__old_l;
    CData/*0:0*/ test_steering_equivalence__DOT__old_r;
    CData/*0:0*/ test_steering_equivalence__DOT__new_l;
    CData/*0:0*/ test_steering_equivalence__DOT__new_r;
    CData/*0:0*/ test_steering_equivalence__DOT__new_go;
    CData/*0:0*/ test_steering_equivalence__DOT__new_back;
    CData/*0:0*/ test_steering_equivalence__DOT__previous__DOT__hmi_traction;
    CData/*0:0*/ test_steering_equivalence__DOT__previous__DOT__hmi_steering;
    CData/*0:0*/ test_steering_equivalence__DOT__previous__DOT__hmi_steering_hold;
    CData/*1:0*/ test_steering_equivalence__DOT__current__DOT__hmi_traction_latched;
    CData/*3:0*/ test_steering_equivalence__DOT__current__DOT__previous_hmi_code;
    CData/*2:0*/ test_steering_equivalence__DOT__current__DOT__traction_stable_count;
    CData/*0:0*/ test_steering_equivalence__DOT__current__DOT__traction_press_consumed;
    CData/*0:0*/ test_steering_equivalence__DOT__current__DOT__hmi_code_changed;
    CData/*0:0*/ test_steering_equivalence__DOT__current__DOT__hmi_traction_hold;
    CData/*0:0*/ test_steering_equivalence__DOT__current__DOT__hmi_steering_hold;
    CData/*0:0*/ test_steering_equivalence__DOT__current__DOT__hmi_owns_traction;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_1;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_3;
    CData/*0:0*/ __VdfgRegularize_h6e95ff9d_0_4;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VstlPhaseResult;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_steering_equivalence__DOT__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__test_steering_equivalence__DOT__rst_n__0;
    CData/*0:0*/ __VactPhaseResult;
    CData/*0:0*/ __VinactPhaseResult;
    CData/*0:0*/ __VnbaPhaseResult;
    IData/*31:0*/ test_steering_equivalence__DOT__checks;
    IData/*31:0*/ test_steering_equivalence__DOT__hold_cycles;
    IData/*31:0*/ test_steering_equivalence__DOT__rng;
    IData/*31:0*/ __VactIterCount;
    IData/*31:0*/ __VinactIterCount;
    IData/*31:0*/ __Vi;
    VlUnpacked<QData/*63:0*/, 1> __VstlTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggered;
    VlUnpacked<QData/*63:0*/, 1> __VactTriggeredAcc;
    VlUnpacked<QData/*63:0*/, 1> __VnbaTriggered;
    VlDelayScheduler __VdlySched;
    VlTriggerScheduler __VtrigSched_h931b9c32__0;
    VlTriggerScheduler __VtrigSched_h931b9f78__0;

    // INTERNAL VARIABLES
    Vtest_steering_equivalence__Syms* vlSymsp;
    const char* vlNamep;

    // CONSTRUCTORS
    Vtest_steering_equivalence___024root(Vtest_steering_equivalence__Syms* symsp, const char* namep);
    ~Vtest_steering_equivalence___024root();
    VL_UNCOPYABLE(Vtest_steering_equivalence___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
