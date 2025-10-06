#ifndef PROC_TYPES_H
#define PROC_TYPES_H

#include "commands.h"
#include <stdio.h>

typedef enum ProcErr {
    PROC_SUCCESS,
    PROC_CODE_FILE_OPENNING_ERROR,
    PROC_READING_FILE_ERROR,
    PROC_CALLOC_ERROR,
    PROC_OUTPUT_FILE_OPENNING_ERROR,
    PROC_UNKNOWN_COMMAND,
    PROC_MATH_ERROR,
    PROC_DATA_IS_NULL,
    PROC_CODE_IS_NULL,
    PROC_STACK_ERROR,
    PROC_CMD_COUNT_NEGATIVE,
    PROC_CODE_SIZE_EXCEEDS_LIMIT,
    PROC_CMD_COUNT_BIGGER_CODE_SIZE
} ProcErr_t;

typedef struct CmdCase {
    const char* str_command;
    Command_t cmd;
} CmdCase_t;

const int REGS_COUNT = 8;

typedef struct Proc {
    Stack_t stack;
    int* code;
    size_t code_size;
    int cmd_count;
    int regs[REGS_COUNT];
#ifdef PROC_DEBUG
    VarInfo_t var_info;
#endif
} Proc_t;

#endif /* PROC_TYPES_H */
