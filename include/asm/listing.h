#ifndef LISTING_H
#define LISTING_H

#include "assembler.h"
#include "commands.h"
#include "asm_types.h"
#include "colors.h"

AsmErr_t CreateListingFile(InputCtx_t* input_ctx,
                           FileInfo_t* listing_file_info);

AsmErr_t AddStringToListing(CmdCtx_t* cmd_ctx,
                            AsmCtx_t* asm_ctx,
                            FILE* listing_stream);

void DPrintLabels(AsmCtx_t* asm_ctx);

void DPrintAsmData(AsmCtx_t* asm_ctx);

#endif /* LISTING_H */
