#include "assembler.h"
#include "config.h"

int SetFilenames(const char** commands_filename,
                 const char** bitecode_filename,
                 int argc, char* argv[])
{
    if (argc == 3)
    {
        *commands_filename = argv[1];
        *bitecode_filename = argv[2];
    }
    else if (argc == 2)
    {
        *commands_filename = argv[1];
        *bitecode_filename = BINARY_BITECODE_FILENAME;
    }
    else if (argc == 1)
    {
        *commands_filename = COMMANDS_FILENAME;
        *bitecode_filename = BINARY_BITECODE_FILENAME;
    }
    else
    {
        printf("Too much arguments given, maximum 2 (current arguments = %d)\n", argc);
        return 1;
    }

    return 0;
}

void DPrintAsmData(CodeData_t* code_data)
{
    DPRINTF("\n-----------------------------------\n");
    DPRINTF("code = ");
    for (size_t i = 0; i < code_data->cur_cmd; i++)
    {
        DPRINTF("%d, ", code_data->buffer[i]);
    }
    DPRINTF("\n");
    DPRINTF("labels = ");
    for (int i = 0; i < LABELS_COUNT; i++)
    {
        DPRINTF("%d, ", code_data->labels[i]);
    }
    DPRINTF("\n");
    DPRINTF("-----------------------------------\n");
}

AsmErr_t CompileProgramm(InputCtx_t* commands_data)
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

    DPrintAsmData(&code_data);

    code_data.cur_cmd = 0;
    if ((error = CompileCommands(commands_data, &code_data)) != ASM_SUCCESS)
    {
        return error;
    }

    DPrintAsmData(&code_data);

    if (WriteBiteCode(&code_data, commands_data))
    {
        return ASM_PRINT_CODE_ERROR;
    }
    if (WriteBiteCodePretty(&code_data, READABLE_BITECODE_FILENAME))
    {
        return ASM_PRINT_CODE_ERROR;
    }

    AsmDestroy(commands_data, &code_data);

    DPRINTF("\n<End of the compilator>");

    return ASM_SUCCESS;
}

AsmErr_t CompileCommands(InputCtx_t* commands_data,
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
                          &command, &value, code_data))
        {
            return ASM_GET_COMMAND_ERROR;
        }
        DPRINTF("Command = %d\n", command);

        if (AddCommandCode(command, value, code_data))
        {
            return ASM_SET_COMMAND_ERROR;
        }
    }

    return ASM_SUCCESS;
}

int GetAsmCommand(char* line, Command_t* command, int* value, CodeData_t* code_data)
{
    assert(line != NULL);
    assert(code_data != NULL);
    assert(command != NULL);

    char operation[CMD_MAX_LEN] = {};

    if (sscanf(line, "%s", operation) != 1)
    {
        DPRINTF("sscanf failed\n");
        return 1;
    }
    if (sscanf(line, ":%d", value) == 1)
    {
        *command = CMD_LABEL;
        return 0;
    }
    for (size_t i = 0; i < COMM_CASES_SIZE; i++)
    {
        if (strcmp(operation, COMM_CASES[i].str_command) == 0)
        {
            *command = COMM_CASES[i].command;
            break;
        }
    }
    if (CmdArgsCount(*command) == 1)
    {
        if (GetValue(*command, line, value, code_data))
        {
            return 1;
        }
    }

    return 0;
}

int GetValue(Command_t command, const char* line, int* value, CodeData_t* code_data)
{
    assert(code_data != NULL);

    DPRINTF("cmd = %d\n", command);
    char operation[CMD_MAX_LEN] = {};
    int label = 0;

    if (sscanf(line, "%s :%d", operation, &label) == 2)
    {
        // TODO: check if label is valid
        *value = code_data->labels[label];
        // TODO: if command not jump - return error;
        return 0;
    }
    else if (command == CMD_PUSHR || command == CMD_POPR)
    {
        char reg[CMD_MAX_LEN] = {};
        if (sscanf(line, "%s %s", operation, reg) != 2)
        {
            DPRINTF("Wrong number of args for reg cmd\n");
            return 1;
        }
        *value = reg[1] - 'A';
        return 0;
    }
    else
    {
        if (sscanf(line, "%s %d", operation, value) != 2)
        {
            DPRINTF("Wrong number of args for push\n");
            return 1;
        }
        return 0;
    }

    return 1;
}

int AddCommandCode(Command_t command, int value,
                   CodeData_t* code_data)
{
    assert(code_data != NULL);

    // TODO: check if number is in enum
    if (command < -1 || command > 33)
    {
        DPRINTF("<ERROR: Unknown command code in AddCommandCode()>\n");
        return 1;
    }
    if (command == CMD_LABEL)
    {
        // labels in size_t
        code_data->labels[value] = (int) code_data->cur_cmd;
        return 0;
    }

    code_data->buffer[code_data->cur_cmd++] = command;

    if (CmdArgsCount(command) == 1)
    {
        if (command == CMD_POPR || command == CMD_PUSHR)
        {
            code_data->buffer[code_data->cur_cmd++] = value;
        }
        else
        {
            code_data->buffer[code_data->cur_cmd++] = value;
        }
    }

    return 0;
}

int CodeDataCtor(InputCtx_t* commands_data, CodeData_t* code_data)
{
    int lines_count = commands_data->buffer_data.lines_count;

    int* buffer = (int*) calloc(lines_count * ASM_MAX_ARGS_COUNT, sizeof(int));
    if (buffer == NULL)
    {
        DPRINTF("Calloc failed\n");
        return 1;
    }
    code_data->buffer = buffer;
    code_data->cur_cmd = 0;

    for (int i = 0; i < LABELS_COUNT; i++)
    {
        code_data->labels[i] = -1;
    }

    return 0;
}

int WriteBiteCode(CodeData_t* code_data, InputCtx_t* commands_data)
{
    if (OpenFile(&commands_data->output_file_info, "wb"))
    {
        return 1;
    }

    CodeParams_t code_params = {.version = CODE_VERSION,
                                .code_size = code_data->cur_cmd};
    if (fwrite(&code_params,
               sizeof(code_params),
               1,
               commands_data->output_file_info.stream) != 1)
    {
        DPRINTF("Code_params writing in file error\n");
        return 1;
    }

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

int WriteBiteCodePretty(CodeData_t* code_data, const char* filepath)
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
        if (CmdArgsCount((Command_t) code_data->buffer[i]) == 1)
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

void AsmDestroy(InputCtx_t* commands_data, CodeData_t* code_data)
{
    free(code_data->buffer);
    free(commands_data->buffer_data.buffer);
    free(commands_data->ptrdata_params.ptrdata);
}
