#include "processor.h"

// TODO: regs

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

    DPRINTF("Proc constructed\n");
    return PROC_SUCCESS;
}

ProcErr_t ProcLoadCode(Proc_t* proc_data, const char* codepath)
{
    assert(codepath != NULL);

    FILE* code_stream = fopen(codepath, "rb");
    if (code_stream == NULL)
    {
        printf("Error with opening file\n");
        return PROC_CODE_FILE_OPENNING_ERROR;
    }

    size_t code_size = 0;
    if (fread(&code_size, sizeof(code_size), 1, code_stream) != 1)
    {
        DPRINTF("Reading the size of code failed\n");
        return PROC_READING_FILE_ERROR;
    }

    proc_data->code_size = code_size;

    proc_data->code = (int*) calloc(code_size, sizeof(int));
    if (proc_data->code == NULL)
    {
        printf("Calloc failed\n");
        return PROC_CALLOC_ERROR;
    }

    size_t fread_return = fread(proc_data->code, sizeof(proc_data->code[0]),
                                code_size, code_stream);
    if (fread_return != code_size)
    {
        DPRINTF("Error with fread, return = %zu\n", fread_return);
        return PROC_READING_FILE_ERROR;
    }

    return PROC_SUCCESS;
}

ProcErr_t ProcDtor(Proc_t* proc_data, FILE* stream)
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

    free(proc_data->code);
    proc_data->code = NULL;
    StackDtor(&proc_data->stack);
    fclose(stream);

    DPRINTF("proc_data destroyed\n");
    return PROC_SUCCESS;
}

ProcErr_t ProcVerify(Proc_t* proc_data)
{
    DPRINTF("Verifying proc...\n");

    if (proc_data == NULL)
    {
        return PROC_DATA_IS_NULL;
    }
    if (proc_data->code == NULL)
    {
        return PROC_CODE_IS_NULL;
    }
    if (proc_data->cmd_count < 0)
    {
        return PROC_CMD_COUNT_NEGATIVE;
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

    DPRINTF("Success in verify\n");
    return PROC_SUCCESS;
}

int ProcErrToStr(ProcErr_t error, const char** error_str)
{
    DPRINTF("Converting error to string...\n");

    switch (error)
    {
        case PROC_SUCCESS:
            *error_str = "Proc_data is OK";
            return 0;
        case PROC_CODE_FILE_OPENNING_ERROR:
            *error_str = "Error with opening code file";
            return 0;
        case PROC_READING_FILE_ERROR:
            *error_str = "Error with reading file";
            return 0;
        case PROC_CALLOC_ERROR:
            *error_str = "Error with memory allocation";
            return 0;
        case PROC_OUTPUT_FILE_OPENNING_ERROR:
            *error_str = "Error with opening output file";
            return 0;
        case PROC_UNKNOWN_COMMAND:
            *error_str = "Unknown command";
            return 0;
        case PROC_MATH_ERROR:
            *error_str = "Error with math ";
            return 0;
        case PROC_DATA_IS_NULL:
            *error_str = "proc_data is null ptr";
            return 0;
        case PROC_CODE_IS_NULL:
            *error_str = "proc_data.code is null ptr";
            return 0;
        case PROC_STACK_ERROR:
            *error_str = "Error with proc_data.stack, check stack logs";
            return 0;
        case PROC_CMD_COUNT_NEGATIVE:
            *error_str = "Proc_data.cmd_count has negative value";
            return 0;
        case PROC_CODE_SIZE_EXCEEDS_LIMIT:
            *error_str = "Proc_data.code_size is bigger than limit (probably was set to negative)";
            return 0;
        case PROC_CMD_COUNT_BIGGER_CODE_SIZE:
            *error_str = "Proc_data.cmd_count is bigger than code_size";
            return 0;
        default:
            return 1;

    return 1;
    }
}

ProcErr_t ProcDump(Proc_t* proc_data, ProcErr_t error)
{
    DPRINTF("Dumping...\n");

    FILE* stream = fopen("processor.log", "w");
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
                    "\tcmd_count = %d;\n"
                    "\tcode_size = %zu;\n",
                    proc_data->var_info.function,
                    proc_data->var_info.file_name,
                    proc_data->var_info.line,
                    error, error_str,
                    proc_data->var_info.struct_name, proc_data,
                    proc_data->cmd_count,
                    proc_data->code_size);
    // stack
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

ProcErr_t ProcExecuteCommands(Proc_t* proc_data, FILE* stream)
{
    DPRINTF("Executing commands...\n");
    PROC_OK_DEBUG(proc_data);

    Command_t command = CMD_HLT;
    int value = 0;

    // TODO: cmd_count in size_t
    while (proc_data->cmd_count < proc_data->code_size)
    {
        DPRINTF("\nEntering %d instruction\n", proc_data->cmd_count);

        if (ProcGetCommand(proc_data, &command, &value))
        {
            return PROC_UNKNOWN_COMMAND;
        }
        DPRINTF("Command = %d\n", command);

        if (command == CMD_HLT)
        {
            break;
        }
        if (ProcRunCommand(proc_data, command, value, stream))
        {
            return PROC_MATH_ERROR;
        }
    }
    DPRINTF("Executed commands\n");

    return PROC_SUCCESS;
}

ProcErr_t ProcGetCommand(Proc_t* proc_data,
                         Command_t* command,
                         int* value)
{
    DPRINTF("Getting command...\n");
    PROC_OK_DEBUG(proc_data);

    // TODO: check if command is valid + number of args given
    *command = (Command_t) proc_data->code[proc_data->cmd_count++];

    if (*command == CMD_PUSH)
    {
        *value = proc_data->code[proc_data->cmd_count++];
    }

    return PROC_SUCCESS;
}

int ProcRunCommand(Proc_t* proc_data, Command_t command,
                   int value, FILE* output_stream)
{
    assert(output_stream != NULL);
    DPRINTF("Running command...\n");
    PROC_OK_DEBUG(proc_data);
    Stack_t* stk_ptr = &proc_data->stack;

    switch (command)
    {
        case CMD_PUSH: return StackPush(stk_ptr, value) == STACK_SUCCESS ? 0 : 1;
        case CMD_ADD:  return ApplyBinaryOperation(stk_ptr, Add) == MATH_SUCCESS ? 0 : 1;
        case CMD_SUB:  return ApplyBinaryOperation(stk_ptr, Sub) == MATH_SUCCESS ? 0 : 1;
        case CMD_MUL:  return ApplyBinaryOperation(stk_ptr, Mul) == MATH_SUCCESS ? 0 : 1;
        case CMD_DIV:  return ApplyBinaryOperation(stk_ptr, Div) == MATH_SUCCESS ? 0 : 1;
        case CMD_SQRT: return Sqrt(stk_ptr) == MATH_SUCCESS ? 0 : 1;
        case CMD_OUT:  return ProcHandleOut(stk_ptr, output_stream);
        case CMD_HLT:  return 1;
        default:       return 1;
    }

    return 0;
}

int ProcHandleOut(Stack_t* stack, FILE* output_stream)
{
    assert(stack != NULL);
    assert(output_stream != NULL);

    int result = 0;
    StackErr_t pop_return = StackPop(stack, &result);

    if (pop_return == STACK_SIZE_IS_ZERO)
    {
        return 0;
    }
    if (pop_return != STACK_SUCCESS)
    {
        return 1;
    }

    fprintf(output_stream, "ANSWER = %d\n", result);
    return 0;
}
