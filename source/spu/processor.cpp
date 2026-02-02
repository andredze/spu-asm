#include "processor.h"

//==========================================================================================

int SpuSetFilenames(const char** code_filename,
                    int argc, char* argv[], int* play_music,
                    char* music_filepath)
{
    switch (argc)
    {
        case 3:
            *code_filename = argv[1];
            *play_music = 1;
            strcat(music_filepath, argv[2]);
            break;
        case 2:
            *code_filename = argv[1];
            break;
        case 1:
            *code_filename = BINARY_BYTECODE_FILENAME;
            break;
        default:
            printf("Too much arguments given, maximum 1 (current arguments = %d)\n", argc);
            return 1;
    }

    return 0;
}

//==========================================================================================

ProcErr_t ProcCtor(Proc_t* proc_data)
{
    if (proc_data == NULL)
    {
        printf("<Proc_data is null pointer>\n");
        return PROC_DATA_IS_NULL;
    }

    if (StackCtor(&proc_data->stack, PROC_MIN_STACK_CAPACITY) != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }
    if (StackCtor(&proc_data->call_stack, PROC_MIN_STACK_CAPACITY) != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }

    proc_data->ram = (int*) calloc(RAM_SIZE, sizeof(int));
    if (proc_data->ram == NULL)
    {
        printf("Memory allocation for ram failed\n");
        return PROC_CALLOC_ERROR;
    }

    DPRINTF("Proc constructed\n");
    return PROC_SUCCESS;
}

//==========================================================================================

ProcErr_t ProcLoadPrettyBC(Proc_t* proc_data, const char* codepath)
{
    assert(proc_data != NULL);
    assert(codepath != NULL);
    DPRINTF("Loading pretty_bytecode...\n");

    InputCtx_t input_ctx = {.input_file_info =  {.filepath = codepath}};

    if (ReadAndParseFile(&input_ctx))
    {
        return PROC_READING_FILE_ERROR;
    }
    DPRINTF("Read and parsed file\n");
    DPRINTF("lines_count = %d\n", input_ctx.buffer_data.lines_count);

    proc_data->code_size = input_ctx.buffer_data.lines_count * 2;
    proc_data->code = (int*) calloc(proc_data->code_size, sizeof(int));
    if (proc_data->code == NULL)
    {
        printf("Calloc failed\n");
        return PROC_CALLOC_ERROR;
    }

    int args_count = 0;

    for (int i = 0; i < input_ctx.buffer_data.lines_count; i++)
    {
        args_count = sscanf(input_ctx.ptrdata_params.ptrdata[i],
                            "%d %d",
                            &proc_data->code[proc_data->cmd_count],
                            &proc_data->code[proc_data->cmd_count + 1]);
        DPRINTF("str = %s\n", input_ctx.ptrdata_params.ptrdata[i]);
        if (args_count == 2)
        {
            DPRINTF("got: %d %d\n", proc_data->code[proc_data->cmd_count],
                                    proc_data->code[proc_data->cmd_count + 1]);
            proc_data->cmd_count += 2;
        }
        else if (args_count == 1)
        {
            DPRINTF("got: %d\n", proc_data->code[proc_data->cmd_count]);
            proc_data->cmd_count++;
        }
        else
        {
            DPRINTF("Error with reading commands from ptrdata\n");
            DPRINTF("args_count = %d\n", args_count);
            return PROC_UNKNOWN_COMMAND;
        }
    }
    proc_data->cmd_count = 0;

    free(input_ctx.buffer_data.buffer);
    free(input_ctx.ptrdata_params.ptrdata);
    DPRINTF("Pretty bytecode loaded\n");

    return PROC_SUCCESS;
}

//==========================================================================================

ProcErr_t ProcLoadCode(Proc_t* proc_data, const char* codepath)
{
    assert(proc_data != NULL);
    assert(codepath != NULL);

    FILE* code_stream = fopen(codepath, "rb");
    if (code_stream == NULL)
    {
        printf("Error with opening file\n");
        return PROC_CODE_FILE_OPENING_ERROR;
    }

    CodeParams_t code_params = {};
    if (fread(&code_params, sizeof(code_params), 1, code_stream) != 1)
    {
        DPRINTF("Reading the code_params failed\n");
        return PROC_READING_FILE_ERROR;
    }

    if (code_params.version != CODE_VERSION)
    {
        printf("Wrong version of code, this processor can only execute %d version\n", CODE_VERSION);
        return PROC_READING_FILE_ERROR;
    }
    if (code_params.code_size > PROC_CODE_SIZE_LIMIT)
    {
        DPRINTF("Too large code\n");
        return PROC_CODE_SIZE_EXCEEDS_LIMIT;
    }
    proc_data->code_size = code_params.code_size;

    proc_data->code = (int*) calloc(proc_data->code_size, sizeof(int));
    if (proc_data->code == NULL)
    {
        printf("Calloc failed\n");
        return PROC_CALLOC_ERROR;
    }

    size_t fread_return = fread(proc_data->code, sizeof(proc_data->code[0]),
                                proc_data->code_size, code_stream);
    if (fread_return != proc_data->code_size)
    {
        DPRINTF("Error with fread, return = %zu\n", fread_return);
        return PROC_READING_FILE_ERROR;
    }

    return PROC_SUCCESS;
}

//==========================================================================================

ProcErr_t ProcDtor(Proc_t* proc_data)
{
    DPRINTF("Destroying proc_data...\n");

    if (proc_data == NULL)
    {
        DPRINTF("proc_data is a null ptr\n");
        return PROC_DATA_IS_NULL;
    }
    if (proc_data->code == NULL)
    {
        DPRINTF("proc_data.code is a null ptr\n");
        return PROC_DATA_IS_NULL;
    }

    free(proc_data->ram);
    proc_data->ram = NULL;

    free(proc_data->code);
    proc_data->code = NULL;

    StackDtor(&proc_data->stack);
    StackDtor(&proc_data->call_stack);

    DPRINTF("proc_data destroyed\n");
    return PROC_SUCCESS;
}

//==========================================================================================

#ifdef PROC_DEBUG

//==========================================================================================

ProcErr_t ProcVerify(Proc_t* proc_data)
{
    // DPRINTF("Verifying proc...\n");

    if (proc_data == NULL)
    {
        return PROC_DATA_IS_NULL;
    }
    if (proc_data->code == NULL)
    {
        return PROC_CODE_IS_NULL;
    }
    if (proc_data->cmd_count > PROC_CODE_SIZE_LIMIT)
    {
        return PROC_CMD_COUNT_EXCEEDS_LIMIT;
    }
    if (proc_data->code_size > PROC_CODE_SIZE_LIMIT)
    {
        return PROC_CODE_SIZE_EXCEEDS_LIMIT;
    }
    if ((size_t) proc_data->cmd_count > proc_data->code_size)
    {
        return PROC_CMD_COUNT_BIGGER_CODE_SIZE;
    }
    if (StackVerify(&proc_data->stack) != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }

    // DPRINTF("Success in verify\n");
    return PROC_SUCCESS;
}

//==========================================================================================

ProcErr_t ProcErrToStr(ProcErr_t error, const char** error_str)
{
    DPRINTF("Converting error to string...\n");

    if (!(0 <= error && error <= PROC_ERR_END))
    {
        DPRINTF("Error in ProcErrToStr(): error not in enum\n");
        return PROC_UNKNOWN_COMMAND;
    }
    *error_str = PROC_ERR_STRS[error];

    return PROC_SUCCESS;
}

//==========================================================================================

ProcErr_t ProcDump(Proc_t* proc_data, ProcErr_t error)
{
    // DPRINTF("Dumping...\n");

    FILE* stream = fopen(PROCESSOR_LOGFILENAME, "w");
    if (stream == NULL)
    {
        DPRINTF("Can not open stream: processor.log");
        return PROC_OUTPUT_FILE_OPENING_ERROR;
    }

    const char* error_str = "";
    if (ProcErrToStr(error, &error_str) != PROC_SUCCESS)
    {
        return PROC_UNKNOWN_COMMAND;
    }
    DPRINTF("Converting ended\n");
    DPRINTF("error = %d\n", error);
    DPRINTF("error_str = %s\n", error_str);

    if (error == PROC_DATA_IS_NULL)
    {
        fprintf(stream, "%s\n", error_str);
        fclose(stream);
        return PROC_SUCCESS;
    }

    fprintf(stream, "ERROR %d: %s\n"
                    "proc_data: [%p]:\n{\n"
                    "\tcmd_count = %zu;\n"
                    "\tcode_size = %zu;\n",
                    error, error_str,
                    proc_data,
                    proc_data->cmd_count,
                    proc_data->code_size);
    // stack
    // if stack_err than dont dump?
    proc_data->stack.var_info = {"proc_data->stack.var_info", __FILE__, __func__, __LINE__};
    if (StackDump(&proc_data->stack, STACK_SUCCESS, "Proc dump") != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }

    // regs
    fprintf(stream, "\tregs [%p]:\n"
                    "\t{\n", proc_data->regs);
    for (int i = 0; i < REGS_COUNT; i++)
    {
        fprintf(stream, "\t\t[%d] = %d;\n", i, proc_data->regs[i]);
    }
    fprintf(stream, "\t}\n"
                    "\tstack - CHECK stack.log\n");

    // code
    fprintf(stream, "\tcode [%p]:\n\t{\n", proc_data->code);
    if (error == PROC_CODE_IS_NULL)
    {
        fprintf(stream, "\t\t---------------\n"
                        "\t}\n"
                        "}\n");
        fclose(stream);
        return PROC_SUCCESS;
    }
    for (size_t i = 0; i < proc_data->code_size; i++)
    {
        if (i == (size_t) proc_data->cmd_count)
        {
            fprintf(stream, "\t\t*[%zu] = %d;\n", i, proc_data->code[i]);
        }
        else
        {
            fprintf(stream, "\t\t[%zu] = %d;\n", i, proc_data->code[i]);
        }
    }
    fprintf(stream, "\t}\n"
                    "}\n");
    fclose(stream);

    return PROC_SUCCESS;
}

//==========================================================================================

#endif /* PROC_DEBUG */

//==========================================================================================

ProcErr_t ProcExecuteCommands(Proc_t* proc_data)
{
    DPRINTF("Executing commands...\n");
    PROC_OK_DEBUG(proc_data);

    Command_t command = CMD_END;
    int break_loop = 0;

    while (proc_data->cmd_count < proc_data->code_size)
    {

#ifdef PROC_DEBUG
        getchar();
#endif /* PROC_DEBUG */

        if (ProcGetCommand(proc_data, &command))
        {
            return PROC_UNKNOWN_COMMAND;
        }
        DPRINTF(PURPLE "\ncode[%zu]: cmd = %d (%s)\n" RESET_CLR,
                proc_data->cmd_count, command, CMD_CASES[command].str_command);

        if (ProcExecuteOperation(proc_data, command, &break_loop) != PROC_SUCCESS)
        {
            return PROC_EXECUTE_OP_ERROR;
        }
        if (break_loop == 1)
        {
            DPRINTF(RED "\nProgram ran successfully\n" RESET_CLR);
            break;
        }

#ifdef PROC_DEBUG
        if (ProcConsoleDump(proc_data))
        {
            return PROC_DUMP_ERR;
        }
#endif /* PROC_DEBUG */

    }

    printf("---Executed commands---\n");

    return PROC_SUCCESS;
}

//==========================================================================================

ProcErr_t ProcGetCommand(Proc_t* proc_data, Command_t* command)
{
    PROC_OK_DEBUG(proc_data);

    *command = (Command_t) proc_data->code[proc_data->cmd_count++];

    return PROC_SUCCESS;
}

//==========================================================================================

ProcErr_t ProcExecuteOperation(Proc_t* proc_data, Command_t command, int* break_loop)
{
    PROC_OK_DEBUG(proc_data);

    if (command < 0 || command >= CMD_CASES_SIZE)
    {
        DPRINTF("Invalid command for ProcRunCommand()");
        return PROC_UNKNOWN_COMMAND;
    }

    HandleOpErr_t handle_op_return = CMD_CASES[command].handle_op(proc_data);
    if (handle_op_return == HANDLE_OP_BREAK_LOOP)
    {
        *break_loop = 1;
        return PROC_SUCCESS;
    }
    else if (handle_op_return != HANDLE_OP_SUCCESS)
    {
        return PROC_EXECUTE_OP_ERROR;
    }

    return PROC_SUCCESS;
}

//==========================================================================================

static void ColorPrintArray(char* name, int* array, size_t size, char* color_code)
{
    DPRINTF("%s%s\t= [", color_code, name, size);

    for (size_t i = 0; i < size; i++)
    {
        if (i == size - 1)
        {
            DPRINTF("%d", array[i]);
            break;
        }
        if ((i + 1) % 24 == 0)
        {
            DPRINTF("\n\t  ");
        }
        DPRINTF("%d, ", array[i]);
    }

    DPRINTF("]\n" RESET_CLR);
}

//------------------------------------------------------------------------------------------

static void DprintCommand(Proc_t* proc_data, size_t* i, size_t* cmds_in_line, int jump_id)
{
    CmdCase_t cmd_case = CMD_CASES[proc_data->code[*i]];

    (*cmds_in_line)++;

    bool opened_brace = false;

    if (*i == proc_data->cmd_count)
    {
        DPRINTF(RED "{");
        opened_brace = true;
    }
    else if (*i == jump_id)
    {
        DPRINTF(YELLOW "{");
        opened_brace = true;
    }

    if (cmd_case.args_count == 2)
    {
        DPRINTF("%-5s", cmd_case.str_command);

        if (20 <= proc_data->code[*i] && proc_data->code[*i] <= 23)
        {
            DPRINTF(" R%cX", (proc_data->code[*i + 1] + 'A'));
        }
        else
        {
            DPRINTF(" %-3d", proc_data->code[*i + 1]);
        }
        (*i)++;
    }
    else
    {
        DPRINTF("%-9s", cmd_case.str_command);
    }

    if (opened_brace)
    {
        DPRINTF("}" BLUE);
    }
    else
    {
        DPRINTF("  ");
    }

    if (*i != proc_data->code_size)
    {
        DPRINTF(", ");
    }

    if (*cmds_in_line % 7 == 0 && *cmds_in_line != 0)
    {
        DPRINTF("\n\t  ");
        *cmds_in_line = 0;
    }
}

//——————————————————————————————————————————————————————————————————————————————————————————

ProcErr_t ProcConsoleDump(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    size_t cmds_in_line = 0;
    size_t ram_size = (RAM_SIZE > MAX_DUMP_RAM_SIZE) ? MAX_DUMP_RAM_SIZE : RAM_SIZE;

    DPRINTF(RED "-----------------------------------<PROC DUMP>"
                "-----------------------------------\n");

    ColorPrintArray("stack", proc_data->stack.data,      proc_data->stack.size,      YELLOW);

    DPRINTF("           ");

    char reg_names[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    size_t reg_names_size = sizeof(reg_names);

    DPRINTF(GREEN);
    for (size_t i = 0; i < reg_names_size; i++)
    {
        if (i == reg_names_size - 1)
        {
            DPRINTF("%c", reg_names[i]);
            break;
        }
        if ((i + 1) % 24 == 0)
        {
            DPRINTF("\n\t");
        }
        DPRINTF("%c  ", reg_names[i]);
    }
    DPRINTF("\n" RESET_CLR);

    ColorPrintArray("regs",  proc_data->regs,            REGS_COUNT,                 GREEN);
    ColorPrintArray("calls", proc_data->call_stack.data, proc_data->call_stack.size, YELLOW);
    ColorPrintArray("ram",   proc_data->ram,             ram_size,                   LIGHT_YELLOW);

    DPRINTF("]\n" BLUE "code[] = [");

    int cmd = proc_data->code[proc_data->cmd_count];

    int jump_id = -1;

    if ((CMD_JMP <= cmd && cmd <= CMD_JNE) || cmd == CMD_CALL)
    {
        jump_id = proc_data->code[proc_data->cmd_count + 1];
    }

    for (size_t i = 0; i < proc_data->code_size; i++)
    {
        DprintCommand(proc_data, &i, &cmds_in_line, jump_id);
    }

    DPRINTF("]\n");

    DPRINTF(RED "-----------------------------------------"
                "----------------------------------------\n");

    DPRINTF(RESET_CLR);

    return PROC_SUCCESS;
}

//==========================================================================================
