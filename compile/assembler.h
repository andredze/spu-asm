#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "input.h"
#include "commands.h"

const int ASM_MAX_COMMAND_LEN = 200;

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

typedef struct AsmCommCase {
    const char* str_command;
    Command_t command;
} AsmCommCase_t;

const AsmCommCase_t COMM_CASES[] = {{"PUSH", CMD_PUSH},
                                    {"ADD",  CMD_ADD},
                                    {"SUB",  CMD_SUB},
                                    {"MUL",  CMD_MUL},
                                    {"DIV",  CMD_DIV},
                                    {"SQRT", CMD_SQRT},
                                    {"OUT",  CMD_OUT},
                                    {"HLT",  CMD_HLT}};

const size_t COMM_CASES_SIZE = sizeof(COMM_CASES) / sizeof(COMM_CASES[0]);

AsmErr_t CompileCalculatorProgramm(Context_t* commands_data);

AsmErr_t CompileCommands(Context_t* commands_data,
                         CodeData_t* code_data);

int GetAsmCommand(char* line, Command_t* command, int* value);

int SetBiteCodeCommand(Command_t command, int value,
                       CodeData_t* code_data);

int CodeDataCtor(Context_t* commands_data, CodeData_t* code_data);

int CreateBiteCode(CodeData_t* code_data, Context_t* commands_data);

int CreateBiteCodePretty(CodeData_t* code_data, const char* filepath);

void AsmDestroy(Context_t* commands_data, CodeData_t* code_data);

#endif /* ASSEMBLER_H */
