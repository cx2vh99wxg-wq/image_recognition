// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_lane_permission__pch.h"

//============================================================
// Constructors

Vtest_lane_permission::Vtest_lane_permission(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_lane_permission__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_lane_permission::Vtest_lane_permission(const char* _vcname__)
    : Vtest_lane_permission(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_lane_permission::~Vtest_lane_permission() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_lane_permission___024root___eval_debug_assertions(Vtest_lane_permission___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_lane_permission___024root___eval_static(Vtest_lane_permission___024root* vlSelf);
void Vtest_lane_permission___024root___eval_initial(Vtest_lane_permission___024root* vlSelf);
void Vtest_lane_permission___024root___eval_settle(Vtest_lane_permission___024root* vlSelf);
void Vtest_lane_permission___024root___eval(Vtest_lane_permission___024root* vlSelf);

void Vtest_lane_permission::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_lane_permission::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_lane_permission___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_lane_permission___024root___eval_static(&(vlSymsp->TOP));
        Vtest_lane_permission___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_lane_permission___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_lane_permission___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_lane_permission::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_lane_permission::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_lane_permission::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_lane_permission___024root___eval_final(Vtest_lane_permission___024root* vlSelf);

VL_ATTR_COLD void Vtest_lane_permission::final() {
    contextp()->executingFinal(true);
    Vtest_lane_permission___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_lane_permission::hierName() const { return vlSymsp->name(); }
const char* Vtest_lane_permission::modelName() const { return "Vtest_lane_permission"; }
unsigned Vtest_lane_permission::threads() const { return 1; }
void Vtest_lane_permission::prepareClone() const { contextp()->prepareClone(); }
void Vtest_lane_permission::atClone() const {
    contextp()->threadPoolpOnClone();
}
