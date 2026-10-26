// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_smdt_prefix__pch.h"

//============================================================
// Constructors

Vtest_smdt_prefix::Vtest_smdt_prefix(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_smdt_prefix__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_smdt_prefix::Vtest_smdt_prefix(const char* _vcname__)
    : Vtest_smdt_prefix(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_smdt_prefix::~Vtest_smdt_prefix() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_smdt_prefix___024root___eval_debug_assertions(Vtest_smdt_prefix___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_smdt_prefix___024root___eval_static(Vtest_smdt_prefix___024root* vlSelf);
void Vtest_smdt_prefix___024root___eval_initial(Vtest_smdt_prefix___024root* vlSelf);
void Vtest_smdt_prefix___024root___eval_settle(Vtest_smdt_prefix___024root* vlSelf);
void Vtest_smdt_prefix___024root___eval(Vtest_smdt_prefix___024root* vlSelf);

void Vtest_smdt_prefix::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_smdt_prefix::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_smdt_prefix___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_smdt_prefix___024root___eval_static(&(vlSymsp->TOP));
        Vtest_smdt_prefix___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_smdt_prefix___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_smdt_prefix___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_smdt_prefix::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_smdt_prefix::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_smdt_prefix::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_smdt_prefix___024root___eval_final(Vtest_smdt_prefix___024root* vlSelf);

VL_ATTR_COLD void Vtest_smdt_prefix::final() {
    contextp()->executingFinal(true);
    Vtest_smdt_prefix___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_smdt_prefix::hierName() const { return vlSymsp->name(); }
const char* Vtest_smdt_prefix::modelName() const { return "Vtest_smdt_prefix"; }
unsigned Vtest_smdt_prefix::threads() const { return 1; }
void Vtest_smdt_prefix::prepareClone() const { contextp()->prepareClone(); }
void Vtest_smdt_prefix::atClone() const {
    contextp()->threadPoolpOnClone();
}
