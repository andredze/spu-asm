#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "handle_operations.h"
#include "input.h"
#include "stack.h"
#include "commands.h"
#include "spu_types.h"
#include "config.h"
#include "colors.h"
#include "cmd_cases.h"

#ifdef PROC_DEBUG
#define PROC_OK_DEBUG(proc_data)                                            \
    do {                                                                    \
        ProcErr_t error = PROC_SUCCESS;                                     \
        if ((error = ProcVerify(proc_data)) != PROC_SUCCESS)                \
        {                                                                   \
            ProcErr_t dump_err = PROC_SUCCESS;                              \
            if ((dump_err = ProcDump(proc_data, error)) != PROC_SUCCESS)    \
            {                                                               \
                return dump_err;                                            \
            }                                                               \
            return error;                                                   \
        }                                                                   \
    } while (0)
#else
#define PROC_OK_DEBUG(proc_data) ;
#endif

#ifdef PROC_DEBUG
    #define PROC_INIT(name) Proc_t name = {.var_info = {#name, __FILE__, __func__, __LINE__}}
#else
    #define PROC_INIT(name) Proc_t name = {}
#endif

int SpuSetFilenames(const char** code_filename,
                    int argc, char* argv[]);

ProcErr_t ProcCtor(Proc_t* proc_data);

ProcErr_t ProcLoadCode(Proc_t* proc_data, const char* codepath);

ProcErr_t ProcLoadPrettyBC(Proc_t* proc_data, const char* codepath);

ProcErr_t ProcExecuteCommands(Proc_t* proc_data);

ProcErr_t ProcGetCommand(Proc_t* proc_data, Command_t* command);

ProcErr_t ProcExecuteOperation(Proc_t* proc_data, Command_t command, int* break_loop);

ProcErr_t ProcDtor(Proc_t* proc_data);

ProcErr_t ProcVerify(Proc_t* proc_data);

ProcErr_t ProcDump(Proc_t* proc_data, ProcErr_t error);

ProcErr_t ProcErrToStr(ProcErr_t error, const char** error_str);

int ProcConsoleDump(Proc_t* proc_data);

#endif /* PROCESSOR_H */
