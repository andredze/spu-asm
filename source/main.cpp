#include "stack.h"
#include "input.h"
#include "maths.h"
// #include "calculator.h"
#include "processor.h"

int main()
{
//     Context_t commands_data =
//         {.input_file_info =  {.filepath = "pretty_bc.txt"},
//          .output_file_info = {.filepath = "answers.txt"}};
//
//     if (ExecuteCalculatorProgramm(&commands_data) != CALC_SUCCESS)
//     {
//         return EXIT_FAILURE;
    // }

    DPRINTF("Programm start\n");
    PROC_INIT(proc_data);
    DPRINTF("Proc initialized\n");
    if (ProcCtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }
    if (ProcLoadCode(&proc_data, "bitecode.bin") != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

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

    if (ProcDtor(&proc_data) != PROC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
