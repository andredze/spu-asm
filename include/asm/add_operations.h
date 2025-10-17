#ifndef ADD_OPERATIONS_H
#define ADD_OPERATIONS_H

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "asm_types.h"
#include "input.h"
#include "colors.h"

const int MIN_LABELS_SIZE = 10;
const int MAX_LABELS_SIZE = 1000;
const int REGS_COUNT = 8;

AsmErr_t ProcessLabelCase(AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx);

int EndIsSpaces(char* str);

AsmErr_t AddLabelCode(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t AddDefaultOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t AddNormalArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t AddLabelArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t CheckLabel(AsmCtx_t* asm_ctx, int label);

AsmErr_t AddRegArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t CheckReg(char reg[]);

AsmErr_t AddRamArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

int LabelsRecalloc(AsmCtx_t* asm_ctx, int new_size);

#endif /* ADD_OPERATIONS_H */
