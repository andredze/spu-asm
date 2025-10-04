#include "stack.h"
#include "input.h"
#include "maths.h"
#include "calculator.h"

int main()
{
    Context_t commands_data =
        {.input_file_info =  {.filepath = "pretty_bc.txt"},
         .output_file_info = {.filepath = "answers.txt"}};

    if (ExecuteCalculatorProgramm(&commands_data) != CALC_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
