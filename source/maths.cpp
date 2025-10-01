#include "maths.h"

MathErr_t ApplyMathOperation(Stack_t* stack,
                             MathErr_t (* calculate) (CalcData_t* calc_data))
{
    assert(stack != NULL);
    assert(stack->data != NULL);
    assert(calculate != NULL);
    assert(stack->size >= 2);

    CalcData_t calc_data = {};

    if (StackPop(stack, &calc_data.number1) != STACK_SUCCESS)
    {
        return MATH_STACK_ERROR;
    }
    if (StackPop(stack, &calc_data.number2) != STACK_SUCCESS)
    {
        return MATH_STACK_ERROR;
    }

    MathErr_t error = MATH_SUCCESS;
    if ((error = calculate(&calc_data)) != MATH_SUCCESS)
    {
        return error;
    }
    if (StackPush(stack, calc_data.result) != STACK_SUCCESS)
    {
        return MATH_STACK_ERROR;
    }

    return MATH_SUCCESS;
}

MathErr_t Add(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    calc_data->result = calc_data->number1 + calc_data->number2;

    return MATH_SUCCESS;
}

MathErr_t Sub(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    calc_data->result = calc_data->number1 - calc_data->number2;

    return MATH_SUCCESS;
}

MathErr_t Mul(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    calc_data->result = calc_data->number1 * calc_data->number2;

    return MATH_SUCCESS;
}

MathErr_t Div(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    if (calc_data->number2 == 0)
    {
        printf("Can not divide by zero\n");
        return MATH_DIVISION_BY_ZERO;
    }
    calc_data->result = calc_data->number1 / calc_data->number2;

    return MATH_SUCCESS;
}

MathErr_t Out(Stack_t* calc_stack)
{
    int answer = 0;

    if (StackPop(calc_stack, &answer) != STACK_SUCCESS)
    {
        return MATH_STACK_ERROR;
    }
    printf("ANSWER = %d", answer);

    return MATH_SUCCESS;
}
