#include "assembler.h"
#include "config.h"

int main(int argc, char* argv[])
{
    if (SetHashInCmdCases())
    {
        return EXIT_FAILURE;
    }
    if (qsort(CMD_CASES, CMD_CASES_SIZE, sizeof(CMD_CASES[0]), AsmCmdCasesCompare))
    {
        printf("qsort() for cmd_cases failed\n");
        return EXIT_FAILURE;
    }

    const char* commands_filename = NULL;
    const char* bytecode_filename = NULL;
    int listing_flag = 0;

    if (SetFilenames(&commands_filename, &bytecode_filename,
                     argc, argv, &listing_flag))
    {
        return EXIT_FAILURE;
    }

    InputCtx_t asm_input_ctx =
        {.input_file_info =  {.filepath = commands_filename},
         .output_file_info = {.filepath = bytecode_filename}};

    if (CompileProgramm(&asm_input_ctx, listing_flag) != ASM_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
