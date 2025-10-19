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
    int hash;
#endif /* ASM */
} CmdCase_t;

#ifdef SPU
    #define SET_CMD_CASE(name, args_count)   {#name, CMD_ ## name, (args_count), Handle ## name}
#endif /* SPU */
#ifdef ASM
    #define SET_CMD_CASE(name, args_count)   {#name, CMD_ ## name, (args_count), AddOp ## name, 0}
#endif /* ASM */
#ifndef SET_CMD_CASE
    #define SET_CMD_CASE(name, args_count)   {#name, CMD_ ## name, (args_count)}
#endif /* SET_CMD_CASE */

extern CmdCase_t CMD_CASES[];

extern const size_t CMD_CASES_SIZE;

#endif /* CMD_CASES_H */
