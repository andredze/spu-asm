#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#ifdef ASM_DEBUG
    #define DEBUG
#endif /* ASM_DEBUG */

#include "input.h"
#include "listing.h"
#include "asm_types.h"
#include "commands.h"
#include "cmd_cases.h"

const int MAX_FILENAME_LEN = 100;
const int ASM_MAX_ARGS_COUNT = 2;
const char COMMENT_SYMBOL = '#';

int SetFilenames(const char** commands_filename,
                 const char** bytecode_filename,
                 int argc, char* argv[], int* listing_flag);

AsmErr_t CompileProgramm(InputCtx_t* input_ctx, int listing_flag);

int AsmCtxCtor(InputCtx_t* input_ctx, AsmCtx_t* asm_ctx);

AsmErr_t CompileCommands(InputCtx_t* input_ctx,
                         AsmCtx_t* asm_ctx,
                         int listing_flag);

AsmErr_t GetCmd(AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx);

int WriteByteCode(AsmCtx_t* asm_ctx, InputCtx_t* input_ctx);

int WriteByteCodePretty(AsmCtx_t* asm_ctx, const char* filepath);

void AsmDestroy(InputCtx_t* input_ctx, AsmCtx_t* asm_ctx);

#endif /* ASSEMBLER_H */
