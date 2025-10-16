#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "stack.h"
#include "input.h"
#include "spu_types.h"
#include <math.h>

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

int HandleJB(Proc_t* proc_data, Stack_t* stack, int new_cmd_count);

int HandleJBE(Proc_t* proc_data, Stack_t* stack, int new_cmd_count);

int HandleJA(Proc_t* proc_data, Stack_t* stack, int new_cmd_count);

int HandleJAE(Proc_t* proc_data, Stack_t* stack, int new_cmd_count);

int HandleJE(Proc_t* proc_data, Stack_t* stack, int new_cmd_count);

int HandleJNE(Proc_t* proc_data, Stack_t* stack, int new_cmd_count);

MathErr_t ExecuteBinaryOperation(Stack_t* stack,
                               MathErr_t (* calculate) (CalcData_t* calc_data));

MathErr_t Add(CalcData_t* calc_data);

MathErr_t Sub(CalcData_t* calc_data);

MathErr_t Mul(CalcData_t* calc_data);

MathErr_t Div(CalcData_t* calc_data);

MathErr_t HandleSqrt(Stack_t* stack);

int HandleOut(Stack_t* stack, FILE* output_stream);

int HandlePopr(Stack_t* stack, Proc_t* proc_data, int index);

int HandlePushr(Stack_t* stack, Proc_t* proc_data, int index);

int HandleIn(Stack_t* stack);

int HandleJmp(Proc_t* proc_data, size_t new_cmd_count);

int HandleCall(Proc_t* proc_data, int new_cmd_count);

int HandleRet(Proc_t* proc_data);

int HandlePushm(Proc_t* proc_data, int mem_addr);

int HandlePopm(Proc_t* proc_data, int mem_addr);

// int HandleJumpIf(Stack_t* stack, Proc_t* proc_data,
//                  size_t new_cmd_count, Command_t command);

// int CompareForJump(int number1, int number2, Command_t command);

#endif /* OPERATIONS_H */
