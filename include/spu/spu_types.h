#ifndef SPU_TYPES_H
#define SPU_TYPES_H

#include "commands.h"
#include <stdio.h>

typedef enum ProcErr {
    PROC_SUCCESS,
    PROC_CODE_FILE_OPENNING_ERROR,
    PROC_READING_FILE_ERROR,
    PROC_CALLOC_ERROR,
    PROC_OUTPUT_FILE_OPENNING_ERROR,
    PROC_UNKNOWN_COMMAND,
    PROC_EXECUTE_OP_ERROR,
    PROC_MATH_ERROR,
    PROC_DATA_IS_NULL,
    PROC_CODE_IS_NULL,
    PROC_STACK_ERROR,
    PROC_CMD_COUNT_EXCEEDS_LIMIT,
    PROC_CODE_SIZE_EXCEEDS_LIMIT,
    PROC_CMD_COUNT_BIGGER_CODE_SIZE,
    PROC_DUMP_ERR,
    PROC_ERR_END
} ProcErr_t;

const int REGS_COUNT = 8;
const size_t RAM_SIDE_SIZE = 20;
const size_t RAM_SIZE = RAM_SIDE_SIZE * RAM_SIDE_SIZE;
const size_t MAX_RAM_ADDRESS = SIZE_MAX / 32 * 20;
const size_t PROC_CODE_SIZE_LIMIT = SIZE_MAX / 32 * 30;
const size_t PROC_MIN_STACK_CAPACITY = 32;

typedef struct Proc {
    Stack_t stack;
    int* code;
    size_t code_size;
    size_t cmd_count;
    int regs[REGS_COUNT];
    Stack_t call_stack;
    int* ram;
#ifdef PROC_DEBUG
    VarInfo_t var_info;
#endif
} Proc_t;

const char* const PROC_ERR_STRS[] =
    {"Proc_data is OK",
     "Error with opening code file",
     "Error with reading file",
     "Error with memory allocation",
     "Error with opening output file",
     "Unknown command",
     "Error with math",
     "proc_data is null ptr",
     "proc_data.code is null ptr",
     "Error with proc_data.stack, check stack logs",
     "Proc_data.cmd_count is bigger than limit (probably was set to negative)",
     "Proc_data.code_size is bigger than limit (probably was set to negative)",
     "Proc_data.cmd_count is bigger than code_size"};

#endif /* SPU_TYPES_H */
