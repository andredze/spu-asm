#include "add_operations.h"
#include "listing.h"

//==========================================================================================

AsmErr_t ProcessLabelCase(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
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

    char label[MAX_LABEL_LEN] = {};

    if (sscanf(cmd_ctx->line, ":%s%n", label, &args_len) != 1)
    {
        printf("Syntax error: wrong label name, should be a number\n");
        return ASM_SYNTAX_ERROR;
    }

    if (!(EndIsSpaces(cmd_ctx->line + args_len)))
    {
        printf("Syntax error: symbols after label name\n");
        return ASM_SYNTAX_ERROR;
    }

    size_t label_ind = 0;

    if ((label_ind = FindLabelInTable(asm_ctx, label)) == (size_t)-1)
    {
        if (LabelPutInTable(asm_ctx, label, &label_ind))
            return ASM_LABEL_ERROR;
    }

    DPRINTF("\nasm_ctx->cur_cmd = %zu\n" RESET_CLR, asm_ctx->cur_cmd);

    asm_ctx->labels[label_ind].code_ind = asm_ctx->cur_cmd;

    DPrintLabels(asm_ctx);

    return ASM_SUCCESS;
}

//==========================================================================================

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

//==========================================================================================

AsmErr_t AddNoArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;

    return ASM_SUCCESS;
}

//==========================================================================================

AsmErr_t AddNormalArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " %d%n", &cmd_ctx->value, &args_len) != 1)
    {
        printf("Syntax error: wrong number of args for cmd: %s\n",
               cmd_ctx->line);
        return ASM_GET_OP_ARG_ERR;
    }
    if (!(EndIsSpaces(cmd_ctx->line + cmd_ctx->op_len + args_len)))
    {
        printf("Syntax error: symbols after argument for cmd = %s\n",
               cmd_ctx->line);
        return ASM_SYNTAX_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->value;

    return ASM_SUCCESS;
}

//==========================================================================================

AsmErr_t AddLabelArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    char label[MAX_LABEL_LEN] = {};

    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " :%s%n", label, &args_len) != 1)
    {
        printf("Syntax error: wrong arguments for label arg cmd: %s\n",
               cmd_ctx->line);
        return ASM_SYNTAX_ERROR;
    }
    if (!(EndIsSpaces(cmd_ctx->line + cmd_ctx->op_len + args_len)))
    {
        printf("Syntax error: symbols after argument for cmd = %s\n",
               cmd_ctx->line);
        return ASM_SYNTAX_ERROR;
    }

    size_t label_ind = 0;

    if ((label_ind = FindLabelInTable(asm_ctx, label)) == (size_t)-1)
    {
        DPRINTF("Delayu put in tablo\n");

        if (LabelPutInTable(asm_ctx, label, &label_ind))
            return ASM_LABEL_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = asm_ctx->labels[label_ind].code_ind;

    DPrintLabels(asm_ctx);

    return ASM_SUCCESS;
}

//==========================================================================================

size_t FindLabelInTable(AsmCtx_t* asm_ctx, char* label_name)
{
    assert(asm_ctx    != NULL);
    assert(label_name != NULL);

    for (size_t ind = 0; ind < asm_ctx->labels_size; ind++)
    {
        if (strcmp(label_name, asm_ctx->labels[ind].name) == 0)
            return ind;
    }

    return (size_t)-1;
}

//==========================================================================================

AsmErr_t LabelPutInTable(AsmCtx_t* asm_ctx, char* label_name, size_t* label_ind)
{
    assert(label_name != NULL);
    assert(asm_ctx    != NULL);

    if (asm_ctx->labels_size >= asm_ctx->labels_capacity)
    {
        if (LabelsRecalloc(asm_ctx, asm_ctx->labels_capacity * 2 + 1))
            return ASM_LABELS_RECALLOC_ERROR;
    }

    char* copy = strdup(label_name);

    if (copy == NULL)
    {
        DPRINTF("Memory allocation failed");
        return ASM_CALLOC_ERROR;
    }

    asm_ctx->labels[asm_ctx->labels_size++].name = copy;

    *label_ind = asm_ctx->labels_size - 1;

    return ASM_SUCCESS;
}

//==========================================================================================

AsmErr_t AddRegArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    char reg[CMD_MAX_LEN] = {};
    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " %s%n", reg, &args_len) != 1)
    {
        printf("Syntax error: wrong arguments for reg args cmd: %s\n",
               cmd_ctx->line);
        return ASM_SYNTAX_ERROR;
    }
    if (CheckReg(reg) != ASM_SUCCESS)
    {
        return ASM_SYNTAX_ERROR;
    }
    if (!(EndIsSpaces(cmd_ctx->line + cmd_ctx->op_len + args_len)))
    {
        printf("Syntax error: symbols after argument for cmd = %s\n",
               cmd_ctx->line);
        return ASM_SYNTAX_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = reg[1] - 'A';

    return ASM_SUCCESS;
}

//==========================================================================================

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

//==========================================================================================

AsmErr_t AddRamArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx)
{
    assert(cmd_ctx);
    assert(asm_ctx);

    char reg[CMD_MAX_LEN] = {};
    int args_len = 0;

    if (sscanf(cmd_ctx->line + cmd_ctx->op_len, " [%[^]]]%n", reg, &args_len) != 1)
    {
        printf("Syntax error: wrong arguments for ram args cmd: %s\n",
               cmd_ctx->line);
        return ASM_SYNTAX_ERROR;
    }
    if (CheckReg(reg) != ASM_SUCCESS)
    {
        return ASM_SYNTAX_ERROR;
    }
    if (!(EndIsSpaces(cmd_ctx->line+ cmd_ctx->op_len + args_len)))
    {
        printf("Syntax error: symbols after argument for cmd = %s\n",
               cmd_ctx->line);
        return ASM_SYNTAX_ERROR;
    }

    asm_ctx->buffer[asm_ctx->cur_cmd++] = cmd_ctx->command;
    asm_ctx->buffer[asm_ctx->cur_cmd++] = reg[1] - 'A';

    return ASM_SUCCESS;
}

//==========================================================================================

int LabelsRecalloc(AsmCtx_t* asm_ctx, size_t new_capacity)
{
    assert(asm_ctx);

    int old_capacity = asm_ctx->labels_capacity;

    Label_t* labels = (Label_t*) realloc(asm_ctx->labels, new_capacity * sizeof(Label_t));

    if (labels == NULL)
    {
        printf("Labels recalloc failed\n");
        return 1;
    }

    asm_ctx->labels_capacity = new_capacity;
    asm_ctx->labels          = labels;

    for (size_t i = old_capacity; i < new_capacity; i++)
    {
        DPRINTF("%zu, ", i);
        asm_ctx->labels[i] = {};
    }

    DPRINTF("\n");
    DPRINTF("labels[old_size] = %zu\n", asm_ctx->labels[old_capacity]);

    return 0;
}

//==========================================================================================
