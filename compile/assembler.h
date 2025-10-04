#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "input.h"

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

typedef enum AsmCommand {
    ASM_CMD_HLT = -1,
    ASM_CMD_OUT = 0,
    ASM_CMD_PUSH = 1,
    ASM_CMD_ADD = 2,
    ASM_CMD_SUB = 3,
    ASM_CMD_MUL = 4,
    ASM_CMD_DIV = 5,
    ASM_CMD_SQRT = 6
} AsmCommand_t;

typedef struct AsmCommCase {
    const char* str_command;
    AsmCommand_t command;
} AsmCommCase_t;

const AsmCommCase_t COMM_CASES[] = {{"PUSH", ASM_CMD_PUSH},
                                    {"ADD",  ASM_CMD_ADD},
                                    {"SUB",  ASM_CMD_SUB},
                                    {"MUL",  ASM_CMD_MUL},
                                    {"DIV",  ASM_CMD_DIV},
                                    {"SQRT", ASM_CMD_SQRT},
                                    {"OUT",  ASM_CMD_OUT},
                                    {"HLT",  ASM_CMD_HLT}};

const size_t COMM_CASES_SIZE = sizeof(COMM_CASES) / sizeof(COMM_CASES[0]);

AsmErr_t CompileCalculatorProgramm(Context_t* commands_data);

AsmErr_t CompileCommands(Context_t* commands_data,
                         CodeData_t* code_data);

int GetAsmCommand(char* line, AsmCommand_t* command, int* value);

int SetBiteCodeCommand(AsmCommand_t command, int value,
                       CodeData_t* code_data);

int CodeDataCtor(Context_t* commands_data, CodeData_t* code_data);

int CreateBiteCode(CodeData_t* code_data, Context_t* commands_data);

int CreateBiteCodePretty(CodeData_t* code_data, const char* filepath);

void AsmDestroy(Context_t* commands_data, CodeData_t* code_data);

#endif /* ASSEMBLER_H */
