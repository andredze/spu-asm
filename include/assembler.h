#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "input.h"
#include "commands.h"

const int LABELS_COUNT = 10;

typedef struct CodeData {
    int* buffer;
    size_t cur_cmd;
    int labels[LABELS_COUNT];
} CodeData_t;

typedef enum AsmErr {
    ASM_SUCCESS,
    ASM_ERROR_WITH_READING_FILE,
    ASM_OUTPUT_FILE_OPENNING_ERROR,
    ASM_GET_COMMAND_ERROR,
    ASM_SET_COMMAND_ERROR,
    ASM_CALLOC_ERROR,
    ASM_PRINT_CODE_ERROR,
    ASM_LISTING_ERROR,
    ASM_CREATE_LISTING_ERROR
} AsmErr_t;

typedef struct CurrCmdData {
    char* line;
    Command_t command;
    int value;
} CurrCmdData_t;

const int MAX_FILENAME_LEN = 100;
const int ASM_MAX_ARGS_COUNT = 2;

int SetFilenames(const char** commands_filename,
                 const char** bitecode_filename,
                 int argc, char* argv[]);

AsmErr_t CreateListingFile(InputCtx_t* commands_data,
                           FileInfo_t* listing_file_info);

AsmErr_t AddStringToListing(CurrCmdData_t* curr_cmd_data,
                            CodeData_t* code_data,
                            FILE* listing_stream);

AsmErr_t CompileProgramm(InputCtx_t* commands_data);

void DPrintAsmData(CodeData_t* code_data);

AsmErr_t CompileCommands(InputCtx_t* commands_data,
                         CodeData_t* code_data);

int GetAsmCommand(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data);

int GetValue(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data);

int AddCommandCode(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data);

int CodeDataCtor(InputCtx_t* commands_data, CodeData_t* code_data);

int WriteBiteCode(CodeData_t* code_data, InputCtx_t* commands_data);

int WriteBiteCodePretty(CodeData_t* code_data, const char* filepath);

void AsmDestroy(InputCtx_t* commands_data, CodeData_t* code_data);

#endif /* ASSEMBLER_H */
