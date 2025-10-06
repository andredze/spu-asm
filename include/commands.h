#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

typedef enum Command {
    CMD_HLT = -1,
    CMD_OUT = 0,
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_MUL = 4,
    CMD_DIV = 5,
    CMD_SQRT = 6,
    CMD_IN = 7,
    CMD_TOTR = 32,
    CMD_PUSHR = 33
} Command_t;

typedef struct CommCase {
    const char* str_command;
    Command_t command;
} CommCase_t;

const CommCase_t COMM_CASES[] = {{"PUSH",  CMD_PUSH},
                                 {"ADD",   CMD_ADD},
                                 {"SUB",   CMD_SUB},
                                 {"MUL",   CMD_MUL},
                                 {"DIV",   CMD_DIV},
                                 {"SQRT",  CMD_SQRT},
                                 {"OUT",   CMD_OUT},
                                 {"HLT",   CMD_HLT},
                                 {"IN",    CMD_IN},
                                 {"TOTR",  CMD_TOTR},
                                 {"PUSHR", CMD_PUSHR}};

const size_t COMM_CASES_SIZE = sizeof(COMM_CASES) / sizeof(COMM_CASES[0]);

const int CMD_MAX_LEN = 50;

int CmdArgsCount(Command_t command);

#endif /* COMMANDS_H */
