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

    Proc_t proc_data = {};

    if (ProcCtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if (ProcLoadCode(&proc_data, code_filename) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

#ifdef PROC_DEBUG
    if (ProcConsoleDump(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
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
