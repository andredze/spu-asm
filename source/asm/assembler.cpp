#include "assembler.h"
#include "config.h"

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

AsmErr_t CompileProgramm(InputCtx_t* input_ctx, int listing_flag)
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
    if ((error = CompileCommands(input_ctx, &asm_ctx, 0)) != ASM_SUCCESS)
    {
        return error;
    }
    DPrintAsmData(&asm_ctx);

    // two-pass compilation for labels that are declared after usage
    asm_ctx.cur_cmd = 0; // reset current compile command to start
    if ((error = CompileCommands(input_ctx, &asm_ctx, listing_flag)) != ASM_SUCCESS)
    {
        return error;
    }
    DPrintAsmData(&asm_ctx);

    free(asm_ctx.labels);

    if (WriteByteCode(&asm_ctx, input_ctx))
    {
        return ASM_PRINT_CODE_ERROR;
    }
    if (WriteByteCodePretty(&asm_ctx, READABLE_BYTECODE_FILENAME))
    {
        return ASM_PRINT_CODE_ERROR;
    }

    AsmDestroy(input_ctx, &asm_ctx);
    DPRINTF("\n<End of the compilator>");

    return ASM_SUCCESS;
}

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

AsmErr_t CompileCommands(InputCtx_t* input_ctx,
                         AsmCtx_t* asm_ctx,
                         int listing_flag)
{
    assert(input_ctx != NULL);
    assert(asm_ctx != NULL);

    FileInfo_t listing_file_info = {};
    if (listing_flag == 1)
    {
        if (CreateListingFile(input_ctx, &listing_file_info))
        {
            return ASM_CREATE_LISTING_ERROR;
        }
    }

    CmdCtx_t cmd_ctx = {};
    char* comment_start = NULL;

    for (int i = 0; i < input_ctx->ptrdata_params.lines_count; i++)
    {
        DPRINTF("\nEntering ptrdata[%d]:\n", i);
        cmd_ctx = {.line = input_ctx->ptrdata_params.ptrdata[i]};

        if ((comment_start = strchr(cmd_ctx.line, COMMENT_SYMBOL)) != NULL)
        {
            *comment_start = '\0';
        }

        if (cmd_ctx.line[0] == '\0')
            continue;

        if (ProcessLabelCase(asm_ctx, &cmd_ctx) != ASM_SUCCESS)
        {
            return ASM_LABEL_CASE_ERROR;
        }
        if (cmd_ctx.command == CMD_LABEL) continue;

        if (GetCmd(asm_ctx, &cmd_ctx) != ASM_SUCCESS)
        {
            return ASM_GET_CMD_ERROR;
        }
        DPRINTF(LIGHT_YELLOW "cmd = %s (%d)\n" RESET_CLR,
                ASM_CMD_CASES[cmd_ctx.command].str_command, cmd_ctx.command);
        if (ASM_CMD_CASES[cmd_ctx.command].add_op(&cmd_ctx, asm_ctx) != ASM_SUCCESS)
        {
            return ASM_ADD_OP_ERROR;
        }
        if (listing_flag == 1)
        {
            if (AddStringToListing(&cmd_ctx, asm_ctx, listing_file_info.stream) != ASM_SUCCESS)
            {
                fclose(listing_file_info.stream);
                return ASM_LISTING_ERROR;
            }
        }
    }
    if (listing_flag == 1)
    {
        fclose(listing_file_info.stream);
    }
    return ASM_SUCCESS;
}

AsmErr_t GetCmd(AsmCtx_t* asm_ctx, CmdCtx_t* cmd_ctx)
{
    assert(asm_ctx != NULL);
    assert(cmd_ctx != NULL);

    char operation[CMD_MAX_LEN] = {};

    if (sscanf(cmd_ctx->line, "%s", operation) != 1)
    {
        DPRINTF("sscanf for GetCmd() failed\n");
        return ASM_SYNTAX_ERROR;
    }
    for (size_t i = 0; i < ASM_CMD_CASES_SIZE; i++)
    {
        if (strcmp(operation, ASM_CMD_CASES[i].str_command) == 0)
        {
            cmd_ctx->command = ASM_CMD_CASES[i].command;
            return ASM_SUCCESS;
        }
    }

    printf("Syntax error: unknown command\n");
    return ASM_SYNTAX_ERROR;
}

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
        if (COMM_CASES[(Command_t) asm_ctx->buffer[i]].args_count == 2)
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

void AsmDestroy(InputCtx_t* input_ctx, AsmCtx_t* asm_ctx)
{
    free(asm_ctx->buffer);
    free(asm_ctx->labels);
    free(input_ctx->buffer_data.buffer);
    free(input_ctx->ptrdata_params.ptrdata);
}
