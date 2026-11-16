// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtest_hmi_camera_view__pch.h"

//============================================================
// Constructors

Vtest_hmi_camera_view::Vtest_hmi_camera_view(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtest_hmi_camera_view__Syms(contextp(), _vcname__, this)}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtest_hmi_camera_view::Vtest_hmi_camera_view(const char* _vcname__)
    : Vtest_hmi_camera_view(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtest_hmi_camera_view::~Vtest_hmi_camera_view() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtest_hmi_camera_view___024root___eval_debug_assertions(Vtest_hmi_camera_view___024root* vlSelf);
#endif  // VL_DEBUG
void Vtest_hmi_camera_view___024root___eval_static(Vtest_hmi_camera_view___024root* vlSelf);
void Vtest_hmi_camera_view___024root___eval_initial(Vtest_hmi_camera_view___024root* vlSelf);
void Vtest_hmi_camera_view___024root___eval_settle(Vtest_hmi_camera_view___024root* vlSelf);
void Vtest_hmi_camera_view___024root___eval(Vtest_hmi_camera_view___024root* vlSelf);

void Vtest_hmi_camera_view::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtest_hmi_camera_view::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtest_hmi_camera_view___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtest_hmi_camera_view___024root___eval_static(&(vlSymsp->TOP));
        Vtest_hmi_camera_view___024root___eval_initial(&(vlSymsp->TOP));
        Vtest_hmi_camera_view___024root___eval_settle(&(vlSymsp->TOP));
        vlSymsp->__Vm_didInit = true;
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtest_hmi_camera_view___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtest_hmi_camera_view::eventsPending() { return !vlSymsp->TOP.__VdlySched.empty() && !contextp()->gotFinish(); }

uint64_t Vtest_hmi_camera_view::nextTimeSlot() { return vlSymsp->TOP.__VdlySched.nextTimeSlot(); }

//============================================================
// Utilities

const char* Vtest_hmi_camera_view::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtest_hmi_camera_view___024root___eval_final(Vtest_hmi_camera_view___024root* vlSelf);

VL_ATTR_COLD void Vtest_hmi_camera_view::final() {
    contextp()->executingFinal(true);
    Vtest_hmi_camera_view___024root___eval_final(&(vlSymsp->TOP));
    contextp()->executingFinal(false);
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtest_hmi_camera_view::hierName() const { return vlSymsp->name(); }
const char* Vtest_hmi_camera_view::modelName() const { return "Vtest_hmi_camera_view"; }
unsigned Vtest_hmi_camera_view::threads() const { return 1; }
void Vtest_hmi_camera_view::prepareClone() const { contextp()->prepareClone(); }
void Vtest_hmi_camera_view::atClone() const {
    contextp()->threadPoolpOnClone();
}
