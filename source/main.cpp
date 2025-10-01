#include "stack.h"
#include "input.h"
#include "maths.h"
#include "calculator.h"
#include "assembler.h"

int main()
{
    Context_t asm_commands_data = {.input_file_info =  {.filepath = "commands.txt"},
                                   .output_file_info = {.filepath = "bitecode.txt"}};

    if (CompileCalculatorProgramm(&asm_commands_data) != ASM_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    Context_t commands_data = {.input_file_info =  {.filepath = "bitecode.txt"},
                               .output_file_info = {.filepath = "answers.txt"}};

    if (ExecuteCalculatorProgramm(&commands_data) != CALC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
