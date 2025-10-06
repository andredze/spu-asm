#include "maths.h"

// TODO: rename to commands (?) and connect w commands.h
MathErr_t ApplyBinaryOperation(Stack_t* stack,
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

MathErr_t Sqrt(Stack_t* stack)
{
    assert(stack != NULL);

    int number = 0;
    if (StackPop(stack, &number) != STACK_SUCCESS)
    {
        return MATH_STACK_ERROR;
    }

    int value = (int) sqrt((double) number);
    if (StackPush(stack, value) != STACK_SUCCESS)
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

    calc_data->result = calc_data->number2 - calc_data->number1;

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
    calc_data->result = calc_data->number2 / calc_data->number1;

    return MATH_SUCCESS;
}

int HandleOut(Stack_t* stack, FILE* output_stream)
{
    assert(stack != NULL);
    assert(output_stream != NULL);

    int result = 0;
    StackErr_t pop_return = StackPop(stack, &result);

    if (pop_return == STACK_SIZE_IS_ZERO)
    {
        return 0;
    }
    if (pop_return != STACK_SUCCESS)
    {
        return 1;
    }
    fprintf(output_stream, "ANSWER = %d\n", result);

    return 0;
}

int HandleTotr(Stack_t* stack, Proc_t* proc_data, int index)
{
    assert(stack != NULL);
    assert(proc_data != NULL);

    int value = 0;
    StackErr_t pop_return = StackPop(stack, &value);

    if (pop_return == STACK_SIZE_IS_ZERO)
    {
        return 0;
    }
    if (pop_return != STACK_SUCCESS)
    {
        return 1;
    }

    if (index < 0 || index > 7)
    {
        DPRINTF("Invalid register index given\n");
        return 1;
    }
    proc_data->regs[index] = value;

    return 0;
}

int HandlePushr(Stack_t* stack, Proc_t* proc_data, int index)
{
    assert(stack != NULL);
    assert(proc_data != NULL);

    if (index < 0 || index > 7)
    {
        DPRINTF("Invalid register index given\n");
        return 1;
    }
    if (StackPush(stack, proc_data->regs[index]) != STACK_SUCCESS)
    {
        return 1;
    }

    return 0;
}

int HandleIn(Stack_t* stack)
{
    assert(stack != NULL);

    int value = 0;
    printf("Enter int value: ");
    if (scanf("%d", &value) != 1)
    {
        printf("Input is wrong, try better :)\n");
        return 1;
    }
    if (StackPush(stack, value) != STACK_SUCCESS)
    {
        return 1;
    }

    return 0;
}
