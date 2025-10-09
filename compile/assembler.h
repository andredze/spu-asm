#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "input.h"
#include "commands.h"

typedef struct CodeData {
    int* buffer;
    size_t cur_cmd;
} CodeData_t;

typedef enum AsmErr {
    ASM_SUCCESS,
    ASM_ERROR_WITH_READING_FILE,
    ASM_OUTPUT_FILE_OPENNING_ERROR,
    ASM_GET_COMMAND_ERROR,
    ASM_SET_COMMAND_ERROR,
    ASM_CALLOC_ERROR,
    ASM_PRINT_CODE_ERROR
} AsmErr_t;

const int ASM_MAX_ARGS_COUNT = 2;

const int LABELS_COUNT = 10;

int SetFilenames(const char** commands_filename,
                 const char** bitecode_filename,
                 int argc, char* argv[]);

AsmErr_t CompileProgramm(InputCtx_t* commands_data, int labels[]);

void DPrintAsmData(CodeData_t* code_data, int labels[]);

AsmErr_t CompileCommands(InputCtx_t* commands_data,
                         CodeData_t* code_data,
                         int labels[]);

int GetAsmCommand(char* line, Command_t* command, int* value, int labels[]);

int GetValue(Command_t command, const char* line, int* value, int labels[]);

int AddCommandCode(Command_t command, int value,
                   CodeData_t* code_data, int labels[]);

int CodeDataCtor(InputCtx_t* commands_data, CodeData_t* code_data);

int WriteBiteCode(CodeData_t* code_data, InputCtx_t* commands_data);

int WriteBiteCodePretty(CodeData_t* code_data, const char* filepath);

void AsmDestroy(InputCtx_t* commands_data, CodeData_t* code_data);

#endif /* ASSEMBLER_H */
