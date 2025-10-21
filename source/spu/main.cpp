#include "processor.h"
#include "config.h"

int main(int argc, char* argv[])
{
    char buffer[RAM_SIZE + RAM_HEIGHT] = {};

    if (setvbuf(stdout, buffer, _IOFBF, sizeof(buffer)) != 0)
    {
        DPRINTF("Setvbuffer size failed\n");
        return 1;
    }

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
