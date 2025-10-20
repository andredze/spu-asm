#include <TXLib.h>
#include "assembler.h"
#include "config.h"

int main(int argc, char* argv[])
{
    printf(BLUE "Compiling programm: " RESET_CLR);

    SetHashInCmdCases();

    qsort(CMD_CASES, CMD_CASES_SIZE, sizeof(CMD_CASES[0]), AsmCmdCasesCompare);

    const char* commands_filename = NULL;
    const char* bytecode_filename = NULL;
    int listing_flag = 0;

    if (SetFilenames(&commands_filename, &bytecode_filename,
                     argc, argv, &listing_flag))
    {
        return EXIT_FAILURE;
    }
    printf(BLUE "%s\n" RESET_CLR, commands_filename);

    InputCtx_t asm_input_ctx =
        {.input_file_info =  {.filepath = commands_filename},
         .output_file_info = {.filepath = bytecode_filename}};

    if (CompileProgramm(&asm_input_ctx, listing_flag) != ASM_SUCCESS)
    {
        printf(RED "ERROR: Compilation failed\n" RESET_CLR);
        return EXIT_FAILURE;
    }

    printf(GREEN "Compilation success! code file: %s" RESET_CLR, bytecode_filename);

    return EXIT_SUCCESS;
}
