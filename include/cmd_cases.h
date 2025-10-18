#ifndef CMD_CASES_H
#define CMD_CASES_H

#include "commands.h"

#ifdef ASM
    #include "add_operations.h"
#endif
#ifdef SPU
    #include "handle_operations.h"
#endif

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

#endif /* CMD_CASES_H */
