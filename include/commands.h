#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum Command {
    CMD_HLT = -1,
    CMD_OUT = 0,
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_MUL = 4,
    CMD_DIV = 5,
    CMD_SQRT = 6
} Command_t;

#endif /* COMMANDS_H */
