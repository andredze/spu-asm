#include "stack.h"
#include "input.h"
#include "maths.h"
#include "processor.h"

int main()
{
    DPRINTF("Programm start\n");
    PROC_INIT(proc_data);
    DPRINTF("Proc initialized\n");
    if (ProcCtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if (ProcLoadPrettyBC(&proc_data, "pretty_bc.txt") != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    // if (ProcLoadCode(&proc_data, "bitecode.bin") != PROC_SUCCESS)
    // {
    //     return EXIT_FAILURE;
    // }

    DPRINTF("proc_data.code:\n");
    for (size_t i = 0; i < proc_data.code_size; i++)
    {
        DPRINTF("%d, ", proc_data.code[i]);
    }
    DPRINTF("\n");

    if (ProcVerify(&proc_data) != PROC_SUCCESS)
    {
        DPRINTF("Proc verify failed\n");
        return EXIT_FAILURE;
    }

    ProcDump(&proc_data, PROC_SUCCESS);

    FILE* stream = fopen("answers.txt", "w");
    if (stream == NULL)
    {
        printf("Error opening output file\n");
        return EXIT_FAILURE;
    }

    if (ProcExecuteCommands(&proc_data, stream) != PROC_SUCCESS)
    {
        fclose(stream);
        return EXIT_FAILURE;
    }

    if (ProcDtor(&proc_data, stream) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
