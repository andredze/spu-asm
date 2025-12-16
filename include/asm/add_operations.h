#ifndef ADD_OPERATIONS_H
#define ADD_OPERATIONS_H

//——————————————————————————————————————————————————————————————————————————————————————————

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "asm_types.h"
#include "input.h"
#include "colors.h"

//——————————————————————————————————————————————————————————————————————————————————————————

const int MIN_LABELS_SIZE = 10;
const int MAX_LABELS_SIZE = 1000;
const int MAX_LABEL_LEN   = 64;
const int REGS_COUNT      = 8;

//——————————————————————————————————————————————————————————————————————————————————————————

AsmErr_t ProcessLabelCase (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddLabelCode     (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddNoArgOp       (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddNormalArgOp   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddLabelArgOp    (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddRegArgOp      (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddRamArgOp      (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

size_t   FindLabelInTable (AsmCtx_t* asm_ctx, char* label_name);
AsmErr_t LabelPutInTable  (AsmCtx_t* asm_ctx, char* label_name, size_t* label_ind);

AsmErr_t CheckReg         (char reg[]);

//——————————————————————————————————————————————————————————————————————————————————————————

int EndIsSpaces    (char* str);
int LabelsRecalloc (AsmCtx_t* asm_ctx, size_t new_capacity);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* ADD_OPERATIONS_H */
