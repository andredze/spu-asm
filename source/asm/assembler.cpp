#include "assembler.h"

//------------------------------------------------------------------------------------------

int AsmGetHash(const char* str)
{
    char c = '\0';
    int hash = 5381;
    while ((c = *str++) != '\0')
    {
        hash += (hash << 5) + hash + c;
    }

    return hash;
}

//------------------------------------------------------------------------------------------

AsmErr_t SetHashInCmdCases()
{
    for (size_t i = 0; i < CMD_CASES_SIZE; i++)
    {
        CMD_CASES[i].hash = AsmGetHash(CMD_CASES[i].str_command);
    }

    return ASM_SUCCESS;
}

//------------------------------------------------------------------------------------------

int AsmCmdCasesCompare(const void* par1, const void* par2)
{
    int hash1 = ((const CmdCase_t*) par1)->hash;
    int hash2 = ((const CmdCase_t*) par2)->hash;

    return hash1 - hash2;
}

//------------------------------------------------------------------------------------------

int SetFilenames(const char** commands_filename,
                 const char** bytecode_filename,
                 int argc, char* argv[], int* listing_flag)
{
    switch (argc)
    {
        case 4:
            *commands_filename = argv[1];
            *bytecode_filename = argv[2];
            if (strcmp(argv[3], "LIST=1") == 0)
            {
                *listing_flag = 1;
            }
            break;
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

//------------------------------------------------------------------------------------------

AsmErr_t CompileProgram(InputCtx_t* input_ctx, int listing_flag)
{
    assert(input_ctx != NULL);

    if (ReadAndParseFile(input_ctx))
    {
        return ASM_ERROR_WITH_READING_FILE;
    }
    DPRINTF("lines_count = %d\n", input_ctx->buffer_data.lines_count);

    AsmCtx_t asm_ctx = {};

    if (AsmCtxCtor(input_ctx, &asm_ctx))
    {
        return ASM_CALLOC_ERROR;
    }

    AsmErr_t error = ASM_SUCCESS;
    if ((error = CompileCode(input_ctx, &asm_ctx, 0)) != ASM_SUCCESS)
    {
        return error;
    }
    DPrintAsmData(&asm_ctx);

    /* two-pass compilation for labels that are declared after usage */

    asm_ctx.cur_cmd = 0; // reset current compile command to start

    if ((error = CompileCode(input_ctx, &asm_ctx, listing_flag)) != ASM_SUCCESS)
    {
        return error;
    }
    DPrintAsmData(&asm_ctx);

    if (WriteByteCode(&asm_ctx, input_ctx))
    {
        return ASM_PRINT_CODE_ERROR;
    }

    AsmDestroy(input_ctx, &asm_ctx);

    DPRINTF("\n<End of the compiler>\n");

    return ASM_SUCCESS;
}

//------------------------------------------------------------------------------------------

int AsmCtxCtor(InputCtx_t* input_ctx, AsmCtx_t* asm_ctx)
{
    int lines_count = input_ctx->buffer_data.lines_count;

    int* buffer = (int*) calloc(lines_count * ASM_MAX_ARGS_COUNT, sizeof(int));
    if (buffer == NULL)
    {
        DPRINTF("Buffer calloc failed\n");
        return 1;
    }
    asm_ctx->buffer = buffer;
    asm_ctx->cur_cmd = 0;

    size_t* labels = (size_t*) calloc(MIN_LABELS_SIZE, sizeof(size_t));
    if (labels == NULL)
    {
        DPRINTF("Labels calloc failed\n");
        return 1;
    }
    asm_ctx->labels = labels;
    asm_ctx->labels_size = MIN_LABELS_SIZE;

    for (int i = 0; i < MIN_LABELS_SIZE; i++)
    {
        asm_ctx->labels[i] = 777;
    }
    DPrintAsmData(asm_ctx);

    return 0;
}

//------------------------------------------------------------------------------------------

AsmErr_t CompileCode(InputCtx_t* input_ctx,
                     AsmCtx_t* asm_ctx,
                     int listing_flag)
{
    assert(input_ctx);
    assert(asm_ctx);

    FileInfo_t listing_file_info = {};

    if (listing_flag == 1)
    {
        if (CreateListingFile(input_ctx, &listing_file_info))
        {
            return ASM_CREATE_LISTING_ERROR;
        }
    }

    for (int i = 0; i < input_ctx->ptrdata_params.lines_count; i++)
    {
        DPRINTF("\nEntering code line %d:\n", i);

#ifdef ASM_DEBUG
        getchar();
#endif /* ASM_DEBUG */

        CmdCtx_t cmd_ctx = {.line = input_ctx->ptrdata_params.ptrdata[i]};

        AsmErr_t error = ASM_SUCCESS;

        int do_continue = 0;

        if ((error = CompileCmd(asm_ctx, &cmd_ctx, &do_continue)) != ASM_SUCCESS)
        {
            return error;
        }
        if (do_continue)
        {
            continue;
        }

        if (listing_flag == 1)
        {
            if (AddStringToListing(&cmd_ctx, asm_ctx, listing_file_info.stream) != ASM_SUCCESS)
            {
                fclose(listing_file_info.stream);
                return ASM_LISTING_ERROR;
            }
        }

        if ((i + 1) % 10000000 == 0)
        {
            printf(LIGHT_YELLOW "Compiled %d million lines..\n" RESET_CLR, (i + 1) / 1000000);
        }
    }

    printf(PURPLE "TOTAL: Compiled %d lines\n" RESET_CLR, input_ctx->ptrdata_params.lines_count);

    if (listing_flag == 1)
    {
        fclose(listing_file_info.stream);
    }

    return ASM_SUCCESS;
}

//------------------------------------------------------------------------------------------

AsmErr_t CompileCmd(AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx, int* do_continue)
{
    assert(asm_ctx);
    assert(cmd_ctx);

    char* comment_start = NULL;

    if ((comment_start = strchr(cmd_ctx->line, COMMENT_SYMBOL)) != NULL)
    {
        *comment_start = '\0';
    }

    cmd_ctx->line = SkipSpaces(cmd_ctx->line);

    if (cmd_ctx->line[0] == '\0')
    {
        *do_continue = 1;
        return ASM_SUCCESS;
    }

    if (ProcessLabelCase(cmd_ctx, asm_ctx) != ASM_SUCCESS)
    {
        return ASM_LABEL_CASE_ERROR;
    }

    if (cmd_ctx->command == CMD_LABEL)
    {
        *do_continue = 1;
        return ASM_SUCCESS;
    }

    if (GetCmd(asm_ctx, cmd_ctx) != ASM_SUCCESS)
    {
        return ASM_GET_CMD_ERROR;
    }

    if (CMD_CASES[cmd_ctx->index].add_op(cmd_ctx, asm_ctx) != ASM_SUCCESS)
    {
        return ASM_ADD_OP_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------------------------------------------------

char* SkipSpaces(char* str)
{
    assert(str);

    while (!(isalpha(*str)) && *str != '\0')
    {
        str++;
    }

    return str;
}

//------------------------------------------------------------------------------------------

AsmErr_t GetCmd(AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx)
{
    assert(asm_ctx != NULL);
    assert(cmd_ctx != NULL);

    char operation[CMD_MAX_LEN] = {};
    int op_len = 0;

    if (sscanf(cmd_ctx->line, "%s%n", operation, &op_len) != 1)
    {
        DPRINTF("sscanf for GetCmd() failed\n");
        return ASM_SYNTAX_ERROR;
    }
    cmd_ctx->op_len = op_len;

    int current_op_hash = AsmGetHash(operation);

    int index = -1;

    if ((index = CmdCasesBinarySearch(current_op_hash, CMD_CASES, CMD_CASES_SIZE)) != EOF)
    {
        cmd_ctx->index = index;
        cmd_ctx->command = CMD_CASES[index].command;

        DPRINTF(LIGHT_YELLOW "index = %d\n"
                             "cmd = %s (%d)\n" RESET_CLR,
                index, CMD_CASES[index].str_command, cmd_ctx->command);

        return ASM_SUCCESS;
    }

    printf("Syntax error: unknown command\n");
    return ASM_SYNTAX_ERROR;
}

//------------------------------------------------------------------------------------------

int CmdCasesBinarySearch(int curr_hash, CmdCase_t cmd_case[], int size)
{
    int left = 0;
    int middle = 0;
    int right = size - 1;

    while (left <= right)
    {
        middle = left + (right - left) / 2;

        if (cmd_case[middle].hash == curr_hash)
            return middle;

        else if (cmd_case[middle].hash < curr_hash)
            left = middle + 1;

        else
            right = middle - 1;
    }

    return -1;
}

//------------------------------------------------------------------------------------------

int WriteByteCode(AsmCtx_t* asm_ctx, InputCtx_t* input_ctx)
{
    if (OpenFile(&input_ctx->output_file_info, "wb"))
    {
        return 1;
    }

    CodeParams_t code_params = {.version = CODE_VERSION,
                                .code_size = asm_ctx->cur_cmd};
    if (fwrite(&code_params,
               sizeof(code_params),
               1,
               input_ctx->output_file_info.stream) != 1)
    {
        DPRINTF("Code_params writing in file error\n");
        return 1;
    }

    if (fwrite(asm_ctx->buffer,
               sizeof(asm_ctx->buffer[0]),
               asm_ctx->cur_cmd,
               input_ctx->output_file_info.stream) != asm_ctx->cur_cmd)
    {
        DPRINTF("Fwrite error\n");
        return 1;
    }
    fclose(input_ctx->output_file_info.stream);

    return 0;
}

//------------------------------------------------------------------------------------------

int WriteByteCodePretty(AsmCtx_t* asm_ctx, const char* filepath)
{
    FileInfo_t pretty_output_info = {.filepath = filepath};

    if (OpenFile(&pretty_output_info, "w"))
    {
        DPRINTF("Failed opening pretty\n");
        return 1;
    }

    size_t i = 0;
    while (i < asm_ctx->cur_cmd)
    {
        if (CMD_CASES[(Command_t) asm_ctx->buffer[i]].args_count == 2)
        {
            fprintf(pretty_output_info.stream,
                    "%d %d\n",
                    asm_ctx->buffer[i],
                    asm_ctx->buffer[i + 1]);
            i++;
        }
        else
        {
            fprintf(pretty_output_info.stream,
                    "%d\n",
                    asm_ctx->buffer[i]);
        }
        i++;
    }
    fclose(pretty_output_info.stream);

    return 0;
}

//------------------------------------------------------------------------------------------

void AsmDestroy(InputCtx_t* input_ctx, AsmCtx_t* asm_ctx)
{
    free(asm_ctx->buffer);
    free(asm_ctx->labels);
    free(input_ctx->buffer_data.buffer);
    free(input_ctx->ptrdata_params.ptrdata);
}

//------------------------------------------------------------------------------------------
