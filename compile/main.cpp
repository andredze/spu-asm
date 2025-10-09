#include "assembler.h"
#include "config.h"

int main(int argc, char* argv[])
{
    const char* commands_filename = NULL;
    const char* bitecode_filename = NULL;

    if (SetFilenames(&commands_filename, &bitecode_filename, argc, argv))
    {
        return EXIT_FAILURE;
    }

    InputCtx_t asm_commands_data =
        {.input_file_info =  {.filepath = commands_filename},
         .output_file_info = {.filepath = bitecode_filename}};

    if (CompileProgramm(&asm_commands_data) != ASM_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
