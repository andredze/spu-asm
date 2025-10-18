#ifndef ASM_COMMANDS_H
#define ASM_COMMANDS_H

#include "add_operations.h"
#include "asm_types.h"
#include "commands.h"

typedef struct AsmCmdCase {
    const char* str_command;
    Command_t command;
    AsmErr_t (*add_op) (CmdCtx_t* cmd_ctx, AsmCtx_t* asm_ctx);
    int args_count;
} AsmCmdCase_t;

const AsmCmdCase_t ASM_CMD_CASES[] = {
    {"HLT",   CMD_HLT,   AddDefaultOp,   1},
    {"PUSH",  CMD_PUSH,  AddNormalArgOp, 2},
    {"OUT",   CMD_OUT,   AddDefaultOp,   1},
    {"IN",    CMD_IN,    AddDefaultOp,   1},
    {"CALL",  CMD_CALL,  AddLabelArgOp,  2},
    {"RET",   CMD_RET,   AddDefaultOp,   1},
    {"ADD",   CMD_ADD,   AddDefaultOp,   1},
    {"SUB",   CMD_SUB,   AddDefaultOp,   1},
    {"MUL",   CMD_MUL,   AddDefaultOp,   1},
    {"DIV",   CMD_DIV,   AddDefaultOp,   1},
    {"MOD",   CMD_MOD,   AddDefaultOp,   1},
    {"SQRT",  CMD_SQRT,  AddDefaultOp,   1},
    {"SQR",   CMD_SQR,   AddDefaultOp,   1},
    {"JMP",   CMD_JMP,   AddLabelArgOp,  2},
    {"JB",    CMD_JB,    AddLabelArgOp,  2},
    {"JBE",   CMD_JBE,   AddLabelArgOp,  2},
    {"JA",    CMD_JA,    AddLabelArgOp,  2},
    {"JAE",   CMD_JAE,   AddLabelArgOp,  2},
    {"JE",    CMD_JE,    AddLabelArgOp,  2},
    {"JNE",   CMD_JNE,   AddLabelArgOp,  2},
    {"PUSHR", CMD_PUSHR, AddRegArgOp,    2},
    {"POPR",  CMD_POPR,  AddRegArgOp,    2},
    {"PUSHM", CMD_PUSHM, AddRamArgOp,    2},
    {"POPM",  CMD_POPM,  AddRamArgOp,    2},
    {"DRAW",  CMD_DRAW,  AddNormalArgOp, 2}
};

const size_t ASM_CMD_CASES_SIZE = sizeof(ASM_CMD_CASES) / sizeof(ASM_CMD_CASES[0]);

#endif /* ASM_COMMANDS_H */
