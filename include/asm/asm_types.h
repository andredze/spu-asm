#ifndef ASM_TYPES_H
#define ASM_TYPES_H

#include "commands.h"
#include <stdio.h>

typedef struct CodeData {
    int* buffer;
    size_t cur_cmd;
    int* labels;
    int labels_size;
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
    ASM_CREATE_LISTING_ERROR,
    ASM_GET_LABEL_ERROR,
    ASM_GET_REG_ERROR,
    ASM_GET_OP_ARG_ERR
} AsmErr_t;

typedef struct CurrCmdData {
    char* line;
    Command_t command;
    int value;
} CurrCmdData_t;

#endif /* ASM_TYPES_H */
