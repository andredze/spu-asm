#ifndef ASSEMBLER_H
#define ASSEMBLER_H

//——————————————————————————————————————————————————————————————————————————————————————————

#ifdef ASM_DEBUG
    #ifndef DEBUG
        #define DEBUG
    #endif /* DEBUG */
#endif /* ASM_DEBUG */

//——————————————————————————————————————————————————————————————————————————————————————————

#include "input.h"
#include "listing.h"
#include "asm_types.h"
#include "commands.h"
#include "cmd_cases.h"
#include "config.h"

//——————————————————————————————————————————————————————————————————————————————————————————

const int  MAX_FILENAME_LEN   = 100;
const int  ASM_MAX_ARGS_COUNT = 2;
const char COMMENT_SYMBOL     = '#';

//——————————————————————————————————————————————————————————————————————————————————————————

int SetFilenames(const char** commands_filename,
                 const char** bytecode_filename,
                 int argc, char* argv[], int* listing_flag);

char* SkipSpaces(char* str);

//——————————————————————————————————————————————————————————————————————————————————————————

AsmErr_t SetHashInCmdCases    ();
int      AsmGetHash           (const char* str);
int      AsmCmdCasesCompare   (const void* par1, const void* par2);
int      CompareInts          (int value1, int value2);
int      AsmCtxCtor           (InputCtx_t* input_ctx, AsmCtx_t* asm_ctx);
AsmErr_t CompileProgram       (InputCtx_t* input_ctx, int listing_flag);
AsmErr_t CompileCode          (InputCtx_t* input_ctx, AsmCtx_t* asm_ctx, int listing_flag);
void     AsmDestroy           (InputCtx_t* input_ctx, AsmCtx_t* asm_ctx);
AsmErr_t CompileCmd           (AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx, int* do_continue);
AsmErr_t GetCmd               (AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx);
int      WriteByteCode        (AsmCtx_t* asm_ctx, InputCtx_t* input_ctx);
int      WriteByteCodePretty  (AsmCtx_t* asm_ctx, const char* filepath);
int      CmdCasesBinarySearch (int curr_hash, CmdCase_t cmd_case[], int size);

//——————————————————————————————————————————————————————————————————————————————————————————

#endif /* ASSEMBLER_H */
