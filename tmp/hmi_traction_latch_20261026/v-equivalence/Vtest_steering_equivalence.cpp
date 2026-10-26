// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_steering_equivalence__pch.h"

//============================================================
// Constructors

Vtest_steering_equivalence::Vtest_steering_equivalence(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_steering_equivalence__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_steering_equivalence::Vtest_steering_equivalence(const char* _vcname__)
    : Vtest_steering_equivalence(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_steering_equivalence::~Vtest_steering_equivalence() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_steering_equivalence___024root___eval_debug_assertions(Vtest_steering_equivalence___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_steering_equivalence___024root___eval_static(Vtest_steering_equivalence___024root* vlSelf);
void Vtest_steering_equivalence___024root___eval_initial(Vtest_steering_equivalence___024root* vlSelf);
void Vtest_steering_equivalence___024root___eval_settle(Vtest_steering_equivalence___024root* vlSelf);
void Vtest_steering_equivalence___024root___eval(Vtest_steering_equivalence___024root* vlSelf);

void Vtest_steering_equivalence::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_steering_equivalence::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_steering_equivalence___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_steering_equivalence___024root___eval_static(&(vlSymsp->TOP));
        Vtest_steering_equivalence___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_steering_equivalence___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_steering_equivalence___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_steering_equivalence::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_steering_equivalence::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_steering_equivalence::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_steering_equivalence___024root___eval_final(Vtest_steering_equivalence___024root* vlSelf);

VL_ATTR_COLD void Vtest_steering_equivalence::final() {
    contextp()->executingFinal(true);
    Vtest_steering_equivalence___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_steering_equivalence::hierName() const { return vlSymsp->name(); }
const char* Vtest_steering_equivalence::modelName() const { return "Vtest_steering_equivalence"; }
unsigned Vtest_steering_equivalence::threads() const { return 1; }
void Vtest_steering_equivalence::prepareClone() const { contextp()->prepareClone(); }
void Vtest_steering_equivalence::atClone() const {
    contextp()->threadPoolpOnClone();
}
