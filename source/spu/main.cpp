#include "stack.h"
#include "input.h"
#include "operations.h"
#include "processor.h"
#include "config.h"

int main(int argc, char* argv[])
{
    const char* code_filename = NULL;

    if (SpuSetFilenames(&code_filename, argc, argv))
    {
        return EXIT_FAILURE;
    }
    DPRINTF("filename = %s\n", code_filename);

    PROC_INIT(proc_data);

    if (ProcCtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    // if (ProcLoadPrettyBC(&proc_data, READABLE_BYTECODE_FILENAME) != PROC_SUCCESS)
    // {
    //     return EXIT_FAILURE;
    // }
    if (ProcLoadCode(&proc_data, code_filename) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    DPRINTF("proc_data.code:\n");
    for (size_t i = 0; i < proc_data.code_size; i++)
    {
        DPRINTF("%d, ", proc_data.code[i]);
    }
    DPRINTF("\n");

#ifdef PROC_DEBUG
    if (ProcVerify(&proc_data) != PROC_SUCCESS)
    {
        DPRINTF("Proc verify failed\n");
        return EXIT_FAILURE;
    }
    ProcDump(&proc_data, PROC_SUCCESS);
#endif /* PROC_DEBUG */

    if (ProcExecuteCommands(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if (ProcDtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
