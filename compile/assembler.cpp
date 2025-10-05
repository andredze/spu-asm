#include "assembler.h"

AsmErr_t CompileCalculatorProgramm(Context_t* commands_data)
{
    assert(commands_data != NULL);

    if (ReadAndParseFile(commands_data))
    {
        return ASM_ERROR_WITH_READING_FILE;
    }
    DPRINTF("lines_count = %d\n", commands_data->buffer_data.lines_count);

    CodeData_t code_data = {};

    if (CodeDataCtor(commands_data, &code_data))
    {
        return ASM_CALLOC_ERROR;
    }

    AsmErr_t error = ASM_SUCCESS;
    if ((error = CompileCommands(commands_data, &code_data)) != ASM_SUCCESS)
    {
        return error;
    }

    for (size_t i = 0; i < code_data.cur_cmd; i++)
    {
        DPRINTF("data[%zu] = %d\n", i, code_data.buffer[i]);
    }

    if (CreateBiteCode(&code_data, commands_data))
    {
        return ASM_PRINT_CODE_ERROR;
    }
    if (CreateBiteCodePretty(&code_data, "pretty_bc.txt"))
    {
        return ASM_PRINT_CODE_ERROR;
    }

    AsmDestroy(commands_data, &code_data);

    DPRINTF("\n<End of the compilator>");

    return ASM_SUCCESS;
}

AsmErr_t CompileCommands(Context_t* commands_data,
                         CodeData_t* code_data)
{
    assert(commands_data != NULL);
    assert(code_data != NULL);

    int value = 0;
    Command_t command = CMD_HLT;

    for (int i = 0; i < commands_data->ptrdata_params.lines_count; i++)
    {
        DPRINTF("\nEntering %d iteration of ptrdata for\n", i);

        if (GetAsmCommand(commands_data->ptrdata_params.ptrdata[i],
                          &command, &value))
        {
            return ASM_GET_COMMAND_ERROR;
        }
        DPRINTF("Command = %d\n", command);

        if (SetBiteCodeCommand(command, value, code_data))
        {
            return ASM_SET_COMMAND_ERROR;
        }
    }

    return ASM_SUCCESS;
}

int GetAsmCommand(char* line, Command_t* command, int* value)
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

int SetBiteCodeCommand(Command_t command, int value,
                       CodeData_t* code_data)
{
    assert(code_data != NULL);

    if (command < -1 || command > 6)
    {
        DPRINTF("<ERROR: Unknown command code in SetBiteCodeCommand()>\n");
        return 1;
    }
    if (command == CMD_PUSH)
    {
        code_data->buffer[code_data->cur_cmd++] = command;
        code_data->buffer[code_data->cur_cmd++] = value;
    }
    else
    {
        code_data->buffer[code_data->cur_cmd++] = command;
    }

    return 0;
}

int CodeDataCtor(Context_t* commands_data, CodeData_t* code_data)
{
    int lines_count = commands_data->buffer_data.lines_count;

    int* buffer = (int*) calloc(lines_count * 2, sizeof(int));
    if (buffer == NULL)
    {
        DPRINTF("Calloc failed\n");
        return 1;
    }
    code_data->buffer = buffer;
    code_data->cur_cmd = 0;

    return 0;
}

int CreateBiteCode(CodeData_t* code_data, Context_t* commands_data)
{
    if (OpenFile(&commands_data->output_file_info, "wb"))
    {
        return 1;
    }

    fwrite(&code_data->cur_cmd,
           sizeof(code_data->cur_cmd),
           1,
           commands_data->output_file_info.stream);

    if (fwrite(code_data->buffer,
               sizeof(code_data->buffer[0]),
               code_data->cur_cmd,
               commands_data->output_file_info.stream) != code_data->cur_cmd)
    {
        DPRINTF("Fwrite error\n");
        return 1;
    }
    fclose(commands_data->output_file_info.stream);

    return 0;
}

int CreateBiteCodePretty(CodeData_t* code_data, const char* filepath)
{
    FileInfo_t pretty_output_info = {.filepath = filepath};

    if (OpenFile(&pretty_output_info, "w"))
    {
        DPRINTF("Failed opening pretty\n");
        return 1;
    }

    size_t i = 0;
    while (i < code_data->cur_cmd)
    {
        if (code_data->buffer[i] == CMD_PUSH)
        {
            fprintf(pretty_output_info.stream,
                    "%d %d\n",
                    code_data->buffer[i],
                    code_data->buffer[i + 1]);
            i++;
        }
        else
        {
            fprintf(pretty_output_info.stream,
                    "%d\n",
                    code_data->buffer[i]);
        }
        i++;
    }
    fclose(pretty_output_info.stream);

    return 0;
}

void AsmDestroy(Context_t* commands_data, CodeData_t* code_data)
{
    free(code_data->buffer);
    free(commands_data->buffer_data.buffer);
    free(commands_data->ptrdata_params.ptrdata);
}
