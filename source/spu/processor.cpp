#include "processor.h"

// TODO: rename struct names (spu ...)

int SpuSetFilenames(const char** code_filename,
                    int argc, char* argv[])
{
    switch (argc)
    {
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

ProcErr_t ProcLoadCode(Proc_t* proc_data, const char* codepath)
{
    assert(proc_data != NULL);
    assert(codepath != NULL);

    FILE* code_stream = fopen(codepath, "rb");
    if (code_stream == NULL)
    {
        printf("Error with opening file\n");
        return PROC_CODE_FILE_OPENNING_ERROR;
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

#ifdef PROC_DEBUG
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

int ProcErrToStr(ProcErr_t error, const char** error_str)
{
    DPRINTF("Converting error to string...\n");

    if (!(0 <= error && error <= PROC_ERR_END))
    {
        DPRINTF("Error in ProcErrToStr(): error not in enum\n");
        return 1;
    }
    *error_str = PROC_ERR_STRS[error];

    return 0;
}

ProcErr_t ProcDump(Proc_t* proc_data, ProcErr_t error)
{
    // DPRINTF("Dumping...\n");

    FILE* stream = fopen(PROCESSOR_LOGFILENAME, "w");
    if (stream == NULL)
    {
        DPRINTF("Can not open stream: processor.log");
        return PROC_OUTPUT_FILE_OPENNING_ERROR;
    }

    const char* error_str = "";
    if (ProcErrToStr(error, &error_str))
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

    fprintf(stream, "from %s at %s:%d\n"
                    "ERROR %d: %s\n"
                    "proc_data: %s [%p]:\n{\n"
                    "\tcmd_count = %zu;\n"
                    "\tcode_size = %zu;\n",
                    proc_data->var_info.function,
                    proc_data->var_info.file_name,
                    proc_data->var_info.line,
                    error, error_str,
                    proc_data->var_info.struct_name, proc_data,
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
#endif /* PROC_DEBUG */

ProcErr_t ProcExecuteCommands(Proc_t* proc_data)
{
    DPRINTF("Executing commands...\n");
    PROC_OK_DEBUG(proc_data);

    Command_t command = CMD_HLT;
    int value = 0;

    while (proc_data->cmd_count < proc_data->code_size)
    {
#ifdef PROC_DEBUG
        getchar();
#endif /* PROC_DEBUG */
        if (ProcGetCommand(proc_data, &command, &value))
        {
            return PROC_UNKNOWN_COMMAND;
        }

        DPRINTF(PURPLE "\ncode[%zu]: cmd = %d (%s)\n" RESET_CLR,
                proc_data->cmd_count, command, COMM_CASES[command].str_command);

        if (command == CMD_HLT)
        {
            DPRINTF(RED "\nProgramm ran successfully\n" RESET_CLR);
            break;
        }
        if (ProcRunCommand(proc_data, command, value))
        {
            return PROC_MATH_ERROR;
        }

#ifdef PROC_DEBUG
        if (ProcConsoleDump(proc_data))
        {
            return PROC_DUMP_ERR;
        }
#endif /* PROC_DEBUG */

    }
    DPRINTF("Executed commands\n");
    printf("---Executed commands---\n");
    // printf("---Check \"answers.txt\"---\n");

    return PROC_SUCCESS;
}

ProcErr_t ProcGetCommand(Proc_t* proc_data,
                         Command_t* command,
                         int* value)
{
    PROC_OK_DEBUG(proc_data);

    *command = (Command_t) proc_data->code[proc_data->cmd_count++];

    if (CmdArgsCount(*command) == 1)
    {
        *value = proc_data->code[proc_data->cmd_count++];
    }

    return PROC_SUCCESS;
}

int ProcRunCommand(Proc_t* proc_data, Command_t command, int value)
{
    PROC_OK_DEBUG(proc_data);

    switch (command)
    {
        case CMD_PUSH:  DPRINTF("\tPUSH: %d\n", value);
                        return StackPush(&proc_data->stack, value) == STACK_SUCCESS ? 0 : 1;
        case CMD_ADD:   return HandleADD(proc_data);
        case CMD_SUB:   return HandleSUB(proc_data);
        case CMD_MUL:   return HandleMUL(proc_data);
        case CMD_DIV:   return HandleDIV(proc_data);
        case CMD_MOD:   return HandleMOD(proc_data);
        case CMD_SQRT:  return HandleSqrt(proc_data);
        case CMD_OUT:   return HandleOut(proc_data);
        case CMD_POPR:  return HandlePopr(proc_data, value);
        case CMD_PUSHR: return HandlePushr(proc_data, value);
        case CMD_IN:    return HandleIn(proc_data);
        case CMD_JMP:   return HandleJmp(proc_data, value);
        case CMD_JB:    return HandleJB (proc_data, value);
        case CMD_JBE:   return HandleJBE(proc_data, value);
        case CMD_JA:    return HandleJA (proc_data, value);
        case CMD_JAE:   return HandleJAE(proc_data, value);
        case CMD_JE:    return HandleJE (proc_data, value);
        case CMD_JNE:   return HandleJNE(proc_data, value);
        case CMD_CALL:  return HandleCall(proc_data, value);
        case CMD_RET:   return HandleRet(proc_data);
        case CMD_PUSHM: return HandlePushm(proc_data, value);
        case CMD_POPM:  return HandlePopm(proc_data, value);
        case CMD_DRAW:  return HandleDraw(proc_data, value);
        case CMD_HLT:   return 1;
        default:        return 1;
    }

    return 0;
}

int ProcConsoleDump(Proc_t* proc_data)
{
    assert(proc_data != NULL);

    DPRINTF(RED "------------------------<PROC DUMP>------------------------\n"
            YELLOW "stack: [");
    for (size_t i = 0; i < proc_data->stack.size; i++)
    {
        if (i == proc_data->stack.size - 1) {
            DPRINTF("%d", proc_data->stack.data[i]); break; }
        if ((i + 1) % 14 == 0 && i != 0) {
            DPRINTF("\n\t"); }
        DPRINTF("%d, ", proc_data->stack.data[i])
    }
    DPRINTF("]\n"
            "call_stack: [");
    for (size_t i = 0; i < proc_data->call_stack.size; i++)
    {
        if (i == proc_data->call_stack.size - 1) {
            DPRINTF("%d", proc_data->call_stack.data[i]); break; }
        if ((i + 1) % 14 == 0 && i != 0) {
            DPRINTF("\n\t"); }
        DPRINTF("%d, ", proc_data->call_stack.data[i])
    }
    DPRINTF("]\n" GREEN
            "regs[%d] = [", REGS_COUNT);
    for (int i = 0; i < REGS_COUNT; i++)
    {
        if (i == REGS_COUNT - 1) {
            DPRINTF("%d", proc_data->regs[i]); break; }
        if ((i + 1) % 14 == 0 && i != 0) {
            DPRINTF("\n\t"); }
        DPRINTF("%d, ", proc_data->regs[i]);
    }
    DPRINTF("]\n" BLUE
            "code[] = [");
    for (size_t i = 0; i < proc_data->code_size; i++)
    {
        if (i == proc_data->code_size - 1) {
            DPRINTF("%d", proc_data->code[i]); break; }
        if ((i + 1) % 14 == 0 && i != 0) {
            DPRINTF("\n\t  "); }
        if (i == proc_data->cmd_count) {
            DPRINTF("{%d}, ", proc_data->code[i]); }
        else {
            DPRINTF("%d, ", proc_data->code[i]); }
    }
    DPRINTF("]\n"
            LIGHT_YELLOW "ram[] = [");
    for (size_t i = 0; i < RAM_SIZE; i++)
    {
        if (i == RAM_SIZE - 1) {
            DPRINTF("%d", proc_data->ram[i]); }
        else if (i % 17 == 0 && i != 0) {
            DPRINTF("\n\t "); }
        else {
            DPRINTF("%d, ", proc_data->ram[i]); }
    }
    DPRINTF("]\n"
            RED "-----------------------------------------------------------" RESET_CLR);

    return 0;
}
