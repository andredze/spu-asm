#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "maths.h"
#include "input.h"
#include "stack.h"
#include "commands.h"

#ifdef PROC_DEBUG
    #define PROC_INIT(name) Proc_t name = {.var_info = {#name, __FILE__, __func__, __LINE__}}
#else
    #define PROC_INIT(name) Proc_t name = {}
#endif

const size_t PROC_CODE_SIZE_LIMIT = SIZE_MAX / 32 * 30;
const int PROC_MAX_COMMAND_LEN = 200;
const size_t PROC_MIN_STACK_CAPACITY = 32;
const int REGS_COUNT = 8;

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

ProcErr_t ProcCtor(Proc_t* proc_data);

ProcErr_t ProcLoadCode(Proc_t* proc_data, const char* codepath);

ProcErr_t ProcDtor(Proc_t* proc_data);

ProcErr_t ProcVerify(Proc_t* proc_data);

ProcErr_t ProcDump(Proc_t* proc_data, ProcErr_t error);

int ProcErrToStr(ProcErr_t error, const char** error_str);

#endif /* PROCESSOR_H */
