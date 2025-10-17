#ifndef ASM_TYPES_H
#define ASM_TYPES_H

#include <stdio.h>
#include "commands.h"

typedef struct AsmCtx {
    int* buffer;
    size_t cur_cmd;
    size_t* labels;
    int labels_size;
} AsmCtx_t;

typedef struct CmdCtx {
    char* line;
    Command_t command;
    int value;
} CmdCtx_t;

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
    ASM_GET_OP_ARG_ERR,
    ASM_GET_RAM_ARG_ERROR,
    ASM_LABEL_CASE_ERROR,
    ASM_GET_CMD_ERROR,
    ASM_ADD_OP_ERROR,
    ASM_SYNTAX_ERROR,
    ASM_LABELS_RECALLOC_ERROR,
    ASM_LABEL_ERROR
} AsmErr_t;

#endif /* ASM_TYPES_H */
