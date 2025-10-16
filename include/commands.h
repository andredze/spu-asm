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
    CMD_SQRT  = 10,
    CMD_JMP   = 11,
    CMD_JB    = 12,
    CMD_JBE   = 13,
    CMD_JA    = 14,
    CMD_JAE   = 15,
    CMD_JE    = 16,
    CMD_JNE   = 17,
    CMD_PUSHR = 18,
    CMD_POPR  = 19,
    CMD_PUSHM = 20,
    CMD_POPM  = 21,
    CMD_LABEL = 22,
    CMD_END
} Command_t;

typedef struct CodeParams {
    int version;
    size_t code_size;
} CodeParams_t;

typedef struct CommCase {
    const char* str_command;
    Command_t command;
} CommCase_t;

const CommCase_t COMM_CASES[] =
    {{"HLT",   CMD_HLT},
     {"PUSH",  CMD_PUSH},
     {"OUT",   CMD_OUT},
     {"IN",    CMD_IN},
     {"CALL",  CMD_CALL},
     {"RET",   CMD_RET},
     {"ADD",   CMD_ADD},
     {"SUB",   CMD_SUB},
     {"MUL",   CMD_MUL},
     {"DIV",   CMD_DIV},
     {"SQRT",  CMD_SQRT},
     {"JMP",   CMD_JMP},
     {"JB",    CMD_JB},
     {"JBE",   CMD_JBE},
     {"JA",    CMD_JA},
     {"JAE",   CMD_JAE},
     {"JE",    CMD_JE},
     {"JNE",   CMD_JNE},
     {"PUSHR", CMD_PUSHR},
     {"POPR",  CMD_POPR},
     {"PUSHM", CMD_PUSHM},
     {"POPM",  CMD_POPM}};

const size_t COMM_CASES_SIZE = sizeof(COMM_CASES) / sizeof(COMM_CASES[0]);

const int CMD_MAX_LEN = 50;

int CmdArgsCount(Command_t command);

#endif /* COMMANDS_H */
