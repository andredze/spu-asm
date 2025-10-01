#include "input.h"

int ReadAndParseFile(Context_t* context)
{
    if (OpenFile(&context->input_file_info, "rb"))
    {
        return 0;
    }
    if (ReadText(context))
    {
        DPRINTF("<Error during parsing text>\n");
        return 0;
    }
    fclose(context->input_file_info.stream);

    if (ParseText(context))
    {
        DPRINTF("<ptr_data is a NULL pointer>\n");
        return 0;
    }

    return 0;
}

int OpenFile(FileInfo_t* file_info, const char* mode)
{
    assert(file_info != NULL);
    assert(file_info->filepath != NULL);

    file_info->stream = fopen(file_info->filepath, mode);
    if (file_info->stream == NULL)
    {
        DPRINTF("\n<Can not open the file %s>\n", file_info->filepath);
        return 1;
    }

    return 0;
}

int CountSize(Context_t* context)
{
    assert(context != NULL);
    assert(context->input_file_info.filepath != NULL);

    struct stat file_stat_info = {};

    if (stat(context->input_file_info.filepath, &file_stat_info) == -1)
    {
        DPRINTF("\n<Error occured with stat()>\n");
        return 1;
    }
    context->input_file_info.size = file_stat_info.st_size;
    DPRINTF("size = %zu\n", context->input_file_info.size);

    return 0;
}

int ReadText(Context_t* context)
{
    assert(context != NULL);

    if (CountSize(context))
    {
        return 1;
    }
    if (AllocateBuffer(context))
    {
        return 1;
    }
    if (FillBuffer(context))
    {
        return 1;
    }

    return 0;
}

int AllocateBuffer(Context_t* context)
{
    assert(context != NULL);

    char* buffer = (char*) calloc(context->input_file_info.size / sizeof(char) + 1,
                                  sizeof(char));
    if (buffer == NULL)
    {
        DPRINTF("<Memory allocation for buffer failed>");
        return 1;
    }
    context->buffer_data.buffer = buffer;

    return 0;
}

int FillBuffer(Context_t* context)
{
    assert(context != NULL);
    assert(context->input_file_info.stream != NULL);

    size_t fread_return = fread(context->buffer_data.buffer,
                                sizeof(char),
                                context->input_file_info.size,
                                context->input_file_info.stream);
    DPRINTF("fread return = %llu\n", fread_return);

    if (fread_return != context->input_file_info.size)
    {
        if (ferror(context->input_file_info.stream))
        {
            DPRINTF("\n<Error with reading the file>\n");
            DPRINTF("<fread_return = %zu>\n", fread_return);
            return 1;
        }
        if (feof(context->input_file_info.stream))
        {
            DPRINTF("\n<EOF reached>\n");
            return 1;
        }
    }

    DPRINTF("BUFFER:\n");
#ifdef DEBUG
    puts(context->buffer_data.buffer);
#endif

    // adding NULL-terminator
    context->buffer_data.buffer[context->input_file_info.size] = '\0';

    return 0;
}

int ParseText(Context_t* context)
{
    assert(context != NULL);

    if (CountLines(context))
    {
        return 1;
    }
    if (AllocatePtrdata(context))
    {
        return 1;
    }
    if (FillPtrdata(context))
    {
        return 1;
    }

    return 0;
}

int CountLines(Context_t* context)
{
    assert(context != NULL);

    char* ptr = context->buffer_data.buffer;
    char* endptr = strchr(ptr, '\0');
    int lines_count = 0;

    while (ptr < endptr)
    {
        assert(ptr != NULL);

        ptr = strchr(ptr, '\n');

        assert(ptr != NULL);

        ptr++; // skip '\n'
        lines_count += 1;
    }

    context->buffer_data.lines_count = lines_count;
    context->ptrdata_params.lines_count = lines_count;

    return 0;
}

int AllocatePtrdata(Context_t* context)
{
    assert(context != NULL);

    context->ptrdata_params.ptrdata = (char**) calloc(context->buffer_data.lines_count,
                                                     sizeof(char*));
    if (context->ptrdata_params.ptrdata == NULL)
    {
        DPRINTF("\n<Memory allocation for ptrdata failed>\n");
        return 1;
    }

    return 0;
}

int FillPtrdata(Context_t* context)
{
    assert(context != NULL);

    // fill ptrdata and switch \r and \n to \0
    char* ptr = context->buffer_data.buffer;

    for (int i = 0; i < context->buffer_data.lines_count; i++)
    {
        context->ptrdata_params.ptrdata[i] = ptr;

        assert(ptr != NULL);

        ptr = strchr(ptr, '\n') + 1;
        if (ptr == NULL)
        {
            DPRINTF("\n<Can not find \\n in buffer>\n");
            return 1;
        }
        AddNullTerminators(ptr);
    }

    return 0;
}

void AddNullTerminators(char* ptr)
{
    assert(ptr != NULL);

    *(ptr - 1) = '\0'; // switch \n to \0
    if (*(ptr - 2) == '\r') // if windows file (\r\n at end)
    {
        *(ptr - 2) = '\0'; // switch \r to \0
    }
}
