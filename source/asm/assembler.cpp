#include "assembler.h"
#include "config.h"

// TODO: пропуск пробельных символов

int SetFilenames(const char** commands_filename,
                 const char** bytecode_filename,
                 int argc, char* argv[])
{
    switch (argc)
    {
        case 3:
            *commands_filename = argv[1];
            *bytecode_filename = argv[2];
            break;
        case 2:
            *commands_filename = argv[1];
            *bytecode_filename = BINARY_BYTECODE_FILENAME;
            break;
        case 1:
            *commands_filename = COMMANDS_FILENAME;
            *bytecode_filename = BINARY_BYTECODE_FILENAME;
            break;
        default:
            printf("Too much arguments given, maximum 2 (current arguments = %d)\n", argc);
            return 1;
    }

    return 0;
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

    free(code_data.labels);

    if (WriteByteCode(&code_data, commands_data))
    {
        return ASM_PRINT_CODE_ERROR;
    }
    if (WriteByteCodePretty(&code_data, READABLE_BYTECODE_FILENAME))
    {
        return ASM_PRINT_CODE_ERROR;
    }

    AsmDestroy(commands_data, &code_data);
    DPRINTF("\n<End of the compilator>");

    return ASM_SUCCESS;
}

int CodeDataCtor(InputCtx_t* commands_data, CodeData_t* code_data)
{
    int lines_count = commands_data->buffer_data.lines_count;

    int* buffer = (int*) calloc(lines_count * ASM_MAX_ARGS_COUNT, sizeof(int));
    if (buffer == NULL)
    {
        DPRINTF("Buffer calloc failed\n");
        return 1;
    }
    code_data->buffer = buffer;
    code_data->cur_cmd = 0;

    int* labels = (int*) calloc(MIN_LABELS_SIZE, sizeof(int));
    if (labels == NULL)
    {
        DPRINTF("Labels calloc failed\n");
        return 1;
    }
    code_data->labels = labels;
    code_data->labels_size = MIN_LABELS_SIZE;

    for (int i = 0; i < MIN_LABELS_SIZE; i++)
    {
        code_data->labels[i] = -1;
    }
    DPrintAsmData(code_data);

    return 0;
}

AsmErr_t CompileCommands(InputCtx_t* commands_data,
                         CodeData_t* code_data)
{
    assert(commands_data != NULL);
    assert(code_data != NULL);

#ifdef LISTING
    FileInfo_t listing_file_info = {};
    if (CreateListingFile(commands_data, &listing_file_info))
    {
        return ASM_CREATE_LISTING_ERROR;
    }
#endif /* LISTING */

    CurrCmdData_t curr_cmd_data = {};

    for (int i = 0; i < commands_data->ptrdata_params.lines_count; i++)
    {
        DPRINTF("\nEntering %d iteration of ptrdata for\n", i);

        curr_cmd_data = {.line = commands_data->ptrdata_params.ptrdata[i],
                         .command = CMD_HLT,
                         .value = 0};

        if (curr_cmd_data.line[0] == '\0')
        {
            continue;
        }

        if (GetAsmCommand(&curr_cmd_data, code_data))
        {
            return ASM_GET_COMMAND_ERROR;
        }
        DPRINTF("Command = %d\n", curr_cmd_data.command);

#ifdef LISTING
        if (AddStringToListing(&curr_cmd_data, code_data, listing_file_info.stream) != ASM_SUCCESS)
        {
            fclose(listing_file_info.stream);
            return ASM_LISTING_ERROR;
        }
#endif /* LISTING */

        if (AddCommandCode(&curr_cmd_data, code_data))
        {
            return ASM_SET_COMMAND_ERROR;
        }
    }

#ifdef LISTING
    fclose(listing_file_info.stream);
#endif /* LISTING */

    return ASM_SUCCESS;
}

int GetAsmCommand(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data)
{
    assert(code_data != NULL);
    assert(curr_cmd_data != NULL);

    char operation[CMD_MAX_LEN] = {};

    if (sscanf(curr_cmd_data->line, "%s", operation) != 1)
    {
        DPRINTF("sscanf failed\n");
        return 1;
    }
    // 1 scanf
    if (sscanf(curr_cmd_data->line, ":%d", &curr_cmd_data->value) == 1)
    {
        curr_cmd_data->command = CMD_LABEL;
        return 0;
    }
    for (size_t i = 0; i < COMM_CASES_SIZE; i++)
    {
        if (strcmp(operation, COMM_CASES[i].str_command) == 0)
        {
            curr_cmd_data->command = COMM_CASES[i].command;
            break;
        }
    }
    if (CmdArgsCount(curr_cmd_data->command) == 1)
    {
        if (GetValue(curr_cmd_data, code_data))
        {
            return 1;
        }
    }

    return 0;
}

int GetValue(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data)
{
    assert(code_data != NULL);
    assert(curr_cmd_data != NULL);

    DPRINTF("cmd = %d\n", curr_cmd_data->command);
    char operation[CMD_MAX_LEN] = {};
    int label = 0;

    if (sscanf(curr_cmd_data->line, "%s :%d", operation, &label) == 2)
    {
        if (GetLabelValue(curr_cmd_data, code_data, label))
        {
            return ASM_GET_LABEL_ERROR;
        }
    }
    else if (curr_cmd_data->command == CMD_PUSHR ||
             curr_cmd_data->command == CMD_POPR)
    {
        if (GetRegValue(curr_cmd_data))
        {
            return ASM_GET_REG_ERROR;
        }
    }
    else if (curr_cmd_data->command == CMD_PUSHM ||
             curr_cmd_data->command == CMD_POPM)
    {
        if (GetRamArgument(curr_cmd_data))
        {
            return ASM_GET_RAM_ARG_ERROR;
        }
    }
    else
    {
        if (sscanf(curr_cmd_data->line, "%s %d",
                   operation, &curr_cmd_data->value) != 2)
        {
            DPRINTF("Wrong number of args for push\n");
            return ASM_GET_OP_ARG_ERR;
        }
    }

    return 0;
}

int GetLabelValue(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data, int label)
{
    assert(curr_cmd_data != NULL);
    assert(code_data != NULL);

    if (!((curr_cmd_data->command >= CMD_JMP &&
           curr_cmd_data->command <= CMD_JNE) ||
           curr_cmd_data->command == CMD_CALL))
    {
        printf("Syntax error: this command doesn't receive label\n");
        return 1;
    }
    if (!(label >= 0))
    {
        printf("Syntax error: Given lable for jump is negative\n");
        return 1;
    }
    if (label >= MAX_LABELS_SIZE)
    {
        printf("Syntax error: Given lable for jump is too big\n");
        return 1;
    }
    if (label >= code_data->labels_size)
    {
        DPRINTF("Memory realloc...\n");
        if (LabelsRecalloc(code_data, 2 * label))
        {
            return 1;
        }
    }
    curr_cmd_data->value = code_data->labels[label];
    DPRINTF("cmd = %d; value = %d;\n", curr_cmd_data->command, curr_cmd_data->value);

    return 0;
}

int GetRegValue(CurrCmdData_t* curr_cmd_data)
{
    assert(curr_cmd_data != NULL);

    char operation[CMD_MAX_LEN] = {};
    char reg[CMD_MAX_LEN] = {};
    if (sscanf(curr_cmd_data->line, "%s %s", operation, reg) != 2)
    {
        DPRINTF("Wrong number of args for reg cmd\n");
        return 1;
    }
    curr_cmd_data->value = reg[1] - 'A';

    return 0;
}

int GetRamArgument(CurrCmdData_t* curr_cmd_data)
{
    assert(curr_cmd_data != NULL);

    char operation[CMD_MAX_LEN] = {};
    char reg[CMD_MAX_LEN] = {};
    if (sscanf(curr_cmd_data->line, "%s [%s]", operation, reg) != 2)
    {
        DPRINTF("Wrong number of args for ram cmd\n");
        return 1;
    }
    curr_cmd_data->value = reg[1] - 'A';

    return 0;
}

int AddCommandCode(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data)
{
    assert(code_data != NULL);
    assert(curr_cmd_data != NULL);

    Command_t command = curr_cmd_data->command;

    if (command < CMD_HLT ||
        command > CMD_END)
    {
        DPRINTF("<ERROR: Unknown command code in AddCommandCode()>\n");
        return 1;
    }
    if (command == CMD_LABEL)
    {
        if (AddLabelCode(curr_cmd_data, code_data))
        {
            return 1;
        }

        return 0;
    }

    code_data->buffer[code_data->cur_cmd++] = command;

    if (CmdArgsCount(command) == 1)
    {
        if (command == CMD_POPR || command == CMD_PUSHR ||
            command == CMD_POPM || command == CMD_PUSHM)
        {
            code_data->buffer[code_data->cur_cmd++] = curr_cmd_data->value;
        }
        else
        {
            code_data->buffer[code_data->cur_cmd++] = curr_cmd_data->value;
        }
    }

    return 0;
}

int AddLabelCode(CurrCmdData_t* curr_cmd_data, CodeData_t* code_data)
{
    int label = curr_cmd_data->value;
    DPRINTF("curr_cmd_data->value = %d\n", curr_cmd_data->value);
    DPRINTF("label_size = %d\n", code_data->labels_size);
    if (label >= MAX_LABELS_SIZE)
    {
        printf("Syntax error: label is too big\n");
        return 1;
    }
    if (label < 0)
    {
        printf("Syntax error: label is negative\n");
        return 1;
    }
    if (label >= code_data->labels_size)
    {
        if (LabelsRecalloc(code_data, 2 * label))
        {
            return 1;
        }
    }
    code_data->labels[label] = (int) code_data->cur_cmd;
    DPrintAsmData(code_data);

    return 0;
}

int LabelsRecalloc(CodeData_t* code_data, int new_size)
{
    DPrintLabels(code_data);
    int old_size = code_data->labels_size;
    int* labels = (int*) realloc(code_data->labels, new_size * sizeof(int));
    if (labels == NULL)
    {
        printf("Labels recalloc failed\n");
        return 1;
    }
    code_data->labels_size = new_size;
    code_data->labels = labels;

    DPRINTF("old_size = %d\n", old_size);
    DPRINTF("labels[old_size] = %d\n", code_data->labels[old_size]);
    DPrintLabels(code_data);

    for (int i = old_size; i < new_size; i++)
    {
        DPRINTF("%d, ", i);
        code_data->labels[i] = -1;
    }
    DPRINTF("\n");
    DPRINTF("labels[old_size] = %d\n", code_data->labels[old_size]);

    DPrintLabels(code_data);
    return 0;
}

int WriteByteCode(CodeData_t* code_data, InputCtx_t* commands_data)
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

int WriteByteCodePretty(CodeData_t* code_data, const char* filepath)
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
