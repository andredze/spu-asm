#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "input.h"

const int ASM_MAX_COMMAND_LEN = 200;

typedef enum AsmErr {
    ASM_SUCCESS,
    ASM_ERROR_WITH_READING_FILE,
    ASM_OUTPUT_FILE_OPENNING_ERROR,
    ASM_GET_COMMAND_ERROR,
    ASM_SET_COMMAND_ERROR
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

AsmErr_t CompileCalculatorProgramm(Context_t* commands_data);

AsmErr_t CompileCommands(Context_t* commands_data);

int GetAsmCommand(char* line, AsmCommand_t* command, int* value);

int SetBiteCodeCommand(AsmCommand_t command, int value, FILE* output_stream);

#endif /* ASSEMBLER_H */
