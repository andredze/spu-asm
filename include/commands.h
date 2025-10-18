#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#ifdef ASM
    #include "add_operations.h"
#endif
#ifdef SPU
    #include "handle_operations.h"
#endif

typedef enum Command {
    CMD_HLT   = 0,
    CMD_PUSH  = 1,
    CMD_OUT   = 2,
    CMD_IN    = 3,
    CMD_CALL  = 4,
    CMD_RET   = 5,
    CMD_ADD   = 6,
    CMD_SUB   = 7,
    CMD_MUL   = 8,
    CMD_DIV   = 9,
    CMD_MOD   = 10,
    CMD_SQRT  = 11,
    CMD_SQR   = 12,
    CMD_JMP   = 13,
    CMD_JB    = 14,
    CMD_JBE   = 15,
    CMD_JA    = 16,
    CMD_JAE   = 17,
    CMD_JE    = 18,
    CMD_JNE   = 19,
    CMD_PUSHR = 20,
    CMD_POPR  = 21,
    CMD_PUSHM = 22,
    CMD_POPM  = 23,
    CMD_DRAW  = 24,
    CMD_LABEL = 25,
    CMD_END
} Command_t;

typedef struct CodeParams {
    int version;
    size_t code_size;
} CodeParams_t;

typedef struct CmdCase {
    const char* str_command;
    Command_t command;
    int args_count;
#ifdef SPU
    HandleOpErr_t (*handle_op) (Proc_t* proc_data);
#endif /* SPU */
#ifdef ASM
    AsmErr_t (*add_op) (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
#endif /* ASM */
} CmdCase_t;

#ifdef SPU
    #define SET_CMD_CASE(name, args_count)   {#name, CMD_ ## name, (args_count), Handle ## name}
#endif /* SPU */
#ifdef ASM
    #define SET_CMD_CASE(name, args_count)   {#name, CMD_ ## name, (args_count), AddOp ## name}
#endif /* ASM */
#ifndef SET_CMD_CASE
    #define SET_CMD_CASE(name, args_count)   {#name, CMD_ ## name, (args_count)}
#endif /* SET_CMD_CASE */

const CmdCase_t CMD_CASES[] =
    {SET_CMD_CASE (HLT,   1),
     SET_CMD_CASE (PUSH,  2),
     SET_CMD_CASE (OUT,   1),
     SET_CMD_CASE (IN,    1),
     SET_CMD_CASE (CALL,  2),
     SET_CMD_CASE (RET,   1),
     SET_CMD_CASE (ADD,   1),
     SET_CMD_CASE (SUB,   1),
     SET_CMD_CASE (MUL,   1),
     SET_CMD_CASE (DIV,   1),
     SET_CMD_CASE (MOD,   1),
     SET_CMD_CASE (SQRT,  1),
     SET_CMD_CASE (SQR,   1),
     SET_CMD_CASE (JMP,   2),
     SET_CMD_CASE (JB,    2),
     SET_CMD_CASE (JBE,   2),
     SET_CMD_CASE (JA,    2),
     SET_CMD_CASE (JAE,   2),
     SET_CMD_CASE (JE,    2),
     SET_CMD_CASE (JNE,   2),
     SET_CMD_CASE (PUSHR, 2),
     SET_CMD_CASE (POPR,  2),
     SET_CMD_CASE (PUSHM, 2),
     SET_CMD_CASE (POPM,  2),
     SET_CMD_CASE (DRAW,  2)};

const size_t CMD_CASES_SIZE = sizeof(CMD_CASES) / sizeof(CMD_CASES[0]);

const int CMD_MAX_LEN = 50;

#endif /* COMMANDS_H */
