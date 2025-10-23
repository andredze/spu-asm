#include "listing.h"

//------------------------------------------------------------------------------------------

AsmErr_t CreateListingFile(InputCtx_t* input_ctx,
                           FileInfo_t* listing_file_info)
{
    assert(input_ctx != NULL);
    assert(listing_file_info != NULL);

    char listing_filename[MAX_FILENAME_LEN] = {};
    strcpy(listing_filename, "logs/");

    const char* filename_start = input_ctx->input_file_info.filepath;
    if ((filename_start = strchr(input_ctx->input_file_info.filepath, '/')) != NULL)
    {
        strcat(listing_filename, filename_start + 1);
    }
    else
    {
        strcat(listing_filename, input_ctx->input_file_info.filepath);
    }
    strcat(listing_filename, ".lst");
    DPRINTF("listing_filename = %s\n", listing_filename);
    listing_file_info->filepath = listing_filename;

    if (OpenFile(listing_file_info, "w"))
    {
        return ASM_CREATE_LISTING_ERROR;
    }

    fprintf(listing_file_info->stream, "addr\tcmd\t\t\tcode\tvalue\n\n");
    DPRINTF("FILE = %p\n", listing_file_info->stream);
    printf(LIGHT_YELLOW "Created listing file: %s\n" RESET_CLR, listing_filename);

    return ASM_SUCCESS;
}

//------------------------------------------------------------------------------------------

AsmErr_t AddStringToListing(CmdCtx_t* cmd_ctx,
                            AsmCtx_t* asm_ctx,
                            FILE* listing_stream)
{
    assert(cmd_ctx != NULL);
    assert(asm_ctx != NULL);

    if (CMD_CASES[cmd_ctx->index].args_count == 2)
    {
        if (fprintf(listing_stream, "[%04zu]\t%-10s\t%-4d\t%-4d\n",
                    asm_ctx->cur_cmd,
                    cmd_ctx->line,
                    cmd_ctx->command,
                    cmd_ctx->value) < 0)
        {
            DPRINTF("Fprintf error in listing cmd with 1 arg\n");
            return ASM_LISTING_ERROR;
        }
    }
    else
    {
        if (fprintf(listing_stream, "[%04zu]\t%-10s\t%-4d\n",
                    asm_ctx->cur_cmd,
                    cmd_ctx->line,
                    cmd_ctx->command) < 0)
        {
            DPRINTF("Fprintf error in listing cmd with 0 arg\n");
            return ASM_LISTING_ERROR;
        }
    }

#ifdef ASM_DEBUG
    fflush(listing_stream);
#endif /* ASM_DEBUG */
    return ASM_SUCCESS;
}

//------------------------------------------------------------------------------------------

void DPrintLabels(AsmCtx_t* asm_ctx)
{
    DPRINTF("labels_size = %d;\n", asm_ctx->labels_size);
    DPRINTF("labels = ");
    for (int i = 0; i < asm_ctx->labels_size; i++)
    {
        DPRINTF("%zu, ", asm_ctx->labels[i]);
    }
    DPRINTF("\n")
}

//------------------------------------------------------------------------------------------

void DPrintAsmData(AsmCtx_t* asm_ctx)
{
    DPRINTF("\n-----------------------------------------------------------------\n"
            "code = ");
    for (size_t i = 0; i < asm_ctx->cur_cmd; i++)
    {
        DPRINTF("%d, ", asm_ctx->buffer[i]);
    }
    DPRINTF("\n"
            "labels = ");
    for (int i = 0; i < asm_ctx->labels_size; i++)
    {
        DPRINTF("%zu, ", asm_ctx->labels[i]);
    }
    DPRINTF("\n-----------------------------------------------------------------\n");
}

//------------------------------------------------------------------------------------------
