#include "handle_operations.h"

//——————————————————————————————————————————————————————————————————————————————————————————

#define DECLARE_HANDLE_JUMP_IF(comp_oper, cmd_name)                     \
    HandleOpErr_t Handle##cmd_name(Proc_t* proc_data)                   \
    {                                                                   \
        assert(proc_data != NULL);                                      \
                                                                        \
        int new_cmd_count = proc_data->code[proc_data->cmd_count++];    \
        int number1 = 0;                                                \
        int number2 = 0;                                                \
        if (StackPop(&proc_data->stack, &number2))                      \
        {                                                               \
            return HANDLE_OP_STACK_ERROR;                               \
        }                                                               \
        if (StackPop(&proc_data->stack, &number1))                      \
        {                                                               \
            return HANDLE_OP_STACK_ERROR;                               \
        }                                                               \
        DPRINTF("\t" #cmd_name ": (%d " #comp_oper " %d) == %d\n",      \
                number1, number2, number1 comp_oper number2);           \
        if (!(number1 comp_oper number2))                               \
        {                                                               \
            DPRINTF("\tJMP rejected\n");                                \
            return HANDLE_OP_SUCCESS;                                   \
        }                                                               \
        if (Jump(proc_data, new_cmd_count))                             \
        {                                                               \
            return HANDLE_OP_INVALID_JUMP_ARG;                          \
        }                                                               \
        return HANDLE_OP_SUCCESS;                                       \
    }

//------------------------------------------------------------------------------------------

DECLARE_HANDLE_JUMP_IF(<,  JB);
DECLARE_HANDLE_JUMP_IF(<=, JBE);
DECLARE_HANDLE_JUMP_IF(>,  JA);
DECLARE_HANDLE_JUMP_IF(>=, JAE);
DECLARE_HANDLE_JUMP_IF(==, JE);
DECLARE_HANDLE_JUMP_IF(!=, JNE);

//------------------------------------------------------------------------------------------

#undef DECLARE_HANDLE_JUMP_IF

//——————————————————————————————————————————————————————————————————————————————————————————

#define DECLARE_HANDLE_BINARY_MATH_OP(cmd_name, math_oper, check_errors)        \
    HandleOpErr_t Handle##cmd_name(Proc_t* proc_data)                           \
    {                                                                           \
        assert(proc_data->stack.size >= 2);                                     \
                                                                                \
        int number1 = 0;                                                        \
        int number2 = 0;                                                        \
                                                                                \
        if (StackPop(&proc_data->stack, &number2) != STACK_SUCCESS)             \
        {                                                                       \
            return HANDLE_OP_STACK_ERROR;                                       \
        }                                                                       \
        if (StackPop(&proc_data->stack, &number1) != STACK_SUCCESS)             \
        {                                                                       \
            return HANDLE_OP_STACK_ERROR;                                       \
        }                                                                       \
                                                                                \
        check_errors;                                                           \
        int result = number1 math_oper number2;                                 \
        DPRINTF("\t" #cmd_name ": %d %s %d = %d\n",                             \
                number1, #math_oper, number2, result);                          \
                                                                                \
        if (StackPush(&proc_data->stack, result) != STACK_SUCCESS)              \
        {                                                                       \
            return HANDLE_OP_STACK_ERROR;                                       \
        }                                                                       \
        return HANDLE_OP_SUCCESS;                                               \
    }

//------------------------------------------------------------------------------------------

DECLARE_HANDLE_BINARY_MATH_OP(ADD, +, {};);
DECLARE_HANDLE_BINARY_MATH_OP(SUB, -, {};);
DECLARE_HANDLE_BINARY_MATH_OP(MUL, *, {};);
DECLARE_HANDLE_BINARY_MATH_OP(DIV, /, {if (number2 == 0) {printf("Error: division by zero\n"); return HANDLE_OP_DIVISION_BY_ZERO;}});
DECLARE_HANDLE_BINARY_MATH_OP(MOD, /, {if (number2 == 0) {printf("Error: division by zero\n"); return HANDLE_OP_DIVISION_BY_ZERO;}});

//------------------------------------------------------------------------------------------

#undef DECLARE_HANDLE_BINARY_MATH_OP

//——————————————————————————————————————————————————————————————————————————————————————————

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

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleHLT(Proc_t* proc_data)
{
    return HANDLE_OP_BREAK_LOOP;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandlePUSH(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int value = proc_data->code[proc_data->cmd_count++];

    DPRINTF("\tPUSH: %d\n", value);
    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleOUT(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int result = 0;
    StackErr_t pop_return = StackPop(&proc_data->stack, &result);

    if (pop_return == STACK_SIZE_IS_ZERO)
    {
        return HANDLE_OP_SUCCESS;
    }
    if (pop_return != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }
    printf("ANSWER = %d\n", result);
    fflush(stdout);

    DPRINTF("\tOUT: %d\n", result);

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleIN(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int value = 0;
    printf("Enter int value: ");
    if (scanf("%d", &value) != 1)
    {
        printf("Input is wrong, try better :)\n");
        return HANDLE_OP_INPUT_ERROR;
    }

#ifdef PROC_DEBUG
    getchar();
#endif /* PROC_DEBUG */

    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }
    DPRINTF("\tIN: got and pushed %d\n", value);

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleCALL(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int new_cmd_count = proc_data->code[proc_data->cmd_count++];

    if (StackPush(&proc_data->call_stack, proc_data->cmd_count) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }

    DPRINTF("\tCALL: pushed %zu to call_stack\n", proc_data->cmd_count);

    if (Jump(proc_data, new_cmd_count))
    {
        return HANDLE_OP_INVALID_JUMP_ARG;
    }

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleRET(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int call_address = -1;

    if (StackPop(&proc_data->call_stack, &call_address) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }

    DPRINTF("\tRET: returning to %d\n", call_address);

    if (Jump(proc_data, call_address))
    {
        return HANDLE_OP_INVALID_JUMP_ARG;
    }

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleSQRT(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int number = 0;
    if (StackPop(&proc_data->stack, &number) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }
    int value = (int) sqrt((double) number);
    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }
    DPRINTF("\tSQRT: sqrt(%d) = %d\n", number, value);

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleSQR(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int number = 0;
    if (StackPop(&proc_data->stack, &number) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }
    int value = number * number;
    if (StackPush(&proc_data->stack, value) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }
    DPRINTF("\tSQR: %d ** 2 = %d\n", number, value);

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleJMP(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int new_cmd_count = proc_data->code[proc_data->cmd_count++];

    DPRINTF("\tJMP: ");
    if (Jump(proc_data, new_cmd_count))
    {
        return HANDLE_OP_INVALID_JUMP_ARG;
    }

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandlePUSHR(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int index = proc_data->code[proc_data->cmd_count++];

    if (index < 0 || index > 7)
    {
        DPRINTF("Invalid register index given\n");
        return HANDLE_OP_INVALID_REG_ARG;
    }
    if (StackPush(&proc_data->stack, proc_data->regs[index]) != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }

    DPRINTF("\tPUSHR: reg[%d] = %d (R%cX)\n"
            "\t\tpushed %d\n",
            index, proc_data->regs[index],
            index + 'A', proc_data->regs[index]);

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandlePOPR(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int index = proc_data->code[proc_data->cmd_count++];
    int value = 0;
    StackErr_t pop_return = StackPop(&proc_data->stack, &value);

    if (pop_return == STACK_SIZE_IS_ZERO)
    {
        return HANDLE_OP_SUCCESS;
    }
    if (pop_return != STACK_SUCCESS)
    {
        return HANDLE_OP_STACK_ERROR;
    }

    if (index < 0 || index > 7)
    {
        DPRINTF("Invalid register index given\n");
        return HANDLE_OP_INVALID_REG_ARG;
    }
    proc_data->regs[index] = value;

    DPRINTF("\tPOPR: poped %d\n"
            "\t\treg[%d] = %d (R%cX)\n",
            value, index, value, index + 'A');

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandlePUSHM(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int reg_number = proc_data->code[proc_data->cmd_count++];
    int mem_addr = proc_data->regs[reg_number];
    DPRINTF("\tPUSHM: address = regs[%d] (R%cX) = %d\n", reg_number, 'A' + reg_number, mem_addr);

    if (!(mem_addr >= 0 && mem_addr < RAM_SIZE))
    {
        printf("Given address to pushm is too big\n");
        return HANDLE_OP_INVALID_RAM_ARG;
    }

    int value = proc_data->ram[mem_addr];
    if (StackPush(&proc_data->stack, value))
    {
        return HANDLE_OP_STACK_ERROR;
    }
    DPRINTF("\t\tpushed from ram[%d] = %d\n", mem_addr, value);

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandlePOPM(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int reg_number = proc_data->code[proc_data->cmd_count++];
    size_t mem_addr = proc_data->regs[reg_number];
    DPRINTF("\tPOPM: address = regs[%d] (R%cX) = %zu\n", reg_number, 'A' + reg_number, mem_addr);

    if (!(mem_addr <= MAX_RAM_ADDRESS && mem_addr < RAM_SIZE))
    {
        printf("Given address to pushm is too big\n");
        return HANDLE_OP_INVALID_RAM_ARG;
    }

    int value = 0;

    if (StackPop(&proc_data->stack, &value))
    {
        return HANDLE_OP_STACK_ERROR;
    }
    proc_data->ram[mem_addr] = value;
    DPRINTF("\t\tpoped to ram[%zu] = %d\n", mem_addr, value);

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleDRAW(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int sleep_time = proc_data->code[proc_data->cmd_count++];

    if (ConsoleDrawVram(proc_data, sleep_time))
    {
        return HANDLE_OP_DRAW_ERROR;
    }

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

#ifdef GRAPHICS

//------------------------------------------------------------------------------------------

HandleOpErr_t HandleWDRAW(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    int sleep_time = proc_data->code[proc_data->cmd_count++];

    WindowOpen();

    if (WindowDrawVram(proc_data, sleep_time))
    {
        return HANDLE_OP_DRAW_ERROR;
    }

    return HANDLE_OP_SUCCESS;
}

//------------------------------------------------------------------------------------------

int WindowOpen()
{
    txCreateWindow(100, 100);
    txSetDefaults();
    txSetColor(TX_WHITE, 0);

    return 0;
}

//------------------------------------------------------------------------------------------

int WindowDrawVram(Proc_t* proc_data, int sleep_time)
{
    HDC dc = txDC();

    COLORREF color = TX_WHITE;
    for (int y_ram = 0; y_ram < RAM_HEIGHT; y_ram++)
    {
        for (int x_ram = 0; x_ram < RAM_WIDTH; x_ram++)
        {
            color = TX_WHITE;
            if (proc_data->ram[y_ram * RAM_WIDTH + x_ram] != 0)
                color = TX_GREEN;
            txSetPixel(x_ram, y_ram, color, dc);
        }
    }

    txSaveImage("draw.bmp", dc);
    Sleep(sleep_time);

    return 0;
}

//------------------------------------------------------------------------------------------

#endif /* GRAPHICS */

//------------------------------------------------------------------------------------------

int ConsoleDrawVram(Proc_t* proc_data, int sleep_time)
{
    assert(proc_data != NULL);

    usleep(sleep_time / 3 * 2.75);

    static HANDLE hConsole = NULL;
    static COORD home = {0, 0};
    static DWORD written = 0;

    if (hConsole == NULL)
    {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        system("cls");
    }

    char buffer[RAM_SIZE * 3 + RAM_HEIGHT + 1] = {};
    int buf_ind = 0;

    for (size_t i = 0; i < RAM_SIZE; i++)
    {
        buffer[buf_ind++] = proc_data->ram[i];

        if ((i + 1) % RAM_WIDTH == 0)
        {
            buffer[buf_ind++] = '\n';
        }
    }
    buffer[buf_ind++] = '\n';

    SetConsoleCursorPosition(hConsole, home);
    WriteConsoleA(hConsole, buffer, buf_ind - 1, &written, NULL);

    return 0;
}

//------------------------------------------------------------------------------------------
