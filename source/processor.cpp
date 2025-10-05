#include "processor.h"

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

    free(proc_data->code);
    proc_data->code = NULL;
    StackDtor(&proc_data->stack);

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
            *error_str = "proc_data.code if null ptr";
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

    if (error == PROC_DATA_IS_NULL)
    {
        fprintf(stream, "%s\n", error_str);
        fclose(stream);
        return PROC_SUCCESS;
    }

    fprintf(stream, "from %s at %s:%d\n"
                    "ERROR %d: %s\n"
                    "proc_data: %s [%p]\n{\n"
                    "\tcmd_count = %d;\n"
                    "\tcode_size = %zu;\n"
                    "\tcode [%p];\n\t{\n",
                    proc_data->var_info.function,
                    proc_data->var_info.file_name,
                    proc_data->var_info.line,
                    error, error_str,
                    proc_data->var_info.struct_name, proc_data,
                    proc_data->cmd_count,
                    proc_data->code_size,
                    proc_data->code);

    // code
    for (size_t i = 0; i < proc_data->code_size; i++)
    {
        if (i == (size_t) proc_data->cmd_count)
        {
            fprintf(stream, "\t\t*[%zu] = %d\n", i, proc_data->code[i]);
        }
        else
        {
            fprintf(stream, "\t\t[%zu] = %d\n", i, proc_data->code[i]);
        }
    }
    fprintf(stream, "\t}\n");

    // regs
    fprintf(stream, "\tregs [%p];\n"
                    "\t{\n", proc_data->regs);
    for (int i = 0; i < REGS_COUNT; i++)
    {
        fprintf(stream, "\t\t[%d] = %d\n", i, proc_data->regs[i]);
    }
    fprintf(stream, "\t}\n"
                    "\tstack - CHECK stack.log\n"
                    "}\n");
    fclose(stream);

    // stack
    if (StackDump(&proc_data->stack, STACK_SUCCESS, "Proc dump") != STACK_SUCCESS)
    {
        return PROC_STACK_ERROR;
    }

    return PROC_SUCCESS;
}

// ProcErr_t ExecutePrettyBCProgramm(Context_t* commands_data)
// {
//     assert(commands_data != NULL);
//
//     if (ReadAndParseFile(commands_data))
//     {
//         return PROC_ERROR_WITH_READING_FILE;
//     }
//     DPRINTF("lines_count = %d\n", commands_data->buffer_data.lines_count);
//
//     if (OpenFile(&commands_data->output_file_info, "w"))
//     {
//         return PROC_OUTPUT_FILE_OPENNING_ERROR;
//     }
//
//     CalcErr_t error = SUCCESS;
//     if ((error = ExecuteCommands(commands_data)) != SUCCESS)
//     {
//         return error;
//     }
//
//     free(commands_data->buffer_data.buffer);
//     free(commands_data->ptrdata_params.ptrdata);
//     fclose(commands_data->output_file_info.stream);
//     printf("\n<End of the calculator>");
//
//     return PROC_SUCCESS;
// }

// ProcErr_t ExecuteCommands(Context_t* commands_data)
// {
//     Command_t command = CMD_HLT;
//     int value = 0;
//
//     INIT_STACK(stack);
//
//     if (StackCtor(&stack, MIN_STACK_CAPACITY) != STACK_SUCCESS)
//     {
//         return PROC_STACK_ERROR;
//     }
//
//     for (int i = 0; i < commands_data->ptrdata_params.lines_count; i++)
//     {
//         DPRINTF("\nEntering %d iteration of ptrdata for\n", i);
//
//         if (GetCommand(commands_data->ptrdata_params.ptrdata[i],
//                        &command, &value))
//         {
//             return PROC_UNKNOWN_COMMAND;
//         }
//         DPRINTF("Command = %d\n", command);
//
//         if (command == CMD_HLT)
//         {
//             break;
//         }
//         if (RunCommand(&stack, command, value,
//                         commands_data->output_file_info.stream))
//         {
//             return PROC_MATH_ERROR;
//         }
//     }
//
//     return PROC_SUCCESS;
// }
//
// int GetCommand(char* line, Command_t* command, int* value)
// {
//     assert(line != NULL);
//     assert(command != NULL);
//     assert(value != NULL);
//
//     int cmd_code = 0;
//     int arg_count = sscanf(line, "%d %d", &cmd_code, value);
//
//     if (!(cmd_code >= -1 && cmd_code <= 6))
//     {
//         DPRINTF("Command code %d doesn't exist\n", cmd_code);
//         return 1;
//     }
//     *command = (Command_t) cmd_code;
//
//     if (arg_count > 2 || arg_count < 1)
//     {
//         DPRINTF("Invalid number of arguments for command\n"
//                 "arg_count = %d; expected = 1 or 2", arg_count);
//         return 1;
//     }
//     if (arg_count == 1 && *command == CMD_PUSH)
//     {
//         DPRINTF("Invalid number of arguments for command push\n"
//                 "arg_count = %d; expected = 2", arg_count);
//         return 1;
//     }
//     if (arg_count == 2 && *command != CMD_PUSH)
//     {
//         DPRINTF("Invalid number of arguments for command\n"
//                 "arg_count = %d; expected = 1", arg_count);
//         return 1;
//     }
//
//     return 0;
// }
//
// int RunCommand(Stack_t* stack, Command_t command,
//                int value, FILE* output_stream)
// {
//     assert(stack != NULL);
//     assert(output_stream != NULL);
//
//     switch (command)
//     {
//         case CMD_PUSH: return StackPush(stack, value) == STACK_SUCCESS ? 0 : 1;
//         case CMD_ADD:  return ApplyBinaryOperation(stack, Add) == MATH_SUCCESS ? 0 : 1;
//         case CMD_SUB:  return ApplyBinaryOperation(stack, Sub) == MATH_SUCCESS ? 0 : 1;
//         case CMD_MUL:  return ApplyBinaryOperation(stack, Mul) == MATH_SUCCESS ? 0 : 1;
//         case CMD_DIV:  return ApplyBinaryOperation(stack, Div) == MATH_SUCCESS ? 0 : 1;
//         case CMD_SQRT: return Sqrt(stack) == MATH_SUCCESS ? 0 : 1;
//         case CMD_OUT:  return HandleOut(stack, output_stream);
//         case CMD_HLT:  return 1;
//         default:       return 1;
//     }
//
//     return 0;
// }
//
// int HandleOut(Stack_t* stack, FILE* output_stream)
// {
//     assert(stack != NULL);
//     assert(output_stream != NULL);
//
//     int result = 0;
//     StackErr_t pop_return = StackPop(stack, &result);
//
//     if (pop_return == STACK_SIZE_IS_ZERO)
//     {
//         return 0;
//     }
//     if (pop_return != STACK_SUCCESS)
//     {
//         return 1;
//     }
//
//     fprintf(output_stream, "ANSWER = %d\n", result);
//     return 0;
// }
// //
