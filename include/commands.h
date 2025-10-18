#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

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

typedef struct CommCase {
    const char* str_command;
    Command_t command;
    int args_count;
} CommCase_t;

const CommCase_t COMM_CASES[] =
    {{"HLT",   CMD_HLT,   1},
     {"PUSH",  CMD_PUSH,  2},
     {"OUT",   CMD_OUT,   1},
     {"IN",    CMD_IN,    1},
     {"CALL",  CMD_CALL,  2},
     {"RET",   CMD_RET,   1},
     {"ADD",   CMD_ADD,   1},
     {"SUB",   CMD_SUB,   1},
     {"MUL",   CMD_MUL,   1},
     {"DIV",   CMD_DIV,   1},
     {"MOD",   CMD_MOD,   1},
     {"SQRT",  CMD_SQRT,  1},
     {"SQR",   CMD_SQR,   1},
     {"JMP",   CMD_JMP,   2},
     {"JB",    CMD_JB,    2},
     {"JBE",   CMD_JBE,   2},
     {"JA",    CMD_JA,    2},
     {"JAE",   CMD_JAE,   2},
     {"JE",    CMD_JE,    2},
     {"JNE",   CMD_JNE,   2},
     {"PUSHR", CMD_PUSHR, 2},
     {"POPR",  CMD_POPR,  2},
     {"PUSHM", CMD_PUSHM, 2},
     {"POPM",  CMD_POPM,  2},
     {"DRAW",  CMD_DRAW,  2}};

const size_t COMM_CASES_SIZE = sizeof(COMM_CASES) / sizeof(COMM_CASES[0]);

const int CMD_MAX_LEN = 50;

#endif /* COMMANDS_H */
