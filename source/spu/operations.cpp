#include <TXLib.h>
#include "operations.h"

#define DECLARE_HANDLE_JUMP_IF(comp_oper, cmd_name) \
    int Handle##cmd_name(Proc_t* proc_data) \
    { \
        assert(proc_data != NULL); \
        \
        int new_cmd_count = proc_data->code[proc_data->cmd_count++]; \
        int number1 = 0; \
        int number2 = 0; \
        if (StackPop(&proc_data->stack, &number2)) \
        { \
            return 1; \
        } \
        if (StackPop(&proc_data->stack, &number1)) \
        { \
            return 1; \
        } \
        DPRINTF("\t" #cmd_name ": (%d " #comp_oper " %d) == %d\n", \
                number1, number2, number1 comp_oper number2); \
        if (!(number1 comp_oper number2)) \
        { \
            DPRINTF("\tJMP rejected\n"); \
            return 0; \
        } \
        if (Jump(proc_data, new_cmd_count)) \
        { \
            return 1; \
        } \
        return 0; \
    }

DECLARE_HANDLE_JUMP_IF(<,  Jb);
DECLARE_HANDLE_JUMP_IF(<=, Jbe);
DECLARE_HANDLE_JUMP_IF(>,  Ja);
DECLARE_HANDLE_JUMP_IF(>=, Jae);
DECLARE_HANDLE_JUMP_IF(==, Je);
DECLARE_HANDLE_JUMP_IF(!=, Jne);

#define DECLARE_HANDLE_MATH_OP(cmd_name, calculate) \
    int Handle##cmd_name(Proc_t* proc_data) \
    { \
        assert(proc_data->stack.size >= 2); \
        \
        CalcData_t calc_data = {}; \
        \
        if (StackPop(&proc_data->stack, &calc_data.number1) != STACK_SUCCESS) \
        { \
            return 1; \
        } \
        if (StackPop(&proc_data->stack, &calc_data.number2) != STACK_SUCCESS) \
        { \
            return 1; \
        } \
        \
        if (calculate(&calc_data) != MATH_SUCCESS) \
        { \
            return 2; \
        } \
        if (StackPush(&proc_data->stack, calc_data.result) != STACK_SUCCESS) \
        { \
            return 1; \
        } \
        \
        return 0; \
    }

DECLARE_HANDLE_MATH_OP(Add, Add);
DECLARE_HANDLE_MATH_OP(Sub, Sub);
DECLARE_HANDLE_MATH_OP(Mul, Mul);
DECLARE_HANDLE_MATH_OP(Div, Div);
DECLARE_HANDLE_MATH_OP(Mod, Mod);

int HandleSqrt(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int number = 0;
    if (StackPop(&proc_data->stack, &number) != STACK_SUCCESS)
    {
        return 1;
    }
    int value = (int) sqrt((double) number);
    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return 1;
    }
    DPRINTF("\tSQRT: sqrt(%d) = %d\n", number, value);

    return 0;
}

int HandleSqr(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int number = 0;
    if (StackPop(&proc_data->stack, &number) != STACK_SUCCESS)
    {
        return 1;
    }
    int value = number * number;
    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return 1;
    }
    DPRINTF("\tSQR: %d ** 2 = %d\n", number, value);

    return 0;
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

MathErr_t Mod(CalcData_t* calc_data)
{
    assert(calc_data != NULL);
    assert(calc_data->result == 0);

    if (calc_data->number1 == 0)
    {
        printf("Can not divide by zero\n");
        return MATH_DIVISION_BY_ZERO;
    }
    calc_data->result = calc_data->number2 % calc_data->number1;
    DPRINTF("\tMOD: %d %% %d = %d\n", calc_data->number2, calc_data->number1, calc_data->result);

    return MATH_SUCCESS;
}

int HandleOut(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int result = 0;
    StackErr_t pop_return = StackPop(&proc_data->stack, &result);

    if (pop_return == STACK_SIZE_IS_ZERO)
    {
        return 0;
    }
    if (pop_return != STACK_SUCCESS)
    {
        return 1;
    }
    printf("ANSWER = %d\n", result);
    fflush(stdout);

    DPRINTF("\tOUT: %d\n", result);

    return 0;
}

int HandlePopr(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int index = proc_data->code[proc_data->cmd_count++];
    int value = 0;
    StackErr_t pop_return = StackPop(&proc_data->stack, &value);

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

int HandlePushr(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int index = proc_data->code[proc_data->cmd_count++];

    if (index < 0 || index > 7)
    {
        DPRINTF("Invalid register index given\n");
        return 1;
    }
    if (StackPush(&proc_data->stack, proc_data->regs[index]) != STACK_SUCCESS)
    {
        return 1;
    }

    DPRINTF("\tPUSHR: reg[%d] = %d (R%cX)\n"
            "\t\tpushed %d\n",
            index, proc_data->regs[index],
            index + 'A', proc_data->regs[index]);

    return 0;
}

int HandleIn(Proc_t* proc_data)
{
    assert(proc_data != NULL);

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

    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return 1;
    }
    DPRINTF("\tIN: got and pushed %d\n", value);

    return 0;
}

int HandleJmp(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int new_cmd_count = proc_data->code[proc_data->cmd_count++];

    DPRINTF("\tJMP: ");
    if (Jump(proc_data, new_cmd_count))
    {
        return 1;
    }

    return 0;
}

int Jump(Proc_t* proc_data, int new_cmd_count)
{
    assert(proc_data != NULL);

    if (new_cmd_count > proc_data->code_size || new_cmd_count < 0)
    {
        DPRINTF("Invalid argument for jump\n");
        return 1;
    }
    proc_data->cmd_count = new_cmd_count;
    DPRINTF("\t   jumping to %d\n", new_cmd_count);

    return 0;
}

int HandleCall(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int new_cmd_count = proc_data->code[proc_data->cmd_count++];

    if (StackPush(&proc_data->call_stack, proc_data->cmd_count) != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }

    DPRINTF("\tCALL: pushed %zu to call_stack\n", proc_data->cmd_count);

    if (Jump(proc_data, new_cmd_count))
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

    if (Jump(proc_data, call_address))
    {
        return 1;
    }

    return 0;
}

int HandlePushm(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int reg_number = proc_data->code[proc_data->cmd_count++];
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

int HandlePopm(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int reg_number = proc_data->code[proc_data->cmd_count++];
    size_t mem_addr = proc_data->regs[reg_number];
    DPRINTF("\tPOPM: address = regs[%d] (R%cX) = %zu\n", reg_number, 'A' + reg_number, mem_addr);

    if (!(mem_addr <= MAX_RAM_ADDRESS && mem_addr < RAM_SIZE))
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
    DPRINTF("\t\tpoped to ram[%zu] = %d\n", mem_addr, value);

    return 0;
}

int HandleDraw(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int sleep_time = proc_data->code[proc_data->cmd_count++];

    for (size_t i = 0; i < RAM_SIZE; i++)
    {
        if (proc_data->ram[i] == 0) {
            printf(" - "); }
        else {
            printf(" %c ", proc_data->ram[i]); }
        if ((i + 1) % RAM_SIDE_SIZE == 0)
        {
            printf("\n");
        }
    }
    printf("\n\n");
    Sleep(sleep_time);

    return 0;
}

int HandlePush(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int value = proc_data->code[proc_data->cmd_count++];

    DPRINTF("\tPUSH: %d\n", value);
    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return 1;
    }

    return 0;
}

int DrawInWindow(Proc_t* proc_data)
{
    txCreateWindow(100, 100);
    HDC dc = txDC();
    txSetDefaults();
    txSetColor(TX_WHITE, 0, dc);

    COLORREF color = TX_WHITE;
    for (int x_ram = 0; x_ram < RAM_SIDE_SIZE; x_ram++)
    {
        for (int y_ram = 0; y_ram < RAM_SIDE_SIZE; y_ram++)
        {
            color = TX_WHITE;
            if (proc_data->ram[y_ram * RAM_SIDE_SIZE + x_ram] != 0)
                color = TX_GREEN;
            txSetPixel(x_ram, y_ram, color, dc);
        }
    }

    txSaveImage("draw.bmp", dc);

    return 0;
}
