// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_direction_arbiter__pch.h"

//============================================================
// Constructors

Vtest_direction_arbiter::Vtest_direction_arbiter(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_direction_arbiter__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_direction_arbiter::Vtest_direction_arbiter(const char* _vcname__)
    : Vtest_direction_arbiter(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_direction_arbiter::~Vtest_direction_arbiter() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_direction_arbiter___024root___eval_debug_assertions(Vtest_direction_arbiter___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_direction_arbiter___024root___eval_static(Vtest_direction_arbiter___024root* vlSelf);
void Vtest_direction_arbiter___024root___eval_initial(Vtest_direction_arbiter___024root* vlSelf);
void Vtest_direction_arbiter___024root___eval_settle(Vtest_direction_arbiter___024root* vlSelf);
void Vtest_direction_arbiter___024root___eval(Vtest_direction_arbiter___024root* vlSelf);

void Vtest_direction_arbiter::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_direction_arbiter::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_direction_arbiter___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_direction_arbiter___024root___eval_static(&(vlSymsp->TOP));
        Vtest_direction_arbiter___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_direction_arbiter___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_direction_arbiter___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_direction_arbiter::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_direction_arbiter::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_direction_arbiter::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_direction_arbiter___024root___eval_final(Vtest_direction_arbiter___024root* vlSelf);

VL_ATTR_COLD void Vtest_direction_arbiter::final() {
    contextp()->executingFinal(true);
    Vtest_direction_arbiter___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_direction_arbiter::hierName() const { return vlSymsp->name(); }
const char* Vtest_direction_arbiter::modelName() const { return "Vtest_direction_arbiter"; }
unsigned Vtest_direction_arbiter::threads() const { return 1; }
void Vtest_direction_arbiter::prepareClone() const { contextp()->prepareClone(); }
void Vtest_direction_arbiter::atClone() const {
    contextp()->threadPoolpOnClone();
}
