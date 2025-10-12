#ifndef LISTING_H
#define LISTING_H

#include "assembler.h"
#include "asm_types.h"

AsmErr_t CreateListingFile(InputCtx_t* commands_data,
                           FileInfo_t* listing_file_info);

AsmErr_t AddStringToListing(CurrCmdData_t* curr_cmd_data,
                            CodeData_t* code_data,
                            FILE* listing_stream);

void DPrintLabels(CodeData_t* code_data);

void DPrintAsmData(CodeData_t* code_data);

#endif /* LISTING_H */
