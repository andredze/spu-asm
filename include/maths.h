#ifndef MATHS_H
#define MATHS_H

#include "stack.h"
#include "input.h"
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

MathErr_t ApplyBinaryOperation(Stack_t* stack,
                               MathErr_t (* calculate) (CalcData_t* calc_data));

MathErr_t Add(CalcData_t* calc_data);

MathErr_t Sub(CalcData_t* calc_data);

MathErr_t Mul(CalcData_t* calc_data);

MathErr_t Div(CalcData_t* calc_data);

MathErr_t Sqrt(Stack_t* stack);

MathErr_t Out(Stack_t* stack);

#endif /* MATHS_H */
