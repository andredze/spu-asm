#include "listing.h"

AsmErr_t CreateListingFile(InputCtx_t* commands_data,
                           FileInfo_t* listing_file_info)
{
    assert(commands_data != NULL);
    assert(listing_file_info != NULL);

    char listing_filename[MAX_FILENAME_LEN] = {};
    strcpy(listing_filename, "logs/");
    strcat(listing_filename, strchr(commands_data->input_file_info.filepath, '/') + 1);
    strcat(listing_filename, ".lst");
    DPRINTF("listing_filename = %s\n", listing_filename);
    listing_file_info->filepath = listing_filename;

    if (OpenFile(listing_file_info, "w"))
    {
        return ASM_CREATE_LISTING_ERROR;
    }
    fprintf(listing_file_info->stream, "addr\tcmd\t\t\tcode\tvalue\n\n");
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
        if (fprintf(listing_stream, "[%04zu]\t%-10s\t%-4d\t%-4d\n",
                    code_data->cur_cmd,
                    curr_cmd_data->line,
                    curr_cmd_data->command,
                    curr_cmd_data->value) < 0)
        {
            DPRINTF("Fprintf error in listing cmd with 1 arg\n");
            return ASM_LISTING_ERROR;
        }
    }
    else
    {
        if (fprintf(listing_stream, "[%04zu]\t%-10s\t%-4d\n",
                    code_data->cur_cmd,
                    curr_cmd_data->line,
                    curr_cmd_data->command) < 0)
        {
            DPRINTF("Fprintf error in listing cmd with 0 arg\n");
            return ASM_LISTING_ERROR;
        }
    }

    return ASM_SUCCESS;
}

void DPrintLabels(CodeData_t* code_data)
{
    DPRINTF("labels_size = %d;\n", code_data->labels_size);
    DPRINTF("labels = ");
    for (int i = 0; i < code_data->labels_size; i++)
    {
        DPRINTF("%d, ", code_data->labels[i]);
    }
    DPRINTF("\n")
}

void DPrintAsmData(CodeData_t* code_data)
{
    DPRINTF("\n-----------------------------------------------------------------\n"
            "code = ");
    for (size_t i = 0; i < code_data->cur_cmd; i++)
    {
        DPRINTF("%d, ", code_data->buffer[i]);
    }
    DPRINTF("\n"
            "labels = ");
    for (int i = 0; i < code_data->labels_size; i++)
    {
        DPRINTF("%d, ", code_data->labels[i]);
    }
    DPRINTF("\n"
            "-----------------------------------------------------------------\n");
}
