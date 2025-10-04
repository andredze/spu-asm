#include "assembler.h"

AsmErr_t CompileCalculatorProgramm(Context_t* commands_data)
{
    assert(commands_data != NULL);

    if (ReadAndParseFile(commands_data))
    {
        return ASM_ERROR_WITH_READING_FILE;
    }
    DPRINTF("lines_count = %d\n", commands_data->buffer_data.lines_count);

    if (OpenFile(&commands_data->output_file_info, "w"))
    {
        return ASM_OUTPUT_FILE_OPENNING_ERROR;
    }

    AsmErr_t error = ASM_SUCCESS;
    if ((error = CompileCommands(commands_data)) != ASM_SUCCESS)
    {
        return error;
    }

    free(commands_data->buffer_data.buffer);
    free(commands_data->ptrdata_params.ptrdata);
    fclose(commands_data->output_file_info.stream);
    DPRINTF("\n<End of the compilator>");

    return ASM_SUCCESS;
}

AsmErr_t CompileCommands(Context_t* commands_data)
{
    assert(commands_data != NULL);

    int value = 0;
    AsmCommand_t command = ASM_CMD_HLT;

    for (int i = 0; i < commands_data->ptrdata_params.lines_count; i++)
    {
        DPRINTF("\nEntering %d iteration of ptrdata for\n", i);

        if (GetAsmCommand(commands_data->ptrdata_params.ptrdata[i],
                       &command, &value))
        {
            return ASM_GET_COMMAND_ERROR;
        }
        DPRINTF("Command = %d\n", command);

        if (SetBiteCodeCommand(command, value,
                               commands_data->output_file_info.stream))
        {
            return ASM_SET_COMMAND_ERROR;
        }
    }

    return ASM_SUCCESS;
}

int GetAsmCommand(char* line, AsmCommand_t* command, int* value)
{
    assert(line != NULL);
    assert(command != NULL);

    char operation[ASM_MAX_COMMAND_LEN] = {};

    int arg_count = sscanf(line, "%s %d", operation, value);
    if (arg_count < 1 || arg_count > 2)
    {
        DPRINTF("Invalid number of arguments for command\n"
                        "arg_count = %d; expected = 2", arg_count);
        return 1;
    }

    for (size_t i = 0; i < COMM_CASES_SIZE; i++)
    {
        if (strcmp(operation, COMM_CASES[i].str_command) == 0)
        {
            *command = COMM_CASES[i].command;
            return 0;
        }
    }

    DPRINTF("Unknown calc command\n");
    return 1;
}

int SetBiteCodeCommand(AsmCommand_t command, int value, FILE* output_stream)
{
    assert(output_stream != NULL);

    if (command < -1 || command > 6)
    {
        DPRINTF("<ERROR: Unknown command code in SetBiteCodeCommand()>\n");
        return 1;
    }
    if (command == ASM_CMD_PUSH)
    {
        fprintf(output_stream, "%d %d\n", command, value);
    }
    else
    {
        fprintf(output_stream, "%d\n", command);
    }

    return 0;
}
