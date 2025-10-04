#include "assembler.h"

int main()
{
    Context_t asm_commands_data =
        {.input_file_info =  {.filepath = "commands.txt"},
         .output_file_info = {.filepath = "bitecode.bin"}};

    if (CompileCalculatorProgramm(&asm_commands_data) != ASM_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
