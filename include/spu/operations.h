#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "stack.h"
#include "input.h"
#include "spu_types.h"
#include <math.h>
#include <windows.h>

typedef enum MathErr {
    MATH_SUCCESS,
    MATH_DIVISION_BY_ZERO,
    MATH_STACK_ERROR,
} MathErr_t;

typedef struct CalcData {
    int number1;
    int number2;
    int result;
} CalcData_t;

MathErr_t Add(CalcData_t* calc_data);

MathErr_t Sub(CalcData_t* calc_data);

MathErr_t Mul(CalcData_t* calc_data);

MathErr_t Div(CalcData_t* calc_data);

MathErr_t Mod(CalcData_t* calc_data);

int Jump(Proc_t* proc_data, int new_cmd_count);

int HandlePush(Proc_t* proc_data);

int HandleOut(Proc_t* proc_data);

int HandleIn(Proc_t* proc_data);

int HandleCall(Proc_t* proc_data);

int HandleRet(Proc_t* proc_data);

int HandleAdd(Proc_t* proc_data);

int HandleSub(Proc_t* proc_data);

int HandleMul(Proc_t* proc_data);

int HandleDiv(Proc_t* proc_data);

int HandleMod(Proc_t* proc_data);

int HandleSqrt(Proc_t* proc_data);

int HandleJmp(Proc_t* proc_data);

int HandleJb(Proc_t* proc_data);

int HandleJbe(Proc_t* proc_data);

int HandleJa(Proc_t* proc_data);

int HandleJae(Proc_t* proc_data);

int HandleJe(Proc_t* proc_data);

int HandleJne(Proc_t* proc_data);

int HandlePushr(Proc_t* proc_data);

int HandlePopr(Proc_t* proc_data);

int HandlePushm(Proc_t* proc_data);

int HandlePopm(Proc_t* proc_data);

int HandleDraw(Proc_t* proc_data);

// int HandleJumpIf(Stack_t* stack, Proc_t* proc_data,
//                  size_t new_cmd_count, Command_t command);

// int CompareForJump(int number1, int number2, Command_t command);

#endif /* OPERATIONS_H */
