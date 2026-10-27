// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_remote_stop_controller__pch.h"

//============================================================
// Constructors

Vtest_remote_stop_controller::Vtest_remote_stop_controller(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_remote_stop_controller__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_remote_stop_controller::Vtest_remote_stop_controller(const char* _vcname__)
    : Vtest_remote_stop_controller(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_remote_stop_controller::~Vtest_remote_stop_controller() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_remote_stop_controller___024root___eval_debug_assertions(Vtest_remote_stop_controller___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_remote_stop_controller___024root___eval_static(Vtest_remote_stop_controller___024root* vlSelf);
void Vtest_remote_stop_controller___024root___eval_initial(Vtest_remote_stop_controller___024root* vlSelf);
void Vtest_remote_stop_controller___024root___eval_settle(Vtest_remote_stop_controller___024root* vlSelf);
void Vtest_remote_stop_controller___024root___eval(Vtest_remote_stop_controller___024root* vlSelf);

void Vtest_remote_stop_controller::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_remote_stop_controller::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_remote_stop_controller___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_remote_stop_controller___024root___eval_static(&(vlSymsp->TOP));
        Vtest_remote_stop_controller___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_remote_stop_controller___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_remote_stop_controller___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_remote_stop_controller::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_remote_stop_controller::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_remote_stop_controller::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_remote_stop_controller___024root___eval_final(Vtest_remote_stop_controller___024root* vlSelf);

VL_ATTR_COLD void Vtest_remote_stop_controller::final() {
    contextp()->executingFinal(true);
    Vtest_remote_stop_controller___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_remote_stop_controller::hierName() const { return vlSymsp->name(); }
const char* Vtest_remote_stop_controller::modelName() const { return "Vtest_remote_stop_controller"; }
unsigned Vtest_remote_stop_controller::threads() const { return 1; }
void Vtest_remote_stop_controller::prepareClone() const { contextp()->prepareClone(); }
void Vtest_remote_stop_controller::atClone() const {
    contextp()->threadPoolpOnClone();
}
