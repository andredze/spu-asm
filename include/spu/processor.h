#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "operations.h"
#include "input.h"
#include "stack.h"
#include "commands.h"
#include "spu_types.h"
#include "config.h"

#ifdef PROC_DEBUG
#define PROC_OK_DEBUG(proc_data) \
    do { \
        ProcErr_t error = PROC_SUCCESS; \
        if ((error = ProcVerify(proc_data)) != PROC_SUCCESS) \
        { \
            ProcErr_t dump_err = PROC_SUCCESS; \
            if ((dump_err = ProcDump(proc_data, error)) != PROC_SUCCESS) \
            { \
                return dump_err; \
            } \
            return error; \
        } \
    } while (0)
#else
#define PROC_OK_DEBUG(proc_data) ;
#endif

#ifdef PROC_DEBUG
    #define PROC_INIT(name) Proc_t name = {.var_info = {#name, __FILE__, __func__, __LINE__}}
#else
    #define PROC_INIT(name) Proc_t name = {}
#endif

const size_t PROC_CODE_SIZE_LIMIT = SIZE_MAX / 32 * 30;
const size_t PROC_MIN_STACK_CAPACITY = 32;

ProcErr_t ProcCtor(Proc_t* proc_data);

ProcErr_t ProcLoadCode(Proc_t* proc_data, const char* codepath);

ProcErr_t ProcLoadPrettyBC(Proc_t* proc_data, const char* codepath);

ProcErr_t ProcExecuteCommands(Proc_t* proc_data, FILE* stream);

ProcErr_t ProcGetCommand(Proc_t* proc_data,
                         Command_t* command,
                         int* value);

int ProcRunCommand(Proc_t* proc_data, Command_t command,
                   int value, FILE* output_stream);

int ProcHandleOut(Stack_t* stack, FILE* output_stream);

ProcErr_t ProcDtor(Proc_t* proc_data, FILE* stream);

ProcErr_t ProcVerify(Proc_t* proc_data);

ProcErr_t ProcDump(Proc_t* proc_data, ProcErr_t error);

int ProcErrToStr(ProcErr_t error, const char** error_str);

#endif /* PROCESSOR_H */
