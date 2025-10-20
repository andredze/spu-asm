#include <TXLib.h>
#include "assembler.h"
#include "config.h"

int main(int argc, char* argv[])
{
    printf(BLUE "Compiling programm...\n" RESET_CLR);

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

    InputCtx_t asm_input_ctx =
        {.input_file_info =  {.filepath = commands_filename},
         .output_file_info = {.filepath = bytecode_filename}};

    if (CompileProgramm(&asm_input_ctx, listing_flag) != ASM_SUCCESS)
    {
        printf(RED "Compilation error\n" RESET_CLR);
        return EXIT_FAILURE;
    }

    printf(GREEN "Compilation success" RESET_CLR);

    return EXIT_SUCCESS;
}
