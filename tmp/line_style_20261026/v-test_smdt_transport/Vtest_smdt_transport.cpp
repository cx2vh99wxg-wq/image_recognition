// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_smdt_transport__pch.h"

//============================================================
// Constructors

Vtest_smdt_transport::Vtest_smdt_transport(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_smdt_transport__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_smdt_transport::Vtest_smdt_transport(const char* _vcname__)
    : Vtest_smdt_transport(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_smdt_transport::~Vtest_smdt_transport() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_smdt_transport___024root___eval_debug_assertions(Vtest_smdt_transport___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_smdt_transport___024root___eval_static(Vtest_smdt_transport___024root* vlSelf);
void Vtest_smdt_transport___024root___eval_initial(Vtest_smdt_transport___024root* vlSelf);
void Vtest_smdt_transport___024root___eval_settle(Vtest_smdt_transport___024root* vlSelf);
void Vtest_smdt_transport___024root___eval(Vtest_smdt_transport___024root* vlSelf);

void Vtest_smdt_transport::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_smdt_transport::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_smdt_transport___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_smdt_transport___024root___eval_static(&(vlSymsp->TOP));
        Vtest_smdt_transport___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_smdt_transport___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_smdt_transport___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_smdt_transport::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_smdt_transport::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_smdt_transport::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_smdt_transport___024root___eval_final(Vtest_smdt_transport___024root* vlSelf);

VL_ATTR_COLD void Vtest_smdt_transport::final() {
    contextp()->executingFinal(true);
    Vtest_smdt_transport___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_smdt_transport::hierName() const { return vlSymsp->name(); }
const char* Vtest_smdt_transport::modelName() const { return "Vtest_smdt_transport"; }
unsigned Vtest_smdt_transport::threads() const { return 1; }
void Vtest_smdt_transport::prepareClone() const { contextp()->prepareClone(); }
void Vtest_smdt_transport::atClone() const {
    contextp()->threadPoolpOnClone();
}
