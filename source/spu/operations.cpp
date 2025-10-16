#include "operations.h"

#define DECLARE_HANDLE_JUMP_IF(comp_oper, cmd_name) \
    int Handle##cmd_name(Proc_t* proc_data, Stack_t* stack, int new_cmd_count) \
    { \
        assert(proc_data != NULL); \
        assert(stack != NULL); \
        \
        int number1 = 0; \
        int number2 = 0; \
        if (StackPop(stack, &number2)) \
        { \
            return 1; \
        } \
        if (StackPop(stack, &number1)) \
        { \
            return 1; \
        } \
        DPRINTF("\t" #cmd_name ": %d " #comp_oper " %d == %d\n", \
                number1, number2, number1 comp_oper number2); \
        if (!(number1 comp_oper number2)) \
        { \
            DPRINTF("\tJMP rejected\n"); \
            return 0; \
        } \
        if (HandleJmp(proc_data, new_cmd_count)) \
        { \
            return 1; \
        } \
        return 0; \
    }

DECLARE_HANDLE_JUMP_IF(<,  JB);
DECLARE_HANDLE_JUMP_IF(<=, JBE);
DECLARE_HANDLE_JUMP_IF(>,  JA);
DECLARE_HANDLE_JUMP_IF(>=, JAE);
DECLARE_HANDLE_JUMP_IF(==, JE);
DECLARE_HANDLE_JUMP_IF(!=, JNE);

MathErr_t ExecuteBinaryOperation(Stack_t* stack,
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

MathErr_t HandleSqrt(Stack_t* stack)
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

    DPRINTF("\tSQRT: sqrt(%d) = %d\n", number, value);

    return MATH_SUCCESS;
}

MathErr_t Add(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    calc_data->result = calc_data->number1 + calc_data->number2;
    DPRINTF("\tADD: %d + %d = %d\n", calc_data->number2, calc_data->number1, calc_data->result);

    return MATH_SUCCESS;
}

MathErr_t Sub(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    calc_data->result = calc_data->number2 - calc_data->number1;
    DPRINTF("\tSUB: %d - %d = %d\n", calc_data->number2, calc_data->number1, calc_data->result);

    return MATH_SUCCESS;
}

MathErr_t Mul(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    calc_data->result = calc_data->number1 * calc_data->number2;
    DPRINTF("\tMUL: %d * %d = %d\n", calc_data->number2, calc_data->number1, calc_data->result);

    return MATH_SUCCESS;
}

MathErr_t Div(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    if (calc_data->number1 == 0)
    {
        printf("Can not divide by zero\n");
        return MATH_DIVISION_BY_ZERO;
    }
    calc_data->result = calc_data->number2 / calc_data->number1;
    DPRINTF("\tDIV: %d / %d = %d\n", calc_data->number2, calc_data->number1, calc_data->result);

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
    fflush(output_stream);

    DPRINTF("\tOUT: %d\n", result);

    return 0;
}

int HandlePopr(Stack_t* stack, Proc_t* proc_data, int index)
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

    DPRINTF("\tPOPR: poped %d\n"
            "\t\treg[%d] = %d (R%cX)\n",
            value, index, value, index + 'A');

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

    DPRINTF("\tPUSHR: reg[%d] = %d (R%cX)\n"
            "\t\tpushed %d\n",
            index, proc_data->regs[index],
            index + 'A', proc_data->regs[index]);

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

#ifdef PROC_DEBUG
    getchar();
#endif /* PROC_DEBUG */

    if (StackPush(stack, value) != STACK_SUCCESS)
    {
        return 1;
    }
    DPRINTF("\tIN: got and pushed %d\n", value);

    return 0;
}

int HandleJmp(Proc_t* proc_data, size_t new_cmd_count)
{
    assert(proc_data != NULL);

    if (new_cmd_count > proc_data->code_size)
    {
        DPRINTF("Invalid argument in JMP\n");
        return 1;
    }
    proc_data->cmd_count = new_cmd_count;
    DPRINTF("\tJMP: jumping to %zu\n", new_cmd_count);

    return 0;
}

int HandleCall(Proc_t* proc_data, int new_cmd_count)
{
    assert(proc_data != NULL);

    if (StackPush(&proc_data->call_stack, proc_data->cmd_count) != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }

    DPRINTF("\tCALL: pushed %d to call_stack\n", proc_data->cmd_count);

    if (HandleJmp(proc_data, new_cmd_count))
    {
        return 1;
    }

    return 0;
}

int HandleRet(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int call_address = -1;

    if (StackPop(&proc_data->call_stack, &call_address) != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }

    DPRINTF("\tRET: returning to %d\n", call_address);

    if (HandleJmp(proc_data, call_address))
    {
        return 1;
    }

    return 0;
}

int HandlePushm(Proc_t* proc_data, int reg_number)
{
    assert(proc_data != NULL);

    int mem_addr = proc_data->regs[reg_number];
    DPRINTF("\tPUSHM: address = regs[%d] (R%cX) = %d\n", reg_number, 'A' + reg_number, mem_addr);

    if (!(mem_addr >= 0 && mem_addr < RAM_SIZE))
    {
        printf("Given address to pushm is too big\n");
        return 1;
    }

    int value = proc_data->ram[mem_addr];
    if (StackPush(&proc_data->stack, value))
    {
        return 1;
    }
    DPRINTF("\t\tpushed from ram[%d] = %d\n", mem_addr, value);

    return 0;
}

int HandlePopm(Proc_t* proc_data, int reg_number)
{
    assert(proc_data != NULL);

    int mem_addr = proc_data->regs[reg_number];
    DPRINTF("\tPOPM: address = regs[%d] (R%cX) = %d\n", reg_number, 'A' + reg_number, mem_addr);

    if (!(mem_addr >= 0 && mem_addr < RAM_SIZE))
    {
        printf("Given address to pushm is too big\n");
        return 1;
    }

    int value = 0;

    if (StackPop(&proc_data->stack, &value))
    {
        return 1;
    }
    proc_data->ram[mem_addr] = value;
    DPRINTF("\t\tpoped to ram[%d] = %d\n", mem_addr, value);

    return 0;
}

// realisation without macro

// int HandleJumpIf(Stack_t* stack, Proc_t* proc_data,
//                  size_t new_cmd_count, Command_t command)
// {
//     assert(proc_data != NULL);
//     assert(stack != NULL);
//
//     int number1 = 0;
//     int number2 = 0;
//     if (StackPop(stack, &number2))
//     {
//         return 1;
//     }
//     if (StackPop(stack, &number1))
//     {
//         return 1;
//     }
//     // if compare returns 0 than don't jump
//     if (!(CompareForJump(number1, number2, command)))
//     {
//         DPRINTF("-Jump rejected\n");
//         return 0;
//     }
//     // else jump
//     if (HandleJmp(proc_data, new_cmd_count))
//     {
//         return 1;
//     }
//
//     return 0;
// }

// int CompareForJump(int number1, int number2, Command_t command)
// {
//     switch (command)
//     {
//         case CMD_JB:  return number1 <  number2;
//         case CMD_JBE: return number1 <= number2;
//         case CMD_JA:  return number1 >  number2;
//         case CMD_JAE: return number1 >= number2;
//         case CMD_JE:  return number1 == number2;
//         case CMD_JNE: return number1 != number2;
//         default:      return EOF;
//     }
//
//     return EOF;
// }
