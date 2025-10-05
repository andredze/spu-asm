#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "maths.h"
#include "input.h"
#include "stack.h"
#include "commands.h"

const int CALC_MAX_COMMAND_LEN = 200;
const size_t CALC_MIN_STACK_CAPACITY = 32;

typedef enum CalcErr {
    CALC_SUCCESS,
    CALC_ERROR_WITH_READING_FILE,
    CALC_STACK_ERROR,
    CALC_OUTPUT_FILE_OPENNING_ERROR,
    CALC_UNKNOWN_COMMAND,
    CALC_MATH_ERROR
} CalcErr_t;

typedef struct CommCase {
    const char* str_command;
    Command_t command;
} CommCase_t;

CalcErr_t ExecutePrettyBCProgramm(Context_t* commands_data);

CalcErr_t ExecuteCommands(Context_t* commands_data);

int GetCommand(char* line, Command_t* command, int* value);

int RunCommand(Stack_t* calc_stack, Command_t command,
               int value, FILE* output_stream);

int HandleOut(Stack_t* calc_stack, FILE* output_stream);

#endif /* CALCULATOR_H */
