#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "input.h"
#include "commands.h"
#include "listing.h"
#include "asm_types.h"

const int MIN_LABELS_SIZE = 10;
const int MAX_LABELS_SIZE = 1000;

const int MAX_FILENAME_LEN = 100;
const int ASM_MAX_ARGS_COUNT = 2;

int SetFilenames(const char** commands_filename,
                 const char** bitecode_filename,
                 int argc, char* argv[]);

AsmErr_t CompileProgramm(InputCtx_t* commands_data);

AsmErr_t CompileCommands(InputCtx_t* commands_data,
                         CodeData_t* code_data);

int GetAsmCommand(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data);

int GetValue(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data);

int AddCommandCode(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data);

int LabelsRecalloc(CodeData_t* code_data, int new_size);

int CodeDataCtor(InputCtx_t* commands_data, CodeData_t* code_data);

int WriteBiteCode(CodeData_t* code_data, InputCtx_t* commands_data);

int WriteBiteCodePretty(CodeData_t* code_data, const char* filepath);

void AsmDestroy(InputCtx_t* commands_data, CodeData_t* code_data);

#endif /* ASSEMBLER_H */
