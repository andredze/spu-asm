#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "maths.h"
#include "input.h"
#include "stack.h"

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

typedef enum Command {
    CMD_HLT = -1,
    CMD_OUT = 0,
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_MUL = 4,
    CMD_DIV = 5,
    CMD_SQRT = 6
} Command_t;

typedef struct CommCase {
    const char* str_command;
    Command_t command;
} CommCase_t;

CalcErr_t ExecuteCalculatorProgramm(Context_t* commands_data);

CalcErr_t ExecuteCommands(Context_t* commands_data);

int GetCommand(char* line, Command_t* command, int* value);

int RunCommand(Stack_t* calc_stack, Command_t command,
               int value, FILE* output_stream);

int HandleOut(Stack_t* calc_stack, FILE* output_stream);

#endif /* CALCULATOR_H */
