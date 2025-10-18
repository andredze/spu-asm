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
    CMD_WDRAW = 25,
    CMD_LABEL = 26,
    CMD_END
} Command_t;

typedef struct CodeParams {
    int version;
    size_t code_size;
} CodeParams_t;

const int CMD_MAX_LEN = 50;

#endif /* COMMANDS_H */
