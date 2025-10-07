#include "assembler.h"
#include "config.h"

int main()
{
    Context_t asm_commands_data =
        {.input_file_info =  {.filepath = COMMANDS_FILENAME},
         .output_file_info = {.filepath = BINARY_BITECODE_FILENAME}};

    if (CompileProgramm(&asm_commands_data) != ASM_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
