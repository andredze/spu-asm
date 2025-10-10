#include "assembler.h"
#include "config.h"

// TODO: labels in dynamic
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
    DPRINTF("\n-----------------------------------------------------------------\n");
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
    DPRINTF("-----------------------------------------------------------------\n");
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

AsmErr_t CreateListingFile(InputCtx_t* commands_data,
                           FileInfo_t* listing_file_info)
{
    assert(commands_data != NULL);
    assert(listing_file_info != NULL);

    char listing_filename[MAX_FILENAME_LEN] = {};
    strcpy(listing_filename, "logs/");
    strcat(listing_filename, strchr(commands_data->input_file_info.filepath, '/') + 1);
    strcat(listing_filename, "_listing.txt");
    DPRINTF("listing_filename = %s\n", listing_filename);
    listing_file_info->filepath = listing_filename;

    if (OpenFile(listing_file_info, "w"))
    {
        return ASM_CREATE_LISTING_ERROR;
    }
    fprintf(listing_file_info->stream, "address      \tcommand     cmd    \tvalue\tindex\n\n");
    DPRINTF("FILE = %p\n", listing_file_info->stream);

    return ASM_SUCCESS;
}

AsmErr_t AddStringToListing(CurrCmdData_t* curr_cmd_data,
                            CodeData_t* code_data,
                            FILE* listing_stream)
{
    assert(curr_cmd_data != NULL);
    assert(code_data != NULL);

    if (CmdArgsCount(curr_cmd_data->command) == 1)
    {
        if (fprintf(listing_stream, "%-12p\t%-10s\t%-4d\t%-4d\t%-4zu\n",
                    &code_data->buffer[code_data->cur_cmd],
                    curr_cmd_data->line,
                    curr_cmd_data->command,
                    curr_cmd_data->value,
                    code_data->cur_cmd) < 0)
        {
            DPRINTF("Fprintf error in listing cmd with 1 arg\n");
            return ASM_LISTING_ERROR;
        }
    }
    else
    {
        if (fprintf(listing_stream, "%-12p\t%-10s\t%-4d\t\t\t%-4zu\n",
                    &code_data->buffer[code_data->cur_cmd],
                    curr_cmd_data->line,
                    curr_cmd_data->command,
                    code_data->cur_cmd) < 0)
        {
            DPRINTF("Fprintf error in listing cmd with 0 arg\n");
            return ASM_LISTING_ERROR;
        }
    }

    return ASM_SUCCESS;
}

AsmErr_t CompileCommands(InputCtx_t* commands_data,
                         CodeData_t* code_data)
{
    assert(commands_data != NULL);
    assert(code_data != NULL);

    FileInfo_t listing_file_info = {};
    if (CreateListingFile(commands_data, &listing_file_info))
    {
        return ASM_CREATE_LISTING_ERROR;
    }

    for (int i = 0; i < commands_data->ptrdata_params.lines_count; i++)
    {
        DPRINTF("\nEntering %d iteration of ptrdata for\n", i);

        CurrCmdData_t curr_cmd_data = {.line = commands_data->ptrdata_params.ptrdata[i],
                                       .command = CMD_HLT,
                                       .value = 0};

        if (GetAsmCommand(&curr_cmd_data, code_data))
        {
            return ASM_GET_COMMAND_ERROR;
        }
        DPRINTF("Command = %d\n", curr_cmd_data.command);

        if (AddStringToListing(&curr_cmd_data, code_data, listing_file_info.stream) != ASM_SUCCESS)
        {
            return ASM_LISTING_ERROR;
        }
        if (AddCommandCode(&curr_cmd_data, code_data))
        {
            return ASM_SET_COMMAND_ERROR;
        }
    }
    fclose(listing_file_info.stream);

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
        if (!(curr_cmd_data->command >= CMD_JMP &&
              curr_cmd_data->command <= CMD_JNE))
        {
            printf("Syntax error\n");
            return 1;
        }
        if (!(label >= 0 && label < LABELS_COUNT))
        {
            printf("Given label exceeds max label number\n");
            return 1;
        }
        curr_cmd_data->value = code_data->labels[label];

        return 0;
    }
    else if (curr_cmd_data->command == CMD_PUSHR ||
             curr_cmd_data->command == CMD_POPR)
    {
        char reg[CMD_MAX_LEN] = {};
        if (sscanf(curr_cmd_data->line, "%s %s", operation, reg) != 2)
        {
            DPRINTF("Wrong number of args for reg cmd\n");
            return 1;
        }
        curr_cmd_data->value = reg[1] - 'A';
        return 0;
    }
    else
    {
        if (sscanf(curr_cmd_data->line, "%s %d",
                   operation, &curr_cmd_data->value) != 2)
        {
            DPRINTF("Wrong number of args for push\n");
            return 1;
        }
        return 0;
    }

    return 1;
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
        // labels in size_t
        code_data->labels[curr_cmd_data->value] = (int) code_data->cur_cmd;
        return 0;
    }

    code_data->buffer[code_data->cur_cmd++] = command;

    if (CmdArgsCount(command) == 1)
    {
        if (command == CMD_POPR || command == CMD_PUSHR)
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
