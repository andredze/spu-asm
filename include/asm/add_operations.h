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

AsmErr_t AddOpHLT   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpPUSH  (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpOUT   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpIN    (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpCALL  (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpRET   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpADD   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpSUB   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpMUL   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpDIV   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpMOD   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpSQRT  (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpSQR   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpJMP   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpJB    (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpJBE   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpJA    (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpJAE   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpJE    (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpJNE   (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpPUSHR (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpPOPR  (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpPUSHM (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpPOPM  (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpDRAW  (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
AsmErr_t AddOpWDRAW (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t ProcessLabelCase(AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx);

int EndIsSpaces(char* str);

AsmErr_t AddLabelCode(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t AddNoArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t AddNormalArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t AddLabelArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t CheckLabel(AsmCtx_t* asm_ctx, int label);

AsmErr_t AddRegArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

AsmErr_t CheckReg(char reg[]);

AsmErr_t AddRamArgOp(CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);

int LabelsRecalloc(AsmCtx_t* asm_ctx, int new_size);

#endif /* ADD_OPERATIONS_H */
