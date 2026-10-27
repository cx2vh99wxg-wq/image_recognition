// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_fspi_control__pch.h"

//============================================================
// Constructors

Vtest_fspi_control::Vtest_fspi_control(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_fspi_control__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_fspi_control::Vtest_fspi_control(const char* _vcname__)
    : Vtest_fspi_control(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_fspi_control::~Vtest_fspi_control() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_fspi_control___024root___eval_debug_assertions(Vtest_fspi_control___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_fspi_control___024root___eval_static(Vtest_fspi_control___024root* vlSelf);
void Vtest_fspi_control___024root___eval_initial(Vtest_fspi_control___024root* vlSelf);
void Vtest_fspi_control___024root___eval_settle(Vtest_fspi_control___024root* vlSelf);
void Vtest_fspi_control___024root___eval(Vtest_fspi_control___024root* vlSelf);

void Vtest_fspi_control::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_fspi_control::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_fspi_control___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_fspi_control___024root___eval_static(&(vlSymsp->TOP));
        Vtest_fspi_control___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_fspi_control___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_fspi_control___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_fspi_control::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_fspi_control::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_fspi_control::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_fspi_control___024root___eval_final(Vtest_fspi_control___024root* vlSelf);

VL_ATTR_COLD void Vtest_fspi_control::final() {
    contextp()->executingFinal(true);
    Vtest_fspi_control___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_fspi_control::hierName() const { return vlSymsp->name(); }
const char* Vtest_fspi_control::modelName() const { return "Vtest_fspi_control"; }
unsigned Vtest_fspi_control::threads() const { return 1; }
void Vtest_fspi_control::prepareClone() const { contextp()->prepareClone(); }
void Vtest_fspi_control::atClone() const {
    contextp()->threadPoolpOnClone();
}
