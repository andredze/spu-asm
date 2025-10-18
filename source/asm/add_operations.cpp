#include "add_operations.h"

#define DECLARE_ADD_OP(cmd_name, args_type)                             \
    AsmErr_t AddOp ## cmd_name(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)    \
    {                                                                   \
        return Add ## args_type ## ArgOp(cmd_ctx, asm_ctx);             \
    }

DECLARE_ADD_OP(HLT,   No);
DECLARE_ADD_OP(PUSH,  Normal);
DECLARE_ADD_OP(OUT,   No);
DECLARE_ADD_OP(IN,    No);
DECLARE_ADD_OP(CALL,  Label);
DECLARE_ADD_OP(RET,   No);
DECLARE_ADD_OP(ADD,   No);
DECLARE_ADD_OP(SUB,   No);
DECLARE_ADD_OP(MUL,   No);
DECLARE_ADD_OP(DIV,   No);
DECLARE_ADD_OP(MOD,   No);
DECLARE_ADD_OP(SQRT,  No);
DECLARE_ADD_OP(SQR,   No);
DECLARE_ADD_OP(JMP,   Label);
DECLARE_ADD_OP(JB,    Label);
DECLARE_ADD_OP(JBE,   Label);
DECLARE_ADD_OP(JA,    Label);
DECLARE_ADD_OP(JAE,   Label);
DECLARE_ADD_OP(JE,    Label);
DECLARE_ADD_OP(JNE,   Label);
DECLARE_ADD_OP(PUSHR, Reg);
DECLARE_ADD_OP(POPR,  Reg);
DECLARE_ADD_OP(PUSHM, Ram);
DECLARE_ADD_OP(POPM,  Ram);
DECLARE_ADD_OP(DRAW,  Normal);
DECLARE_ADD_OP(WDRAW, Normal);

AsmErr_t ProcessLabelCase(AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    if (cmd_ctx->line[0] != ':')
    {
        return ASM_SUCCESS;
    }
    cmd_ctx->command = CMD_LABEL;
    DPRINTF(LIGHT_YELLOW "cmd = LABEL (%s)" RESET_CLR, cmd_ctx->line);

    int args_len = 0;
    if (sscanf(cmd_ctx->line, ":%d%n", &cmd_ctx->value, &args_len) != 1)
    {
        printf("Syntax error: wrong label name, should be a number\n");
        return ASM_SYNTAX_ERROR;
    }
    if (!(EndIsSpaces(cmd_ctx->line + args_len)))
    {
        printf("Syntax error: symbols after label name\n");
        return ASM_SYNTAX_ERROR;
    }
    if (AddLabelCode(cmd_ctx, asm_ctx) != ASM_SUCCESS)
    {
        return ASM_ADD_OP_ERROR;
    }

    return ASM_SUCCESS;
}

int EndIsSpaces(char* str)
{
    assert(str != NULL);

    int i = 0;
    while (str[i] != '\0')
    {
        if (!(isspace(str[i])))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

AsmErr_t AddLabelCode(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    if (CheckLabel(asm_ctx, cmd_ctx->value) != ASM_SUCCESS)
    {
        return ASM_LABEL_ERROR;
    }
    asm_ctx->labels[cmd_ctx->value] = asm_ctx->cur_cmd;

    return ASM_SUCCESS;
}

AsmErr_t AddNoArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;

    return ASM_SUCCESS;
}

AsmErr_t AddNormalArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " %d%n", &cmd_ctx->value, &args_len) != 1)
    {
        // printf("Syntax error: wrong number of args for cmd: %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_GET_OP_ARG_ERR;
    }
    if (!(EndIsSpaces(cmd_ctx->line + cmd_ctx->op_len + args_len)))
    {
        // printf("Syntax error: symbols after argument for cmd = %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_SYNTAX_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->value;

    return ASM_SUCCESS;
}

AsmErr_t AddLabelArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    int label = 0;
    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " :%d%n", &label, &args_len) != 1)
    {
        // printf("Syntax error: wrong arguments for label arg cmd: %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_SYNTAX_ERROR;
    }
    if (!(EndIsSpaces(cmd_ctx->line + cmd_ctx->op_len + args_len)))
    {
        // printf("Syntax error: symbols after argument for cmd = %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_SYNTAX_ERROR;
    }
    if (CheckLabel(asm_ctx, label) != ASM_SUCCESS)
    {
        return ASM_LABEL_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = asm_ctx->labels[label];

    return ASM_SUCCESS;
}

AsmErr_t CheckLabel(AsmCtx_t* asm_ctx, int label)
{
    assert(asm_ctx != NULL);

    if (!(label >= 0))
    {
        printf("Syntax error: Given lable for operation is negative\n");
        return ASM_SYNTAX_ERROR;
    }
    if (label >= MAX_LABELS_SIZE)
    {
        printf("Syntax error: Given lable for operation is too big\n");
        return ASM_SYNTAX_ERROR;
    }
    if (label < asm_ctx->labels_size)
    {
        return ASM_SUCCESS;
    }
    DPRINTF("Memory realloc...\n");
    if (LabelsRecalloc(asm_ctx, 2 * label))
    {
        return ASM_LABELS_RECALLOC_ERROR;
    }

    return ASM_SUCCESS;
}

AsmErr_t AddRegArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    char reg[CMD_MAX_LEN] = {};
    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " %s%n", reg, &args_len) != 1)
    {
        // printf("Syntax error: wrong arguments for reg args cmd: %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_SYNTAX_ERROR;
    }
    if (CheckReg(reg) != ASM_SUCCESS)
    {
        return ASM_SYNTAX_ERROR;
    }
    if (!(EndIsSpaces(cmd_ctx->line + cmd_ctx->op_len + args_len)))
    {
        // printf("Syntax error: symbols after argument for cmd = %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_SYNTAX_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = reg[1] - 'A';

    return ASM_SUCCESS;
}

AsmErr_t CheckReg(char reg[])
{
    assert(reg);

    if (strlen(reg) != 3)
    {
        printf("Syntax error: register should match format R*X (given %s) (len == 3 failed)\n", reg);
        return ASM_SYNTAX_ERROR;
    }
    if (!(reg[0] == 'R' && reg[2] == 'X' &&
          reg[1] - 'A' <= REGS_COUNT && reg[1] >= 'A'))
    {
        printf("Syntax error: register should match format R*X (given %s)\n", reg);
        return ASM_SYNTAX_ERROR;
    }

    return ASM_SUCCESS;
}

AsmErr_t AddRamArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    char reg[CMD_MAX_LEN] = {};
    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " [%[^]]]%n", reg, &args_len) != 1)
    {
        // printf("Syntax error: wrong arguments for ram args cmd: %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_SYNTAX_ERROR;
    }
    if (CheckReg(reg) != ASM_SUCCESS)
    {
        return ASM_SYNTAX_ERROR;
    }
    if (!(EndIsSpaces(cmd_ctx->line+ cmd_ctx->op_len + args_len)))
    {
        // printf("Syntax error: symbols after argument for cmd = %s\n",
        //        COMM_CASES[cmd_ctx->command].str_command);
        return ASM_SYNTAX_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = reg[1] - 'A';

    return ASM_SUCCESS;
}

int LabelsRecalloc(AsmCtx_t* asm_ctx, int new_size)
{
    int old_size = asm_ctx->labels_size;
    size_t* labels = (size_t*) realloc(asm_ctx->labels, new_size * sizeof(size_t));
    if (labels == NULL)
    {
        printf("Labels recalloc failed\n");
        return 1;
    }
    asm_ctx->labels_size = new_size;
    asm_ctx->labels = labels;

    DPRINTF("old_size = %d\n", old_size);
    DPRINTF("labels[old_size] = %zu\n", asm_ctx->labels[old_size]);

    for (int i = old_size; i < new_size; i++)
    {
        DPRINTF("%d, ", i);
        asm_ctx->labels[i] = -1;
    }
    DPRINTF("\n");
    DPRINTF("labels[old_size] = %zu\n", asm_ctx->labels[old_size]);

    return 0;
}
